#include "subsystems/youtube_manager.hxx"

#include "actors/chat_ghost.hxx"
#include "core/engine.hxx"
#include "core/log.hxx"

#include <format>
#include <future>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#if __has_include("secrets/youtube-secret.h")
#include "secrets/youtube-secret.h"
#else
#include "secrets/youtube-secret-template.h"
#endif

static gl::app::youtube_manager* gYoutubeManager = nullptr;

gl::app::youtube_manager::~youtube_manager()
{
	if (gYoutubeManager == this)
	{
		gYoutubeManager = nullptr;
	}
}

gl::app::youtube_manager* gl::app::youtube_manager::get()
{
	return gYoutubeManager;
}

void gl::app::youtube_manager::init()
{
	subsystem::init();

	timer_manager::get()->addTimer(0.0, std::bind(&youtube_manager::requestAuth, this), false);

	gYoutubeManager = this;
}

void gl::app::youtube_manager::update(double delta)
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
			LOG(Display, "User authenticated");

			auth = authInfo;
			bAuthSuccess = true;

			auto timerManager = timer_manager::get();

			mRefreshAuthTimer = timerManager->addTimer(auth.expiresIn - 15, std::bind(&youtube_manager::requestRefreshAuth, this), false);

			if (mRefreshRecentSubs == timer_handle())
			{
				mRefreshRecentSubs = timerManager->addTimer(180.0, std::bind(&youtube_manager::requestSubs, this), true);
				requestSubs();
			}

			if (mRefreshBroadcasts == timer_handle())
			{
				mRefreshBroadcasts = timerManager->addTimer(300.0, std::bind(&youtube_manager::requestBroadcasts, this), true);
				requestBroadcasts();
			}

			if (mRefreshLiveChat == timer_handle())
			{
				mRefreshLiveChat = timerManager->addTimer(7.5, std::bind(&youtube_manager::requestLiveChatMessages, this), true);
			}
		}
	}

	processSubs();
	processBroadcasts();
	processLiveChatMessages();
}

void gl::app::youtube_manager::draw(SDL_Renderer* renderer)
{
}

void gl::app::youtube_manager::requestAuth()
{
	LOG(Display, "Requesting fresh auth");
	mAuthFuture = std::async(yt::api::initalAuth, yt::secret::clientId, yt::secret::clientSecret);
}

void gl::app::youtube_manager::requestRefreshAuth()
{
	if (!bAuthSuccess)
	{
		LOG(Error, "Failed to refresh auth, not authenticated.");
		return;
	}

	LOG(Display, "Requesting refresh of auth");
	mAuthFuture = std::async(yt::api::refreshAuth, yt::secret::clientId, yt::secret::clientSecret, auth.refreshToken);
}

void gl::app::youtube_manager::requestSubs()
{
	if (!bAuthSuccess)
		return;

	const auto listSubscribersRequest = yt::api::live::listSubscribtionsRequest(yt::secret::clientSecret)
											.setParts({"snippet", "subscriberSnippet"})
											.setFields("etag,nextPageToken,items(id,subscriberSnippet(title,channelId),snippet(publishedAt))")
											.setMyRecentSubscribers(true)
											.setMaxResults(3);

	mRecentSubsFuture = std::async(yt::api::fetch, listSubscribersRequest.url, auth.accessToken, mSubsribersETag);
}

void gl::app::youtube_manager::processSubs()
{
	if (mRecentSubsFuture.valid() && mRecentSubsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string subscribersResponse = mRecentSubsFuture.get();
		mRecentSubsFuture = {};

		if (subscribersResponse.empty())
			return;

		const auto subsribersResponseJson = nlohmann::json::parse(subscribersResponse);
		if (subsribersResponseJson.contains("error"))
		{
			const int32_t code = subsribersResponseJson["error"]["code"];
			const std::string message = subsribersResponseJson["error"]["message"];

			std::cerr << std::format("Failed to fetch subscribers: Code: {0}. Message: {1}.", code, message) << std::endl;
		}
		else
		{
			mSubsribersETag = subsribersResponseJson["etag"];
			mSubsribersNextPageToken = subsribersResponseJson["nextPageToken"];

			for (const auto& item : subsribersResponseJson["items"])
			{
				const std::string id = item["id"];

				const auto iter = std::find_if(mRecentSubs.begin(), mRecentSubs.end(), [&id](const subscriber& val)
					{ return val.id == id; });
				if (iter == mRecentSubs.end())
				{
					subscriber val{};
					val.id = id;
					val.title = item["subscriberSnippet"]["title"];
					val.channelId = item["subscriberSnippet"]["channelId"];
					val.publishedAt = item["snippet"]["publishedAt"];

					const auto& newSubscriber = mRecentSubs.emplace_back(std::move(val));

					LOG(Display, "Subscriber - \"{0}\" ({1}) at \'{2}\'", newSubscriber.title, newSubscriber.channelId, newSubscriber.publishedAt);
				}
			}
		}
	}
}

void gl::app::youtube_manager::requestBroadcasts()
{
	if (!bAuthSuccess)
		return;

	const auto listLiveBroadcastsRequest = yt::api::live::listLiveBroadcastsRequest(yt::secret::clientSecret)
											   .setParts({"snippet", "status"})
											   .setFields("etag,nextPageToken,items(id,etag,snippet(title,liveChatId),status(lifeCycleStatus))")
											   .setBroadcastStatus("all")
											   .setBroadcastType("event")
											   .setMaxResults(3);

	mRefreshBroadcastsFuture = std::async(yt::api::fetch, listLiveBroadcastsRequest.url, auth.accessToken, mBroadcastsETag);
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
			mBroadcastsETag = liveBroadcastsResponseJson["etag"];
			mBroadcastsNextPageToken = liveBroadcastsResponseJson["nextPageToken"];

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

					LOG(Display, "Found new broadcast - \"{0}\" in status \'{1}\'", newBroadcast.id, newBroadcast.lifeCycleStatus);
				}
				else if (iter->lifeCycleStatus != liveCycleStatus)
				{
					iter->lifeCycleStatus = liveCycleStatus;
					LOG(Display, "Updated broadcast - \"{0}\" new status \'{1}\'", iter->id, iter->lifeCycleStatus);
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
		{ return val.lifeCycleStatus == "live" || val.lifeCycleStatus == "testing" || val.lifeCycleStatus == "ready"; });

	if (liveBroadcast == mBroadcasts.end())
		return;

	const auto listLiveMessagesRequest = yt::api::live::listLiveChatMessagesRequest(yt::secret::clientSecret)
											 .setParts({"snippet", "authorDetails"})
											 .setFields("etag,nextPageToken,pollingIntervalMillis,items(id,etag,snippet(type,publishedAt,displayMessage),authorDetails(channelId,displayName))")
											 .setNextPageToken(mLiveChatNextPageToken)
											 .setLiveChatId(liveBroadcast->liveChatId)
											 .setMaxResults(20);

	mRefreshLiveChatFuture = std::async(yt::api::fetch, listLiveMessagesRequest.url, auth.accessToken, mLiveChatETag);
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
			mLiveChatNextPageToken = liveMessagesResponseJson["nextPageToken"];
			uint32_t pollingMs = liveMessagesResponseJson["pollingIntervalMillis"];

			mLiveChatETag = liveMessagesResponseJson["etag"];

			// saving quota
			//auto timerManager = engine::get()->getTimerManager();
			//timerManager->clearTimer(mRefreshLiveChat);
			//mRefreshLiveChat = timerManager->addTimer((pollingMs + 100) / 1000.0, std::bind(&youtube_manager::requestLiveChatMessages, this), true);

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

					const auto& newLiveMessage = mLiveChat.emplace_back(std::move(val));

					LOG(Display, "New message: {0} - {1}", newLiveMessage.displayName, newLiveMessage.displayMessage);

					onLiveChatMessage.execute(newLiveMessage.channelId, newLiveMessage.displayName, newLiveMessage.displayMessage);
				}
			}
		}
	}
}
