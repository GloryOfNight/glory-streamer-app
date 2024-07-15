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
	class engine final
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

		SDL_Texture* LoadTexture(const std::string& texturePath);

		template <typename T, typename... Args>
		T* createObject(Args&&... args)
		{
			T* obj = new T(args...);
			mObjects.push_back(std::unique_ptr<object>(obj));

			obj->init();

			return obj;
		}

		void getWindowSize(int32_t* width, int32_t* height);

		const std::vector<std::unique_ptr<object>>& getObjects() const { return mObjects; }

		bool removeObject(object* obj);

	private:
		void pollEvents();

		void createSubsystems();

		void showObjectInspector();

		void showYoutubeManagerExpector();

		SDL_Window* mWindow{};

		SDL_Renderer* mRenderer{};

		std::map<std::string, SDL_Texture*> mTextures{};

		std::vector<std::unique_ptr<object>> mObjects{};

		std::vector<object*> mObjectsToRemove{};

		bool bShowObjectInspector{};

		bool bShowYoutubeManagerInspector{};

		bool mRunning{};
	};
} // namespace gl::app