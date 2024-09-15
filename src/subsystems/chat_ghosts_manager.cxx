#include "subsystems/chat_ghosts_manager.hxx"

#include "actors/chat_ghost.hxx"
#include "core/engine.hxx"
#include "subsystems/twitch_manager.hxx"
#include "subsystems/youtube_manager.hxx"

#include <format>

static std::vector<std::string> mSpawnGhostsCommands = {"!show", "!spawnghost", "!ghost", "!hi", "!sayhi"};
static std::vector<std::string> mDespawnGhostsCommands = {"!hide"};
static std::vector<std::string> mSayHiCommands = {"!hi", "!sayhi"};

static gl::app::chat_ghost_subsystem* gChatGhostSubsystem = nullptr;

gl::app::chat_ghost_subsystem::~chat_ghost_subsystem()
{
	if (gChatGhostSubsystem == this)
	{
		gChatGhostSubsystem = nullptr;
	}
}

gl::app::chat_ghost_subsystem* gl::app::chat_ghost_subsystem::get()
{
	return nullptr;
}

void gl::app::chat_ghost_subsystem::init()
{
	subsystem::init();
	gChatGhostSubsystem = this;

	youtube_manager::get()->onLiveChatMessage.bind(std::bind(&chat_ghost_subsystem::onLiveChatMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	twitch_manager::get()->onChatterReceived.bind(std::bind(&chat_ghost_subsystem::onTwitchChatterReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	twitch_manager::get()->onMessageReceived.bind(std::bind(&chat_ghost_subsystem::onTwitchMessageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void gl::app::chat_ghost_subsystem::update(double delta)
{
}

void gl::app::chat_ghost_subsystem::draw(SDL_Renderer* renderer)
{
}

void gl::app::chat_ghost_subsystem::onLiveChatMessage(const std::string& channelId, const std::string& displayName, const std::string& displayMessage)
{
	const auto& objects = engine::get()->getObjects();
	auto ghost = findGhostByUserId(channelId);

	const bool bSpawn = std::find(mSpawnGhostsCommands.begin(), mSpawnGhostsCommands.end(), displayMessage) != mSpawnGhostsCommands.end();
	const bool bDespawn = std::find(mDespawnGhostsCommands.begin(), mDespawnGhostsCommands.end(), displayMessage) != mDespawnGhostsCommands.end();
	const bool bSayHi = std::find(mSayHiCommands.begin(), mSayHiCommands.end(), displayMessage) != mSayHiCommands.end();

	if (bSpawn && !ghost)
	{
		auto newGhost = engine::get()->createObject<chat_ghost>(displayName, channelId);
		newGhost->showYoutubeLogo();

		ghost = newGhost;
	}

	if (bSayHi && ghost)
	{
		ghost->sayHi();
		ghost->resetDeathTimer();

		youtube_manager::get()->sendLiveChatMessage(std::format("Приветсвуем, @{}!", displayName));
	}
	else if (!bSpawn && ghost)
	{
		ghost->setMessage(displayMessage);
		ghost->resetDeathTimer();
	}

	if (bDespawn && ghost)
	{
		ghost->destroy();
	}
}

void gl::app::chat_ghost_subsystem::onTwitchChatterReceived(const std::string& userId, const std::string& userLogin, const std::string& userName)
{
	const auto& objects = engine::get()->getObjects();
	const auto iter = std::find_if(objects.begin(), objects.end(), [&userId](const std::unique_ptr<object>& obj)
		{ 
							const auto ghost = dynamic_cast<const chat_ghost*>(obj.get());
							return ghost && ghost->getUserId() == userId; });

	if (iter != objects.end())
	{
		auto ghost = dynamic_cast<chat_ghost*>(iter->get());
		ghost->resetDeathTimer();
	}
}

void gl::app::chat_ghost_subsystem::onTwitchMessageReceived(const std::string& userId, const std::string& userLogin, const std::string& userName, const std::string& message, const std::string messageId)
{
	const auto& objects = engine::get()->getObjects();
	auto ghost = findGhostByUserId(userId);

	const bool bSpawn = std::find(mSpawnGhostsCommands.begin(), mSpawnGhostsCommands.end(), message) != mSpawnGhostsCommands.end();
	const bool bDespawn = std::find(mDespawnGhostsCommands.begin(), mDespawnGhostsCommands.end(), message) != mDespawnGhostsCommands.end();
	const bool bSayHi = std::find(mSayHiCommands.begin(), mSayHiCommands.end(), message) != mSayHiCommands.end();

	if (bSpawn && !ghost)
	{
		auto newGhost = engine::get()->createObject<chat_ghost>(userName, userId);
		newGhost->showTwitchLogo();

		ghost = newGhost;
	}

	if (bSayHi && ghost)
	{
		ghost->sayHi();
		ghost->resetDeathTimer();

		const std::vector<std::string> greetings = {"Привествуем!", "Добро пожаловать!", "Здравствуй!", "Какая встреча!", "Привет!"};

		twitch_manager::get()->sendChatMessage(greetings[std::rand() % greetings.size()], messageId);
	}
	else if (!bSpawn && ghost)
	{
		ghost->setMessage(message);
		ghost->resetDeathTimer();
	}

	if (bDespawn && ghost)
	{
		ghost->destroy();
	}
}

gl::app::chat_ghost* gl::app::chat_ghost_subsystem::findGhostByUserId(const std::string& userId)
{
	const auto& objects = engine::get()->getObjects();
	const auto iter = std::find_if(objects.begin(), objects.end(), [&userId](const std::unique_ptr<object>& obj)
		{ 
							const auto ghost = dynamic_cast<const chat_ghost*>(obj.get());
							return ghost && ghost->getUserId() == userId; });

	return iter != objects.end() ? dynamic_cast<chat_ghost*>(iter->get()) : nullptr;
}
