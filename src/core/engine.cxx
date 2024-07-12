#include "core/engine.hxx"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <format>
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

	mTimerManager = std::unique_ptr<timer_manager>(new timer_manager());

	gEngine = this;

	return true;
}

void gl::app::engine::run()
{
	mRunning = true;

	const int32_t frameTimeMs = 1000 / 30;

	static uint32_t nowTicks = SDL_GetTicks();
	static uint32_t nextTick = nowTicks;
	static uint32_t prevTick = nowTicks;

	while (mRunning)
	{
		uint32_t nowTicks = SDL_GetTicks();
		if (nextTick > nowTicks)
		{
			SDL_Delay(nextTick - nowTicks);
		}
		nowTicks = SDL_GetTicks();
		const uint32_t elapsedMs = nowTicks - prevTick;

		prevTick = nowTicks;
		nextTick += frameTimeMs;

		const double deltaSeconds = elapsedMs / 1000.;

		pollEvents();

		mTimerManager->update(deltaSeconds);

		for (auto& object : mObjects)
		{
			object->update(deltaSeconds);
		}

		SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mRenderer);

		for (auto& object : mObjects)
		{
			object->draw(mRenderer);
		}

		SDL_RenderPresent(mRenderer);
	}
}

void gl::app::engine::stop()
{
	mRunning = false;
}

void gl::app::engine::shutdown()
{
	mObjects.clear();

	for (auto& [name, texture] : mTextures)
	{
		SDL_DestroyTexture(texture);
	}

	mTimerManager.reset();

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	gEngine = nullptr;
}

SDL_Texture* gl::app::engine::LoadTexture(const std::string& texturePath)
{
	if (mTextures.contains(texturePath))
	{
		return mTextures.at(texturePath);
	}

	SDL_Texture* texture = IMG_LoadTexture(mRenderer, texturePath.c_str());
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
