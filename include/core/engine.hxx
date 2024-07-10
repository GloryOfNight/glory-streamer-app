#pragma once

#include "core/object.hxx"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <chrono>
#include <map>
#include <memory>

namespace gl::app
{
	class engine
	{
	public:
		static engine* get();

		engine();
		~engine();

		bool init();
		void run();
		void stop();

		void shutdown();

		TTF_Font* LoadFont(const std::string& fontPath, int32_t fontSize);

		SDL_Texture* LoadTexture(const std::string& texturePath);

		template <typename T, typename... Args>
		T* createObject(Args&&... args)
		{
			T* obj = new T(args...);
			mObjects.push_back(std::unique_ptr<object>(obj));
			return obj;
		}

	private:
		void pollEvents();

		SDL_Window* mWindow{};

		SDL_Renderer* mRenderer{};

		SDL_Texture* mTexture{};

		SDL_Texture* mFontTexture{};

		std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime{};

		std::map<std::string, TTF_Font*> mFonts{};

		std::map<std::string, SDL_Texture*> mTextures{};

		std::vector<std::unique_ptr<object>> mObjects{};

		bool mRunning{};
	};
} // namespace gl::app