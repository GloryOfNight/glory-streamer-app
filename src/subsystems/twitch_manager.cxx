#include "subsystems/twitch_manager.hxx"

#include "api/twitch_api.hxx"
#include "core/log.hxx"
#include "secrets/twitch-secret.h"

#include <nlohmann/json.hpp>

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

	timer_manager::get()->addTimer(0.1, std::bind(&twitch_manager::requestAuth, this), false);
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

			if (mUserId.empty())
				requestUser();

			if (mRefreshAuth)
				timer_manager::get()->clearTimer(mRefreshAuth);

			timer_manager::get()->addTimer(authInfo.expiresIn - 15, std::bind(&twitch_manager::requestRefreshAuth, this), false);
		}
	}

	if (mUserFuture.valid() && mUserFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string userResponse = mUserFuture.get();
		mUserFuture = {};

		const auto userResponseJson = nlohmann::json::parse(userResponse);

		if (userResponseJson.contains("error"))
		{
			LOG(Error, "Failed to fetch user.");
		}
		else
		{
			mUserId = userResponseJson["data"][0]["id"];
			mUserLogin = userResponseJson["data"][0]["login"];
			mUserDisplayName = userResponseJson["data"][0]["display_name"];

			LOG(Display, "Twitch logged in as {}", mUserLogin);

			if (mRefreshChatters == timer_handle())
			{
				mRefreshChatters = timer_manager::get()->addTimer(100.0, std::bind(&twitch_manager::requestChatters, this), true);
				requestChatters();
			}

			mEventSubClient = std::make_unique<ttv::api::eventSubClient>();
			mEventSubClient->connect();
		}
	}

	if (mChattersFuture.valid() && mChattersFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string chatters = mChattersFuture.get();
		mChattersFuture = {};

		const auto chattersJson = nlohmann::json::parse(chatters);
		if (chattersJson.contains("error"))
		{
			LOG(Error, "Failed to fetch chatters.");
		}
		else
		{
			for (const auto& chatter : chattersJson["data"])
			{
				onChatterReceived.execute(chatter["user_id"], chatter["user_login"], chatter["user_name"]);
			}
		}
	}

	if (mEventSubClient)
	{
		const std::string eventSubMessage = mEventSubClient->recv();
		if (!eventSubMessage.empty())
		{
			try
			{
				nlohmann::json eventSubJson = nlohmann::json::parse(eventSubMessage);
				if (eventSubJson.contains("metadata"))
				{
					const std::string messageType = eventSubJson["metadata"]["message_type"];
					if (messageType == "session_welcome")
					{
						LOG(Display, "EventSub connected.");

						const std::string sessionId = eventSubJson["payload"]["session"]["id"];

						nlohmann::json postJson;
						postJson["type"] = "channel.chat.message";
						postJson["version"] = "1";
						postJson["condition"]["broadcaster_user_id"] = mUserId;
						postJson["condition"]["user_id"] = mUserId;
						postJson["transport"]["method"] = "websocket";
						postJson["transport"]["session_id"] = sessionId;

						const std::string postStr = postJson.dump();

						const std::string postRes = ttv::api::post("https://api.twitch.tv/helix/eventsub/subscriptions", mAuth.accessToken, ttv::secret::clientId, postStr);
						const auto postResJson = nlohmann::json::parse(postRes);
						if (postResJson.contains("error"))
						{
							LOG(Error, "Failed to subscribe to EventSub.");
						}
					}
					else if (messageType == "notification")
					{
						if (eventSubJson["metadata"]["subscription_type"] == "channel.chat.message")
						{
							const std::string EventType = eventSubJson["payload"]["event"]["message_type"];
							const std::string userId = eventSubJson["payload"]["event"]["chatter_user_id"];
							const std::string userName = eventSubJson["payload"]["event"]["chatter_user_login"];
							const std::string userDisplayName = eventSubJson["payload"]["event"]["chatter_user_name"];
							const std::string message = eventSubJson["payload"]["event"]["message"]["text"];

							LOG(Display, "Twitch chat message from {}: {}", userDisplayName, message);

							onMessageReceived.execute(userId, userName, userDisplayName, message);
						}
					}
					else if (messageType == "session_keepalive") // do nothing
					{
					}
					else
					{
						LOG(Display, "Twitch received unsupported message type: {}", messageType);
					}
				}
			}
			catch (...)
			{
				LOG(Error, "Failed to parse EventSub message.");
			}
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

void gl::app::twitch_manager::requestRefreshAuth()
{
	if (!bAuthSuccess)
	{
		LOG(Error, "Cannot refresh auth without authentication.");
		return;
	}

	LOG(Display, "Refreshing Twitch authentication.");
	mAuthFuture = std::async(ttv::api::refreshAuth, ttv::secret::clientId, ttv::secret::clientSecret, mAuth.refreshToken);
}

void gl::app::twitch_manager::requestUser()
{
	if (!bAuthSuccess)
	{
		LOG(Error, "Cannot request user without authentication.");
		return;
	}

	const ttv::api::ListUsers request = ttv::api::ListUsers();

	mUserFuture = std::async(ttv::api::fetch, request.url, mAuth.accessToken, ttv::secret::clientId);
}

void gl::app::twitch_manager::requestChatters()
{
	if (!bAuthSuccess)
	{
		LOG(Error, "Cannot request chatters without authentication.");
		return;
	}

	const ttv::api::ListChattersRequest request = ttv::api::ListChattersRequest(mUserId, mUserId);

	mChattersFuture = std::async(ttv::api::fetch, request.url, mAuth.accessToken, ttv::secret::clientId);
}
