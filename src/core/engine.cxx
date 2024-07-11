#include "core/engine.hxx"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <thread>

static gl::app::engine* gEngine{nullptr};

gl::app::engine* gl::app::engine::get()
{
	return gEngine;
}

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

	gEngine = this;

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

		for (auto& object : mObjects)
		{
			object->update(delta);
		}

		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
		SDL_RenderClear(mRenderer);

		for (auto& object : mObjects)
		{
			object->draw(mRenderer);
		}

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
	mObjects.clear();

	for (auto& [name, font] : mFonts)
	{
		TTF_CloseFont(font);
	}

	for (auto& [name, texture] : mTextures)
	{
		SDL_DestroyTexture(texture);
	}

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	gEngine = nullptr;
}

TTF_Font* gl::app::engine::LoadFont(const std::string& fontPath, int32_t fontSize)
{
	const std::string fontStoreName = std::format("{0}|{1}", fontPath, fontSize);

	if (mFonts.contains(fontStoreName))
	{
		return mFonts.at(fontStoreName);
	}

	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);

	if (font)
	{
		mFonts.emplace(fontStoreName, font);
	}

	return font;
}

SDL_Texture* gl::app::engine::LoadTexture(const std::string& texturePath)
{
	if (mTextures.contains(texturePath))
	{
		return mTextures.at(texturePath);
	}

	SDL_Texture* texture = IMG_LoadTexture(mRenderer, "assets/sprites/ghost/first_test_ghost.png");
	if (texture)
	{
		mTextures.emplace(texturePath, texture);
	}

	return texture;
}

void gl::app::engine::getWindowSize(int32_t* width, int32_t* height)
{
	SDL_GetWindowSize(mWindow, width, height);
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
