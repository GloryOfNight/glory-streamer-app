#define SDL_MAIN_HANDLED

#include "api/youtube_api.hxx"
#include "core/engine.hxx"
#include "objects/subscriber_ghost.hxx"
#include "objects/youtube_manager.hxx"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
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

	gl::app::engine eng{};

	if (!eng.init())
	{
		std::cerr << "Failed to initialize engine" << std::endl;
		return -1;
	}

	eng.createObject<gl::app::youtube_manager>();

	eng.run();

	eng.shutdown();

	TTF_Quit();
	SDL_Quit();
	return 0;
}
