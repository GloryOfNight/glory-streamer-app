#include "core/engine.hxx"

#include "actors/chat_ghost.hxx"
#include "actors/pony.hxx"
#include "assets/sprite_asset.hxx"
#include "core/engine.hxx"
#include "subsystems/chat_ghosts_manager.hxx"
#include "subsystems/timer_manager.hxx"
#include "subsystems/twitch_manager.hxx"
#include "subsystems/youtube_manager.hxx"

#include <SDL3/SDL_system.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <format>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <stdexcept>
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
	if (!SDL_CreateWindowAndRenderer("Glorious App", 2560, 1440, SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT, &mWindow, &mRenderer))
	{
		std::cerr << "SDL_CreateWindowAndRenderer failed with: " << SDL_GetError() << std::endl;
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	auto newFont = io.Fonts->AddFontFromFileTTF("assets/fonts/Arsenal-Regular.ttf", 16, nullptr, io.Fonts->GetGlyphRangesCyrillic());

	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
	ImGui_ImplSDLRenderer3_Init(mRenderer);

	gEngine = this;

	createSubsystems();

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

		for (auto& object : mObjects)
		{
			if (!object->isInitialized())
				object->init();
			if (!object->isInitialized())
				throw std::runtime_error("Object unable to init! Did you forget to call parent init()?");
		};

		// remove all objects that were marked for removal
		const auto copyObjectToRemove = mObjectsToRemove;
		for (auto& obj : copyObjectToRemove)
		{
			const auto pred = [obj](const std::unique_ptr<object>& other)
			{
				return other.get() == obj;
			};

			std::erase_if(mObjects, pred);
		}
		mObjectsToRemove.clear();

		pollEvents();

		// copy objects to avoid iterator invalidation
		std::vector<object*> copyObjects{};
		copyObjects.reserve(mObjects.size());
		for (const auto& object : mObjects)
		{
			copyObjects.push_back(object.get());
		}

		for (auto& object : copyObjects)
		{
			object->update(deltaSeconds);
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
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

		for (auto& object : copyObjects)
		{
			object->draw(mRenderer);
		}

		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);

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

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	gEngine = nullptr;
}

SDL_Texture* gl::app::engine::loadTexture(const std::string& texturePath)
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
	const auto pred = [obj](const std::unique_ptr<object>& other)
	{
		return other.get() == obj;
	};

	auto iter = std::find_if(mObjects.begin(), mObjects.end(), pred);
	const bool bFind = iter != mObjects.end();
	if (bFind)
	{
		mObjectsToRemove.push_back(obj);
	}
	return bFind;
}

const gl::app::assets::asset_header* gl::app::engine::loadAsset(const std::string& assetJsonPath)
{
	const auto findIter = mAssets.find(assetJsonPath);
	if (findIter != mAssets.end())
	{
		return findIter->second.get();
	}

	auto newAsset = assets::loadAssetJson(assetJsonPath);
	if (newAsset && newAsset->type != assets::eAssetType::Unknown)
	{
		const auto& newPair = mAssets.emplace(assetJsonPath, std::move(newAsset));
		return newPair.first->second.get();
	}
	return nullptr;
}

void gl::app::engine::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_EVENT_KEY_UP:
			if (event.key.key == SDLK_0 && !bShowObjectInspector)
			{
				bShowObjectInspector = true;
			}
			else if (event.key.key == SDLK_9 && !bShowYoutubeManagerInspector)
			{
				bShowYoutubeManagerInspector = true;
			}
			break;
		case SDL_EVENT_QUIT:
			mRunning = false;
			break;
		}
	}
}

void gl::app::engine::createSubsystems()
{
	// todo: make sure subsystems are unique
	createObject<timer_manager>();
	createObject<youtube_manager>();
	createObject<twitch_manager>();
	createObject<chat_ghost_subsystem>();
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

	if (ImGui::Button("Spawn bread"))
	{
		createObject<pony>("GloryOfNight", std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	}

	if (ImGui::Button("Spawn ghost"))
	{
		auto newGhost = createObject<chat_ghost>(newGhostName, std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
		newGhost->setSpeed(newGhostSpeed);
	}

	ImGui::Separator();

	auto& ghostBox = chat_ghost::getGhostBox();

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

	for (auto& object : mObjects)
	{
		auto ghost = dynamic_cast<chat_ghost*>(object.get());
		if (ghost)
		{
			const auto channelId = ghost->getUserId();

			ImGui::Text("Ghost - %s (%s)", ghost->getUserName().c_str(), ghost->getUserId().c_str());

			double x, y;
			ghost->getPos(&x, &y);

			float pos[2] = {static_cast<float>(x), static_cast<float>(y)};
			ImGui::InputFloat2((std::string("Position##") + channelId).c_str(), pos, "%.1f");

			float speed = ghost->getSpeed();
			ImGui::InputFloat((std::string("Speed##") + channelId).c_str(), &speed, 0, 0, "%.1f");

			ghost->setPos(pos[0], pos[1]);
			ghost->setSpeed(speed);

			bool bHide = ghost->getVisible();
			ImGui::Checkbox((std::string("Hide##") + channelId).c_str(), &bHide);
			ghost->setVisible(bHide);

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

			ImGui::SameLine();

			if (ImGui::Button((std::string("Set Twitch##") + channelId).c_str()))
			{
				ghost->showTwitchLogo();
			}

			ImGui::SameLine();

			if (ImGui::Button((std::string("Set Youtube##") + channelId).c_str()))
			{
				ghost->showYoutubeLogo();
			}

			ImGui::Separator();
		}

		auto ponyChar = dynamic_cast<pony*>(object.get());
		if (ponyChar)
		{
			const auto& userId = ponyChar->getUserId();

			double x, y;
			ponyChar->getPos(&x, &y);

			float pos[2] = {static_cast<float>(x), static_cast<float>(y)};
			ImGui::InputFloat2((std::string("Position##") + userId).c_str(), pos, "%.1f");

			ponyChar->setPos(pos[0], pos[1]);

			bool bHide = ponyChar->getVisible();
			ImGui::Checkbox((std::string("Hide##") + userId).c_str(), &bHide);
			ponyChar->setVisible(bHide);

			float scale = ponyChar->getScale();
			ImGui::SliderFloat((std::string("Scale##") + userId).c_str(), &scale, 0.01f, 10.0f);

			ponyChar->setScale(scale);

			ImGui::SameLine();

			if (ImGui::Button((std::string("Remove##") + userId).c_str()))
			{
				removeObject(ponyChar);
			}

			ImGui::Separator();
		}
	};

	ImGui::End();
}

void gl::app::engine::showYoutubeManagerExpector()
{
	const auto pred = [](const std::unique_ptr<object>& obj)
	{
		return dynamic_cast<const youtube_manager*>(obj.get()) != nullptr;
	};

	auto iter = std::find_if(mObjects.begin(), mObjects.end(), pred);

	if (iter == mObjects.end())
		return;

	auto ytManager = dynamic_cast<youtube_manager*>(iter->get());

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
