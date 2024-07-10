#define SDL_MAIN_HANDLED

#include "engine.hxx"
#include "youtube_data_api.hxx"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[], char* envp[])
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Failed to initialize SDL" << SDL_GetError() << std::endl;
		return -1;
	}

	if (TTF_Init())
	{
		std::cerr << "Failed to initialize TTF" << TTF_GetError() << std::endl;
		return -1;
	}

	gl::app::engine eng{};
	if (!eng.init())
	{
		std::cerr << "Failed to initialize engine" << std::endl;
		return -1;
	}

	eng.run();

	eng.shutdown();

	const std::string clientId = "1002066649738-3gfrfvhfdt9q2j3n7vq1ufkdlav603a9.apps.googleusercontent.com";
	const std::string clientSecret = "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";

	auto [bAuth, authInfo] = yt::data::api::beginAuth(clientId, clientSecret);

	if (!bAuth)
	{
		std::cerr << "Failed to authenticate" << std::endl;
		return -1;
	}

	const std::string recent = yt::data::api::fetchSubscribers(clientSecret, authInfo.accessToken, false, 50);

	const auto recentJson = nlohmann::json::parse(recent);

	std::vector<std::string> titles;
	for (const auto& item : recentJson["items"])
	{
		titles.push_back(item["subscriberSnippet"]["title"]);
	}

	TTF_Quit();
	SDL_Quit();
	return 0;
}
