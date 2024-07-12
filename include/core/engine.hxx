#pragma once

#include "core/object.hxx"
#include "core/timer_manager.hxx"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <memory>
#include <string>

namespace gl::app
{
	class engine
	{
	public:
		static engine* get();

		engine();
		engine(const engine&) = delete;
		engine(engine&&) = delete;
		~engine();

		engine& operator=(const engine&) = delete;
		engine& operator=(engine&&) = delete;

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
			mObjects.push_back(obj);

			obj->init();

			return obj;
		}

		void getWindowSize(int32_t* width, int32_t* height);

		const std::vector<object*>& getObjects() const { return mObjects; }

		timer_manager* getTimerManager() const { return mTimerManager.get(); }

	private:
		void pollEvents();

		SDL_Window* mWindow{};

		SDL_Renderer* mRenderer{};

		std::map<std::string, SDL_Texture*> mTextures{};

		std::vector<object*> mObjects{};

		std::unique_ptr<timer_manager> mTimerManager{};

		bool mRunning{};
	};
} // namespace gl::app