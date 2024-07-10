#pragma once

#include <SDL2/SDL.h>
#include <chrono>

namespace gl::app
{
	class engine
	{
	public:
		engine();
		~engine();

		bool init();
		void run();
		void stop();

		void shutdown();

	private:
		void pollEvents();

		SDL_Window* mWindow{};

		SDL_Renderer* mRenderer{};

		SDL_Texture* mTexture{};

		SDL_Texture* mFontTexture{};

		std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime{};

		bool mRunning{};
	};
} // namespace gl::app