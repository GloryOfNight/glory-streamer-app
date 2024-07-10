#include "youtube_data_api.hxx"

#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_opengl.h>
#include <codecvt>
#include <format>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[], char* envp[])
{
	std::locale::global(std::locale("en_US.utf8"));

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

	return 0;
}