#include "engine.hxx"

#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <thread>

gl::app::engine::engine()
{
}

gl::app::engine::~engine()
{
}

bool gl::app::engine::init()
{
	if (SDL_CreateWindowAndRenderer(1920, 1080, 0, &mWindow, &mRenderer))
	{
		std::cerr << "SDL_CreateWindowAndRenderer failed with: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetWindowTitle(mWindow, "Glory streamer app");
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

	mTexture = IMG_LoadTexture(mRenderer, "assets/sprites/ghost/first_test_ghost.png");

	TTF_Font* font = TTF_OpenFont("assets/fonts/Buran USSR.ttf", 80);

	SDL_Surface* surf = TTF_RenderUTF8_Blended(font, "русская банька матрешка", SDL_Color{255, 255, 255, 255});
	mFontTexture = SDL_CreateTextureFromSurface(mRenderer, surf);

	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);

	return true;
}

void gl::app::engine::run()
{
	mRunning = true;

	mLastFrameTime = std::chrono::high_resolution_clock::now();

	while (mRunning)
	{
		pollEvents();

		const auto now = std::chrono::high_resolution_clock::now();
		const double delta = std::chrono::duration<double>(now - mLastFrameTime).count();
		mLastFrameTime = now;

		SDL_RenderClear(mRenderer);

		SDL_Rect src;
		src.h = 512;
		src.w = 512;
		src.x = 0;
		src.y = 0;

		SDL_Rect dst = src;
		//dst.h = 512;
		//dst.w = 512;
		//dst.x = dst.x + 100 * delta;
		//dst.y = dst.y + 100 * delta;

		SDL_RenderCopy(mRenderer, mTexture, &src, &dst);

		src.h = 128;
		src.w = 512;
		src.x = 0;
		src.y = 0;

		dst = src;

		SDL_RenderCopy(mRenderer, mFontTexture, &src, &dst);

		SDL_RenderPresent(mRenderer);

		std::this_thread::sleep_for(std::chrono::milliseconds(41));
	}
}

void gl::app::engine::stop()
{
	mRunning = false;
}

void gl::app::engine::shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
}

void gl::app::engine::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mRunning = false;
			break;
		}
	}
}
