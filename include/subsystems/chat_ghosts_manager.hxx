#pragma once

#include "core/subsystem.hxx"

#include <string>
#include <vector>

namespace gl::app
{
	class chat_ghost_subsystem : public subsystem
	{
	public:
		chat_ghost_subsystem() = default;
		~chat_ghost_subsystem();

		static chat_ghost_subsystem* get();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

	private:
		void onLiveChatMessage(const std::string& channelId, const std::string& displayName, const std::string& displayMessage);
		void onTwitchChatterReceived(const std::string& userId, const std::string& userLogin, const std::string& userName);
		void onTwitchMessageReceived(const std::string& userId, const std::string& userLogin, const std::string& userName, const std::string& message, const std::string messageId);

		void parseCommandMessage(const std::string& fullMessage, std::string& command, std::string& realMessage);

		class chat_ghost* findGhostByUserId(const std::string& userId);
	};
} // namespace gl::app