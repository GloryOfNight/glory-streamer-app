#pragma once

#include "api/twitch_api.hxx"
#include "core/delegate.hxx"
#include "core/subsystem.hxx"
#include "subsystems/timer_manager.hxx"

#include <future>
#include <memory>
#include <string>

namespace gl::app
{
	class twitch_manager : public subsystem
	{
	public:
		delegate<const std::string& /*userId*/, const std::string& /*userLogin*/, const std::string& /*userName*/> onChatterReceived;

		delegate<const std::string& /*userId*/, const std::string& /*userLogin*/, const std::string& /*userName*/, const std::string& /*message*/, const std::string& /*messageId*/> onMessageReceived;

		twitch_manager() = default;
		~twitch_manager();

		static twitch_manager* get();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		void sendChatMessage(const std::string& message, const std::string replyMessageId);

	private:
		void requestAuth();
		void requestRefreshAuth();

		void requestUser();

		void requestChatters();

		std::future<std::pair<bool, ttv::api::auth_info>> mAuthFuture;
		ttv::api::auth_info mAuth{};
		timer_handle mRefreshAuth;
		bool bAuthSuccess{false};

		std::future<std::string> mUserFuture;
		std::string mUserId{};
		std::string mUserLogin{};
		std::string mUserDisplayName{};

		std::future<std::string> mChattersFuture;
		timer_handle mRefreshChatters;

		std::unique_ptr<ttv::api::eventSubClient> mEventSubClient;
	};
} // namespace gl::app