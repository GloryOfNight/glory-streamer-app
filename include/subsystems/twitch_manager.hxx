#pragma once

#include "api/twitch_api.hxx"
#include "core/delegate.hxx"
#include "core/subsystem.hxx"
#include "subsystems/timer_manager.hxx"

#include <future>
#include <string>

namespace gl::app
{
	class twitch_manager : public subsystem
	{
	public:
		delegate<const std::string& /*user_id*/, const std::string& /*user_login*/, const std::string& /*user_name*/> onChatterReceived;

		twitch_manager() = default;
		~twitch_manager();

		static twitch_manager* get();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

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
	};
} // namespace gl::app