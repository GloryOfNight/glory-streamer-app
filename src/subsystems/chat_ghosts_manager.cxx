#include "subsystems/chat_ghosts_manager.hxx"

#include "actors/chat_ghost.hxx"
#include "core/engine.hxx"
#include "subsystems/youtube_manager.hxx"

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
}

void gl::app::chat_ghost_subsystem::update(double delta)
{
}

void gl::app::chat_ghost_subsystem::draw(SDL_Renderer* renderer)
{
}

void gl::app::chat_ghost_subsystem::onLiveChatMessage(const std::string& channelId, const std::string& displayName, const std::string& displayMessage)
{
	// temp: move somewhere else
	const auto& objects = engine::get()->getObjects();
	const auto iter = std::find_if(objects.begin(), objects.end(), [&channelId](const std::unique_ptr<object>& obj)
		{ 
							const auto ghost = dynamic_cast<const subscriber_ghost*>(obj.get());
							return ghost && ghost->getChannelId() == channelId; });

	if (iter == objects.end())
	{
		auto ghost = engine::get()->createObject<subscriber_ghost>(displayName, channelId);
		ghost->setMessage(displayMessage);
	}
	else
	{
		auto ghost = dynamic_cast<subscriber_ghost*>(iter->get());
		ghost->setSpeed(ghost->getSpeed() + 1);
		ghost->setMessage(displayMessage);
	}
}
