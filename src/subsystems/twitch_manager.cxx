#include "subsystems/twitch_manager.hxx"

#include "api/twitch_api.hxx"
#include "core/log.hxx"
#include "secrets/twitch-secret.h"

gl::app::twitch_manager* gTwitchManager{nullptr};

gl::app::twitch_manager::~twitch_manager()
{
	if (gTwitchManager == this)
	{
		gTwitchManager = nullptr;
	}
}

gl::app::twitch_manager* gl::app::twitch_manager::get()
{
	return gTwitchManager;
}

void gl::app::twitch_manager::init()
{
	subsystem::init();

	gTwitchManager = this;

	requestAuth();
}

void gl::app::twitch_manager::update(double delta)
{
	if (mAuthFuture.valid() && mAuthFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		auto [bAuthed, authInfo] = mAuthFuture.get();
		mAuthFuture = {};

		if (!bAuthed)
		{
			LOG(Error, "Failed to authenticate. Reattempting.");
			requestAuth();
		}
		else
		{
			mAuth = authInfo;
			bAuthSuccess = true;

			LOG(Display, "Authenticated with Twitch.");
		}
	}
}

void gl::app::twitch_manager::draw(SDL_Renderer* renderer)
{
}

void gl::app::twitch_manager::requestAuth()
{
	mAuthFuture = std::async(ttv::api::initialAuth, ttv::secret::clientId, ttv::secret::clientSecret);
}
