#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_opengl.h>
#include <codecvt>
#include <curl/curl.h>
#include <format>
#include <httplib.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <nlohmann/json.hpp>

struct authResponse
{
	std::string accessToken{};
	std::string expiresIn{};
	std::string refreshToken{};
	std::string scope{};
	std::string tokenType{};
};

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

authResponse DoOAuth(const std::string& clientId, const std::string& clientSecret)
{
	authResponse auth{};

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
		res.set_content("Authorization successful! You can close this window.", "text/plain");

		localServer.stop();

		const auto jsonAuth = nlohmann::json::parse(tokenResponse);

		auth.accessToken = jsonAuth["access_token"];
		//auth.expiresIn = jsonAuth["expires_in"];
		//auth.refreshToken = jsonAuth["refresh_token"];
		//auth.scope = jsonAuth["scope"];
		//auth.tokenType = jsonAuth["token_type"];
	};

	localServer.Get("/callback", callbackLam);

	localServer.listen("localhost", 8080);

	return auth;
}

std::string fetchRecentSubscribers(const std::string& apiKey, const std::string& accessToken)
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl)
	{
		// Construct the URL
		std::string url = "https://youtube.googleapis.com/youtube/v3/subscriptions?part=subscriberSnippet&myRecentSubscribers=true&key=" + apiKey;

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
int main(int argc, char* argv[], char* envp[])
{
	const std::string clientId = "1002066649738-3gfrfvhfdt9q2j3n7vq1ufkdlav603a9.apps.googleusercontent.com";
	const std::string clientSecret = "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";

	auto NewAuth = DoOAuth(clientId, clientSecret);

	const std::string recent = fetchRecentSubscribers(clientSecret, NewAuth.accessToken);

	const auto recentJson = nlohmann::json::parse(recent);

	std::vector<std::string> titles;
	for (const auto& item : recentJson["items"])
	{
		titles.push_back(item["subscriberSnippet"]["title"]);
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter{};

	for (const auto& title : titles)
	{
		const std::wstring wtitle = converter.from_bytes(title.c_str());
		std::wcout << wtitle << std::endl;
	}

	return 0;
}