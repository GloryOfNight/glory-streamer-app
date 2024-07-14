#include "core/engine.hxx"

#include "objects/subscriber_ghost.hxx"
#include "objects/youtube_manager.hxx"

#include <SDL2/SDL_syswm.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <format>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
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
	if (SDL_CreateWindowAndRenderer(2560, 1440, SDL_WINDOW_MINIMIZED, &mWindow, &mRenderer))
	{
		std::cerr << "SDL_CreateWindowAndRenderer failed with: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetWindowTitle(mWindow, "Glory streamer app");

	mTimerManager = std::unique_ptr<timer_manager>(new timer_manager());

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	auto newFont = io.Fonts->AddFontFromFileTTF("assets/fonts/Arsenal-Regular.ttf", 16, nullptr, io.Fonts->GetGlyphRangesCyrillic());

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(mWindow, mRenderer);
	ImGui_ImplSDLRenderer2_Init(mRenderer);

	gEngine = this;

	return true;
}

void gl::app::engine::run()
{
	mRunning = true;

	const int32_t frameTimeMs = 1000 / 60;

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

		auto copyObjects = mObjects;
		for (auto& object : copyObjects)
		{
			object->update(deltaSeconds);
		}

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (bShowObjectInspector)
		{
			showObjectInspector();
		}
		if (bShowYoutubeManagerInspector)
		{
			showYoutubeManagerExpector();
		}

		ImGui::Render();

		SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mRenderer);

		copyObjects = mObjects;
		for (auto& object : copyObjects)
		{
			object->draw(mRenderer);
		}

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), mRenderer);

		SDL_RenderPresent(mRenderer);
	}
}

void gl::app::engine::stop()
{
	mRunning = false;
}

void gl::app::engine::shutdown()
{
	for (auto& object : mObjects)
	{
		delete object;
	}
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

bool gl::app::engine::removeObject(object* obj)
{
	auto iter = std::find(mObjects.begin(), mObjects.end(), obj);
	const bool bFind = iter != mObjects.end();
	if (bFind)
	{
		mObjects.erase(iter);
		delete obj;
	}
	return bFind;
}

void gl::app::engine::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_0 && !bShowObjectInspector)
			{
				bShowObjectInspector = true;
			}
			else if (event.key.keysym.sym == SDLK_9 && !bShowYoutubeManagerInspector)
			{
				bShowYoutubeManagerInspector = true;
			}
			break;
		case SDL_QUIT:
			mRunning = false;
			break;
		}
	}
}

void gl::app::engine::showObjectInspector()
{
	ImGui::Begin("Objects inspector", &bShowObjectInspector);

	ImGui::Text("Spawn new ghost");

	static char newGhostName[24] = {0};
	ImGui::InputText("Name", newGhostName, sizeof(newGhostName));
	ImGui::SameLine();
	static float newGhostSpeed = 60.0;
	ImGui::InputFloat("Speed", &newGhostSpeed, 0.f, 0.f, "%.1f");

	if (ImGui::Button("Spawn ghost"))
	{
		auto newGhost = createObject<subscriber_ghost>(newGhostName, std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
		newGhost->setSpeed(newGhostSpeed);
	}

	ImGui::Separator();

	auto& ghostBox = subscriber_ghost::getGhostBox();

	ImGui::Text("Ghost box limits");
	float limitHorz[2] = {static_cast<float>(ghostBox.x), static_cast<float>(ghostBox.w)};
	float limitVert[2] = {static_cast<float>(ghostBox.y), static_cast<float>(ghostBox.h)};
	ImGui::InputFloat2("Limit X", limitHorz);
	ImGui::InputFloat2("Limit Y", limitVert);

	if (limitHorz[0] < limitHorz[1])
		ghostBox.x = limitHorz[0];

	if (limitHorz[0] < limitHorz[1])
		ghostBox.w = limitHorz[1];

	if (limitVert[0] < limitVert[1])
		ghostBox.y = limitVert[0];

	if (limitVert[0] < limitVert[1])
		ghostBox.h = limitVert[1];

	ImGui::Separator();
	ImGui::Text("Objects (Total %i)", static_cast<int32_t>(mObjects.size()));

	static char newMessage[512] = {0};
	ImGui::InputText("Custom message", newMessage, sizeof(newMessage));

	auto copyObjects = mObjects;
	for (auto& object : copyObjects)
	{
		auto ghost = dynamic_cast<subscriber_ghost*>(object);

		if (ghost)
		{
			const auto channelId = ghost->getChannelId();

			ImGui::Text("Ghost - %s (%s)", ghost->getTitle().c_str(), ghost->getChannelId().c_str());

			double x, y;
			ghost->getPos(&x, &y);

			float pos[2] = {static_cast<float>(x), static_cast<float>(y)};
			ImGui::InputFloat2((std::string("Position##") + channelId).c_str(), pos, "%.1f");

			float speed = ghost->getSpeed();
			ImGui::InputFloat((std::string("Speed##") + channelId).c_str(), &speed, 0, 0, "%.1f");

			ghost->setPos(pos[0], pos[1]);
			ghost->setSpeed(speed);

			bool bHide = ghost->isHidden();
			ImGui::Checkbox((std::string("Hide##") + channelId).c_str(), &bHide);
			ghost->setHidden(bHide);

			ImGui::SameLine();

			if (ImGui::Button((std::string("Remove##") + channelId).c_str()))
			{
				removeObject(ghost);
			}

			ImGui::SameLine();

			if (ImGui::Button((std::string("Set Message##") + channelId).c_str()))
			{
				ghost->setMessage(newMessage);
			}

			ImGui::Separator();
		}
	};

	ImGui::End();
}

void gl::app::engine::showYoutubeManagerExpector()
{
	const auto pred = [](const object* obj)
	{
		return dynamic_cast<const youtube_manager*>(obj) != nullptr;
	};

	auto iter = std::find_if(mObjects.begin(), mObjects.end(), pred);

	if (iter == mObjects.end())
		return;

	auto ytManager = dynamic_cast<youtube_manager*>(*iter);

	ImGui::Begin("Youtube manager inspector", &bShowYoutubeManagerInspector);

	ImGui::Text("Youtube manager");

	ImGui::Separator();

	ImGui::Text("Latest subscribers");

	const auto& subs = ytManager->getSubscribers();
	for (const auto& sub : subs)
	{
		ImGui::Text("Subscriber - %s (%s) at %s", sub.title.c_str(), sub.channelId.c_str(), sub.publishedAt.c_str());
	}
	ImGui::Separator();

	ImGui::Text("Broadcasts");
	const auto& broadcasts = ytManager->getBroadcasts();
	for (const auto& broadcast : broadcasts)
	{
		ImGui::Text("Broadcast - %s (%s)", broadcast.id.c_str(), broadcast.lifeCycleStatus.c_str());
	}
	ImGui::Separator();

	const auto& chatMessages = ytManager->getLiveChatMessages();

	ImGui::Text("Chat messages");

	for (const auto& chatMessage : chatMessages)
	{
		ImGui::Text("%s (%s): %s", chatMessage.displayName.c_str(), chatMessage.channelId.c_str(), chatMessage.displayMessage.c_str());
	}

	ImGui::End();
}
