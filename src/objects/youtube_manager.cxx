#include "objects/youtube_manager.hxx"

#include "core/engine.hxx"
#include "objects/subscriber_ghost.hxx"

#include <format>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

static yt::api::auth_info auth{};

static const std::string clientId = "1002066649738-3gfrfvhfdt9q2j3n7vq1ufkdlav603a9.apps.googleusercontent.com";
static const std::string clientSecret = "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";

void gl::app::youtube_manager::init()
{
	mAuthFuture = std::async(yt::api::initalAuth, clientId, clientSecret);
}

void gl::app::youtube_manager::update(double delta)
{
	if (mAuthFuture.valid() && mAuthFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		auto [bAuth, authInfo] = mAuthFuture.get();

		if (!bAuth)
		{
			std::cerr << "Failed to authenticate" << std::endl;
		}
		else
		{
			auth = authInfo;
			bAuthSuccess = true;

			auto timerManager = engine::get()->getTimerManager();

			mRefreshAuthTimer = timerManager->addTimer(auth.expiresIn, std::bind(&youtube_manager::refreshAuth, this), false);

			if (mRefreshRecentSubs == timer_handle())
			{
				// do not request for subs to save quota
				mRefreshRecentSubs = timerManager->addTimer(5.0, std::bind(&youtube_manager::requestSubs, this), true);
				requestSubs();
			}

			if (mRefreshBroadcasts == timer_handle())
			{
				mRefreshBroadcasts = timerManager->addTimer(180.0, std::bind(&youtube_manager::requestBroadcasts, this), true);
				requestBroadcasts();
			}

			if (mRefreshLiveChat == timer_handle())
			{
				mRefreshLiveChat = timerManager->addTimer(5.0, std::bind(&youtube_manager::requestLiveChatMessages, this), true);
			}
		}

		mAuthFuture = {};
	}

	processSubs();
	processBroadcasts();
	processLiveChatMessages();
}

void gl::app::youtube_manager::draw(SDL_Renderer* renderer)
{
}

void gl::app::youtube_manager::refreshAuth()
{
	bAuthSuccess = false;
	mAuthFuture = std::async(yt::api::refreshAuth, clientId, clientSecret, auth.refreshToken);
}

void gl::app::youtube_manager::requestSubs()
{
	if (!bAuthSuccess)
		return;

	const auto listSubscribersRequest = yt::api::live::listSubscribtionsRequest(clientSecret)
											.setParts({"snippet", "subscriberSnippet"})
											.setMyRecentSubscribers(true)
											.setMaxResults(5);

	mRecentSubsFuture = std::async(yt::api::fetch, listSubscribersRequest.url, auth.accessToken, mSubsEtag);
}

void gl::app::youtube_manager::processSubs()
{
	if (mRecentSubsFuture.valid() && mRecentSubsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string recentSubsResponse = mRecentSubsFuture.get();
		mRecentSubsFuture = {};

		if (recentSubsResponse.empty())
			return;

		const auto recentSubsResponseJson = nlohmann::json::parse(recentSubsResponse);
		if (recentSubsResponseJson.contains("error"))
		{
			std::cerr << "Failed to fetch subs" << std::endl;
		}
		else
		{
			mSubsEtag = recentSubsResponseJson["etag"];

			for (const auto& item : recentSubsResponseJson["items"])
			{
				const std::string id = item["id"];

				const auto iter = std::find_if(mRecentSubs.begin(), mRecentSubs.end(), [&id](const subscriber& val)
					{ return val.id == id; });
				if (iter == mRecentSubs.end())
				{
					subscriber val{};
					val.id = id;
					val.title = item["subscriberSnippet"]["title"];
					val.publishedAt = item["snippet"]["publishedAt"];

					mRecentSubs.emplace_back(std::move(val));
				}
			}
		}
	}
}

void gl::app::youtube_manager::requestBroadcasts()
{
	if (!bAuthSuccess)
		return;

	const auto listLiveBroadcastsRequest = yt::api::live::listLiveBroadcastsRequest(clientSecret)
											   .setParts({"snippet", "status"})
											   .setBroadcastStatus("all")
											   .setBroadcastType("event")
											   .setMaxResults(5);

	mRefreshBroadcastsFuture = std::async(yt::api::fetch, listLiveBroadcastsRequest.url, auth.accessToken, mBroadcastsEtag);
}

void gl::app::youtube_manager::processBroadcasts()
{
	if (mRefreshBroadcastsFuture.valid() && mRefreshBroadcastsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string liveBroadcastsResponse = mRefreshBroadcastsFuture.get();
		mRefreshBroadcastsFuture = {};

		if (liveBroadcastsResponse.empty())
			return;

		const auto liveBroadcastsResponseJson = nlohmann::json::parse(liveBroadcastsResponse);

		if (liveBroadcastsResponseJson.contains("error"))
		{
			const int32_t code = liveBroadcastsResponseJson["error"]["code"];
			const std::string message = liveBroadcastsResponseJson["error"]["message"];

			std::cerr << std::format("Failed to fetch broadcasts: Code: {0}. Message: {1}.", code, message) << std::endl;
		}
		else
		{
			mBroadcastsEtag = liveBroadcastsResponseJson["etag"];

			for (const auto& item : liveBroadcastsResponseJson["items"])
			{
				const std::string id = item["id"];
				const std::string liveCycleStatus = item["status"]["lifeCycleStatus"];

				const auto iter = std::find_if(mBroadcasts.begin(), mBroadcasts.end(), [&id](const broadcast& val)
					{ return val.id == id; });
				if (iter == mBroadcasts.end())
				{
					broadcast val{};
					val.id = id;
					val.liveChatId = item["snippet"]["liveChatId"];
					val.lifeCycleStatus = item["status"]["lifeCycleStatus"];

					const auto& newBroadcast = mBroadcasts.emplace_back(std::move(val));

					std::cout << std::format("Found new broadcast - \"{0}\" in status \'{1}\'", newBroadcast.id, newBroadcast.lifeCycleStatus) << std::endl;
				}
				else if (iter->lifeCycleStatus != liveCycleStatus)
				{
					iter->lifeCycleStatus = liveCycleStatus;
					std::cout << std::format("Updated broadcast - \"{0}\" new status \'{1}\'", iter->id, iter->lifeCycleStatus) << std::endl;
				}
			}
		}
	}
}

void gl::app::youtube_manager::requestLiveChatMessages()
{
	if (!bAuthSuccess)
		return;

	const auto liveBroadcast = std::find_if(mBroadcasts.begin(), mBroadcasts.end(), [](const broadcast& val)
		{ return val.lifeCycleStatus == "live" || val.lifeCycleStatus == "ready"; });

	if (liveBroadcast == mBroadcasts.end())
		return;

	const auto listLiveMessagesRequest = yt::api::live::listLiveChatMessagesRequest(clientSecret)
											 .setParts({"snippet", "authorDetails"})
											 .setLiveChatId(liveBroadcast->liveChatId)
											 .setMaxResults(200);

	mRefreshLiveChatFuture = std::async(yt::api::fetch, listLiveMessagesRequest.url, auth.accessToken, mLiveChatEtag);
}

void gl::app::youtube_manager::processLiveChatMessages()
{
	if (mRefreshLiveChatFuture.valid() && mRefreshLiveChatFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string liveMessagesResponse = mRefreshLiveChatFuture.get();
		mRefreshLiveChatFuture = {};

		if (liveMessagesResponse.empty())
			return;

		const auto liveMessagesResponseJson = nlohmann::json::parse(liveMessagesResponse);

		if (liveMessagesResponseJson.contains("error"))
		{
			const int32_t code = liveMessagesResponseJson["error"]["code"];
			const std::string message = liveMessagesResponseJson["error"]["message"];

			std::cerr << std::format("Failed to fetch live chat: Code: {0}. Message: {1}.", code, message) << std::endl;
		}
		else
		{
			uint32_t pollingMs = liveMessagesResponseJson["pollingIntervalMillis"];

			mLiveChatEtag = liveMessagesResponseJson["etag"];

			auto timerManager = engine::get()->getTimerManager();
			timerManager->clearTimer(mRefreshLiveChat);
			mRefreshLiveChat = timerManager->addTimer((pollingMs + 100) / 1000.0, std::bind(&youtube_manager::requestLiveChatMessages, this), true);

			for (const auto& item : liveMessagesResponseJson["items"])
			{
				const std::string id = item["id"];

				const std::string eventType = item["snippet"]["type"];
				if (eventType != "textMessageEvent")
					continue;

				const auto iter = std::find_if(mLiveChat.begin(), mLiveChat.end(), [&id](const live_chat_message& val)
					{ return val.id == id; });

				if (iter == mLiveChat.end())
				{
					live_chat_message val{};
					val.id = id;
					val.publishedAt = item["snippet"]["publishedAt"];
					val.displayName = item["authorDetails"]["displayName"];
					val.displayMessage = item["snippet"]["displayMessage"];
					val.channelId = item["authorDetails"]["channelId"];

					std::cout << val.displayName << ": " << val.displayMessage << std::endl;

					const auto& liveMessage = mLiveChat.emplace_back(std::move(val));

					// temp: move somewhere else
					const auto& objects = engine::get()->getObjects();
					const auto iter = std::find_if(objects.begin(), objects.end(), [&liveMessage](const object* obj)
						{ 
							const auto ghost = dynamic_cast<const subsubscriber_ghost*>(obj);
							return ghost && ghost->getChannelId() == liveMessage.channelId; });

					if (iter == objects.end())
					{
						engine::get()->createObject<subsubscriber_ghost>(liveMessage.displayName, liveMessage.channelId);
					}
				}
			}
		}
	}
}
