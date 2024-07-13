#pragma once

#include "api/youtube_api.hxx"
#include "core/object.hxx"
#include "core/timer_manager.hxx"

#include <future>

namespace gl::app
{
	class youtube_manager : public object
	{
	public:
		struct subscriber
		{
			std::string id{};
			std::string publishedAt{};
			std::string title{};
			std::string channelId{};
		};

		struct broadcast
		{
			std::string id{};
			std::string liveChatId{};
			std::string lifeCycleStatus{};
		};

		struct live_chat_message
		{
			std::string id{};
			std::string publishedAt{};
			std::string displayName{};
			std::string displayMessage{};
			std::string channelId{};
		};

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		

	private:
		void requestAuth();
		void requestRefreshAuth();

		void requestSubs();
		void processSubs();

		void requestBroadcasts();
		void processBroadcasts();

		void requestLiveChatMessages();
		void processLiveChatMessages();

		timer_handle mRefreshAuthTimer{};
		std::future<std::pair<bool, yt::api::auth_info>> mAuthFuture;

		timer_handle mRefreshRecentSubs{};
		std::future<std::string> mRecentSubsFuture{};
		std::vector<subscriber> mRecentSubs{};
		std::string mSubsEtag{};

		timer_handle mRefreshBroadcasts{};
		std::future<std::string> mRefreshBroadcastsFuture{};
		std::vector<broadcast> mBroadcasts{};
		std::string mBroadcastsEtag{};

		timer_handle mRefreshLiveChat{};
		std::future<std::string> mRefreshLiveChatFuture{};
		std::vector<live_chat_message> mLiveChat{};
		std::string mLiveChatEtag{};

		bool bAuthSuccess{};
	};
} // namespace gl::app