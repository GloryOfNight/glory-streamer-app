#include "api/twitch_api.hxx"

#include <curl/curl.h>
#include <format>
#include <httplib.h>
#include <nlohmann/json.hpp>

static const char* OAUTH2_REDIRECT_URL = "http://localhost:8081/oauth2callback";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string exchangeTwitchAuthCodeForAccessToken(const std::string& clientId, const std::string& clientSecret, const std::string& authCode, const std::string& redirectUri)
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl)
	{
		const std::string tokenUrl = "https://id.twitch.tv/oauth2/token";
		const std::string postFields = "client_id=" + clientId +
									   "&client_secret=" + clientSecret +
									   "&code=" + authCode +
									   "&grant_type=authorization_code"
									   "&redirect_uri=" + redirectUri;

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

std::pair<bool, ttv::api::auth_info> ttv::api::initialAuth(const std::string clientId, const std::string clientSecret)
{
	bool bSuccess = false;
	auth_info auth{};

	std::cout << "========================\n"
			  << std::endl;
	const std::string uri = std::format("https://id.twitch.tv/oauth2/authorize?response_type=code&client_id={0}&redirect_uri={1}&scope=channel:read:subscriptions+moderator:read:chatters&force_verify=true", clientId, OAUTH2_REDIRECT_URL);
	std::cout << uri << std::endl;
	std::cout << "\n========================" << std::endl;
	std::cout << "Please follow to link above and process to gAuth" << std::endl;
	std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;

	httplib::Server localServer;

	const auto callbackLam = [&](const httplib::Request& req, httplib::Response& res)
	{
		const std::string authCode = req.get_param_value("code");

		if (authCode.empty())
		{
			res.set_content("Authorization failure! You can close this window.", "text/plain");
			localServer.stop();
			return;
		}

		// Exchange the authorization code for an access token
		std::string tokenResponse = exchangeTwitchAuthCodeForAccessToken(clientId, clientSecret, authCode, OAUTH2_REDIRECT_URL);
		if (tokenResponse.empty())
		{
			res.set_content("Authorization failure! You can close this window.", "text/plain");
		}
		else
		{
			const auto jsonAuth = nlohmann::json::parse(tokenResponse);

			if (jsonAuth.contains("error"))
			{
				const std::string error = jsonAuth["error"];
				const std::string errorDetails = jsonAuth["error_description"];
				res.set_content(std::format("Authorization failure! {0} ({1})", error, errorDetails), "text/plain");
				localServer.stop();
				return;
			}

			auth.accessToken = jsonAuth["access_token"];
			auth.expiresIn = jsonAuth["expires_in"];
			auth.refreshToken = jsonAuth["refresh_token"];
			auth.scope = jsonAuth["scope"];
			auth.tokenType = jsonAuth["token_type"];

			bSuccess = true;

			res.set_content("Authorization successful! You can close this window.", "text/plain");
		}

		localServer.stop();
	};

	localServer.Get("/oauth2callback", callbackLam);
	localServer.listen("localhost", 8081);

	return std::pair<bool, auth_info>{bSuccess, auth};
}
