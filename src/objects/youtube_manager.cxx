#include "objects/youtube_manager.hxx"

#include "core/engine.hxx"

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

	//auto [bAuth, authInfo] = yt::api::initalAuth(clientId, clientSecret);

	//if (!bAuth)
	//{
	//	std::cerr << "Failed to authenticate" << std::endl;
	//}

	//const auto listLiveBroadcastsRequest = yt::api::live::listLiveBroadcastsRequest(clientSecret)
	//										   .setParts({"snippet", "status"})
	//										   .setBroadcastStatus("completed")
	//										   .setBroadcastType("event")
	//										   .setMaxResults(1);

	//const std::string liveBroadcastsResponse = yt::api::fetch(listLiveBroadcastsRequest.url, authInfo.accessToken);

	//const auto liveBroadcastsResponseJson = nlohmann::json::parse(liveBroadcastsResponse);
	//for (const auto& item : liveBroadcastsResponseJson["items"])
	//{
	//	const std::string liveChatId = item["snippet"]["liveChatId"];

	//	const auto listLiveMessagesRequest = yt::api::live::listLiveChatMessagesRequest(clientSecret)
	//											 .setParts({"snippet", "authorDetails"})
	//											 .setLiveChatId(liveChatId);

	//	const std::string liveMessagesResponse = yt::api::fetch(listLiveMessagesRequest.url, authInfo.accessToken);
	//}

	//const auto listSubscribersRequest = yt::api::live::listSubscribtionsRequest(clientSecret)
	//										.setParts({"subscriberSnippet"})
	//										.setMyRecentSubscribers(true)
	//										.setMaxResults(50);

	//const std::string listSubscribersResponse = yt::api::fetch(listSubscribersRequest.url, authInfo.accessToken);

	//const auto recentJson = nlohmann::json::parse(listSubscribersResponse);

	//for (const auto& item : recentJson["items"])
	//{
	//	const std::string title = item["subscriberSnippet"]["title"];
	//	const std::string id = item["subscriberSnippet"]["channelId"];

	//	//auto subGhost = eng.createObject<gl::app::subsubscriber_ghost>(title, id);
	//}
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

			mRefreshAuthTimer = timerManager->addTimer(auth.expiresIn + 1, std::bind(&youtube_manager::refreshAuth, this), false);

			if (mRefreshRecentSubs == timer_handle())
			{
				mRefreshRecentSubs = timerManager->addTimer(5.0, std::bind(&youtube_manager::requestSubs, this), true);
				requestSubs();
			}

			if (mRefreshBroadcasts == timer_handle())
			{
				mRefreshBroadcasts = timerManager->addTimer(60.0, std::bind(&youtube_manager::requestBroadcasts, this), true);
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

	mRecentSubsFuture = std::async(yt::api::fetch, listSubscribersRequest.url, auth.accessToken);
}

void gl::app::youtube_manager::processSubs()
{
	if (mRecentSubsFuture.valid() && mRecentSubsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string recentSubsResponse = mRecentSubsFuture.get();

		const auto recentSubsResponseJson = nlohmann::json::parse(recentSubsResponse);

		if (recentSubsResponseJson.contains("error"))
		{
			std::cerr << "Failed to fetch subs" << std::endl;
		}
		else
		{
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

					// do stuff
				}
			}
		}

		mRecentSubsFuture = {};
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

	mRefreshBroadcastsFuture = std::async(yt::api::fetch, listLiveBroadcastsRequest.url, auth.accessToken);
}

void gl::app::youtube_manager::processBroadcasts()
{
	if (mRefreshBroadcastsFuture.valid() && mRefreshBroadcastsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string liveBroadcastsResponse = mRefreshBroadcastsFuture.get();

		const auto liveBroadcastsResponseJson = nlohmann::json::parse(liveBroadcastsResponse);

		if (liveBroadcastsResponseJson.contains("error"))
		{
			std::cerr << "Failed to fetch broadcasts" << std::endl;
		}
		else
		{
			for (const auto& item : liveBroadcastsResponseJson["items"])
			{
				const std::string id = item["id"];

				const auto iter = std::find_if(mBroadcasts.begin(), mBroadcasts.end(), [&id](const broadcast& val)
					{ return val.id == id; });
				if (iter == mBroadcasts.end())
				{
					broadcast val{};
					val.id = id;
					val.liveChatId = item["snippet"]["liveChatId"];
					val.lifeCycleStatus = item["status"]["lifeCycleStatus"];

					mBroadcasts.emplace_back(std::move(val));

					std::cout << "Found new broadcast -" << val.lifeCycleStatus << std::endl;
				}
				else 
				{
					iter->lifeCycleStatus = item["status"]["lifeCycleStatus"];
				}
			}
		}

		mRefreshBroadcastsFuture = {};
	}
}

void gl::app::youtube_manager::requestLiveChatMessages()
{
	if (!bAuthSuccess)
		return;

	const auto liveBroadcast = std::find_if(mBroadcasts.begin(), mBroadcasts.end(), [](const broadcast& val)
		{ return val.lifeCycleStatus == "active" || val.lifeCycleStatus == "ready"; });

	if (liveBroadcast == mBroadcasts.end())
		return;

	const auto listLiveMessagesRequest = yt::api::live::listLiveChatMessagesRequest(clientSecret)
											 .setParts({"snippet", "authorDetails"})
											 .setLiveChatId(liveBroadcast->liveChatId);

	mRefreshLiveChatFuture = std::async(yt::api::fetch, listLiveMessagesRequest.url, auth.accessToken);
}

void gl::app::youtube_manager::processLiveChatMessages()
{
	if (mRefreshLiveChatFuture.valid() && mRefreshLiveChatFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		const std::string liveMessagesResponse = mRefreshLiveChatFuture.get();

		const auto liveMessagesResponseJson = nlohmann::json::parse(liveMessagesResponse);

		if (liveMessagesResponseJson.contains("error"))
		{
			std::cerr << "Failed to fetch live chat messages" << std::endl;
		}
		else
		{
			uint32_t pollingMs = liveMessagesResponseJson["pollingIntervalMillis"];

			auto timerManager = engine::get()->getTimerManager();
			timerManager->clearTimer(mRefreshLiveChat);
			mRefreshLiveChat = timerManager->addTimer(pollingMs / 1000.0, std::bind(&youtube_manager::requestLiveChatMessages, this), true);

			for (const auto& item : liveMessagesResponseJson["items"])
			{
				const std::string id = item["id"];

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

					mLiveChat.emplace_back(std::move(val));
				}
			}
		}

		mRefreshLiveChatFuture = {};
	}
}