#define SDL_MAIN_HANDLED

#include "core/engine.hxx"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <curl/curl.h>
#include <iostream>
#include <thread>

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

	if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
	{
		std::cerr << "Failed to initialize CURL" << std::endl;
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

	curl_global_cleanup();
	TTF_Quit();
	SDL_Quit();
	return 0;
}
