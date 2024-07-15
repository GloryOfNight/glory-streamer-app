#pragma once

#include "core/subsystem.hxx"

#include <string>

namespace gl::app
{
	class chat_ghost_subsystem : public subsystem
	{
	public:
		static chat_ghost_subsystem* get();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

	private:
		void onLiveChatMessage(const std::string& channelId, const std::string& displayName, const std::string& displayMessage);
	};
} // namespace gl::app