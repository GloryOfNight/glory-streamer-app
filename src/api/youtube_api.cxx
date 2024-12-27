#include "api/youtube_api.hxx"

#include <curl/curl.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <httplib.h>
#include <nlohmann/json.hpp>

static const char* OAUTH2_REDIRECT_URL = "http://localhost:8080/oauth2callback";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string exchangeAuthCodeForAccessToken(const std::string& clientId, const std::string& clientSecret, const std::string& authCode, const std::string& redirectUri)
{
	std::string readBuffer;

	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::string tokenUrl = "https://oauth2.googleapis.com/token";
		std::string postFields = "code=" + authCode + "&client_id=" + clientId + "&client_secret=" + clientSecret + "&redirect_uri=" + redirectUri + "&grant_type=authorization_code";

		curl_easy_setopt(curl, CURLOPT_URL, tokenUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		const CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	return readBuffer;
}

std::pair<bool, yt::api::auth_info> yt::api::initalAuth(const std::string clientId, const std::string clientSecret)
{
	const std::filesystem::path cacheFilePath = std::filesystem::temp_directory_path() / std::filesystem::path("glory-app-cache/google.json");
	const std::string cacheFilePathStr = cacheFilePath.generic_string();
	std::fstream cacheStream{};

	std::filesystem::create_directory(std::filesystem::temp_directory_path() / std::filesystem::path("glory-app-cache"));

	cacheStream.open(cacheFilePathStr, std::ios::in);
	if (cacheStream.is_open())
	{
		nlohmann::json cacheJson{};
		cacheStream >> cacheJson;

		cacheStream.close();

		const std::string refreshToken = cacheJson.contains("refresh_token") ? cacheJson["refresh_token"].template get<std::string>() : std::string();
		if (!refreshToken.empty())
		{
			const auto refreshRes = refreshAuth(clientId, clientSecret, refreshToken);
			if (refreshRes.first) // bSuccess
			{
				return std::move(refreshRes);
			}
		}
	}

	bool bSuccess = false;
	auth_info auth{};

	std::cout << "========================\n"
			  << std::endl;
	const std::string uri = std::format("https://accounts.google.com/o/oauth2/v2/auth?scope=https://www.googleapis.com/auth/youtube&access_type=offline&include_granted_scopes=true&response_type=code&client_id={0}&redirect_uri={1}", clientId, OAUTH2_REDIRECT_URL);
	std::cout << uri << std::endl;
	std::cout << "\n========================" << std::endl;
	std::cout << "Please follow to link above and process to gAuth" << std::endl;
	std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;

	httplib::Server localServer;

	const auto callbackLam = [&](const httplib::Request& req, httplib::Response& res)
	{
		const std::string authCode = req.get_param_value("code");

		// Exchange the authorization code for an access token
		std::string tokenResponse = exchangeAuthCodeForAccessToken(clientId, clientSecret, authCode, OAUTH2_REDIRECT_URL);
		if (tokenResponse.empty())
		{
			res.set_content("Authorization failure! You can close this window.", "text/plain");
		}
		else
		{
			res.set_content("Authorization successful! You can close this window.", "text/plain");

			const auto jsonAuth = nlohmann::json::parse(tokenResponse);

			auth.accessToken = jsonAuth["access_token"];
			auth.expiresIn = jsonAuth["expires_in"];
			if (jsonAuth.contains("refresh_token"))
				auth.refreshToken = jsonAuth["refresh_token"];
			auth.scope = jsonAuth["scope"];
			auth.tokenType = jsonAuth["token_type"];

			bSuccess = true;
		}

		localServer.stop();
	};

	localServer.Get("/oauth2callback", callbackLam);
	localServer.listen("localhost", 8080);

	if (bSuccess && !auth.refreshToken.empty())
	{
		cacheStream.open(cacheFilePathStr, std::ios::out | std::ios::trunc);

		nlohmann::json cacheJson{};
		cacheJson["refresh_token"] = auth.refreshToken;

		cacheStream << cacheJson.dump();

		cacheStream.close();
	}

	return std::pair<bool, auth_info>{bSuccess, auth};
}

std::pair<bool, yt::api::auth_info> yt::api::refreshAuth(const std::string clientId, const std::string clientSecret, const std::string refreshToken)
{
	bool bSuccess = false;
	auth_info auth{};

	CURL* curl = curl_easy_init();
	if (curl)
	{
		const std::string url = std::format("https://oauth2.googleapis.com/token");
		const std::string postFields = std::format("client_id={0}&client_secret={1}&refresh_token={2}&grant_type=refresh_token", clientId, clientSecret, refreshToken);

		std::string readBuffer{};

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		const CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else if (!readBuffer.empty())
		{
			const auto jsonAuth = nlohmann::json::parse(readBuffer);

			auth.accessToken = jsonAuth["access_token"];
			auth.expiresIn = jsonAuth["expires_in"];
			auth.scope = jsonAuth["scope"];
			auth.tokenType = jsonAuth["token_type"];

			if (jsonAuth.contains("refresh_token"))
			{
				auth.refreshToken = jsonAuth["refresh_token"];
			}
			else
			{
				auth.refreshToken = refreshToken;
			}

			bSuccess = true;
		}

		curl_easy_cleanup(curl);
	}

	return std::pair<bool, auth_info>{bSuccess, auth};
}

std::string yt::api::fetch(const std::string url, const std::string accessToken, const std::string eTag)
{
	std::string headerBuffer;
	std::string responseBuffer;

	CURL* curl = curl_easy_init();
	if (curl)
	{
		// Set the URL
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Set the Authorization header
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, std::string("Authorization: Bearer " + accessToken).c_str());
		headers = curl_slist_append(headers, "Accept: application/json");
		if (!eTag.empty())
			headers = curl_slist_append(headers, std::string("If-None-Match: " + eTag).c_str());

		if (curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip") == CURLE_OK)
			headers = curl_slist_append(headers, "Content-Encoding: gzip");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &headerBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

		// Perform the request
		const CURLcode res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		// Clean up
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	return responseBuffer;
}

std::string yt::api::post(const std::string url, const std::string accessToken, const std::string postJson)
{
	std::string headerBuffer;
	std::string responseBuffer;

	CURL* curl = curl_easy_init();
	if (curl)
	{
		// Set the URL
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postJson.c_str());

		// Set the Authorization header
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, std::string("Authorization: Bearer " + accessToken).c_str());
		headers = curl_slist_append(headers, std::string("Accept: application/json").c_str());
		headers = curl_slist_append(headers, std::string("Content-Type: application/json").c_str());

		if (curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip") == CURLE_OK)
			headers = curl_slist_append(headers, "Content-Encoding: gzip");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &headerBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

		// Perform the request
		const CURLcode res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		// Clean up
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	return responseBuffer;
}
