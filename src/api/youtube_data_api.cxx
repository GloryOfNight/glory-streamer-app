#include "api/youtube_api.hxx"

#include <curl/curl.h>
#include <format>
#include <httplib.h>
#include <nlohmann/json.hpp>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string exchangeAuthCodeForAccessToken(const std::string& clientId, const std::string& clientSecret, const std::string& authCode, const std::string& redirectUri)
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl)
	{
		std::string tokenUrl = "https://oauth2.googleapis.com/token";
		std::string postFields = "code=" + authCode + "&client_id=" + clientId + "&client_secret=" + clientSecret + "&redirect_uri=" + redirectUri + "&grant_type=authorization_code";

		curl_easy_setopt(curl, CURLOPT_URL, tokenUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	return readBuffer;
}

std::pair<bool, yt::data::api::auth_info> yt::data::api::beginAuth(const std::string& clientId, const std::string& clientSecret)
{
	bool bSuccess = false;
	auth_info auth{};

	const std::string redirectUri = "http://localhost:8080/callback";

	std::cout << "========================\n"
			  << std::endl;
	const std::string uri = std::format("https://accounts.google.com/o/oauth2/v2/auth?scope=https://www.googleapis.com/auth/youtube.readonly&access_type=offline&include_granted_scopes=true&response_type=code&client_id={0}&redirect_uri={1}", clientId, redirectUri);
	std::cout << uri << std::endl;
	std::cout << "\n========================" << std::endl;
	std::cout << "Please follow to link above and process to gAuth" << std::endl;
	std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;

	httplib::Server localServer;

	const auto callbackLam = [&](const httplib::Request& req, httplib::Response& res)
	{
		auto authCode = req.get_param_value("code");

		// Exchange the authorization code for an access token
		std::string tokenResponse = exchangeAuthCodeForAccessToken(clientId, clientSecret, authCode, redirectUri);
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
			auth.refreshToken = jsonAuth["refresh_token"];
			auth.scope = jsonAuth["scope"];
			auth.tokenType = jsonAuth["token_type"];

			bSuccess = true;
		}

		localServer.stop();
	};

	localServer.Get("/callback", callbackLam);
	localServer.listen("localhost", 8080);

	return std::pair<bool, auth_info>{bSuccess, auth};
}

std::string yt::data::api::fetchSubscribers(const std::string& apiKey, const std::string& accessToken, bool bRecent, uint8_t maxResults)
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl)
	{
		std::string subFilter = bRecent ? "myRecentSubscribers=true" : "mySubscribers=true";

		std::string url = std::format("https://youtube.googleapis.com/youtube/v3/subscriptions?part=subscriberSnippet&{0}&maxResults={1}&key={2}", subFilter, maxResults, apiKey);

		// Set the URL
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Set the Authorization header
		struct curl_slist* headers = NULL;
		std::string bearerToken = "Authorization: Bearer " + accessToken;
		headers = curl_slist_append(headers, bearerToken.c_str());
		headers = curl_slist_append(headers, "Accept: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Set the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

		// Set the user pointer to pass to the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		// Perform the request
		res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		// Clean up
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	return readBuffer;
}
