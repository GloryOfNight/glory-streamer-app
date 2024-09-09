#pragma once

#include "api/twitch_api.hxx"
#include "core/subsystem.hxx"

#include <future>

namespace gl::app
{
	class twitch_manager : public subsystem
	{
	public:
		twitch_manager() = default;
		~twitch_manager();

		static twitch_manager* get();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

	private:
		void requestAuth();

		std::future<std::pair<bool, ttv::api::auth_info>> mAuthFuture;

		ttv::api::auth_info mAuth{};
		bool bAuthSuccess{false};
	};
} // namespace gl::app