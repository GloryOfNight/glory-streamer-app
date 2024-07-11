#pragma once

#include <string>
#include <utility>

namespace yt::api
{
	struct auth_info
	{
		std::string accessToken{};
		int32_t expiresIn{};
		std::string refreshToken{};
		std::string scope{};
		std::string tokenType{};
	};

	std::pair<bool, auth_info> initalAuth(const std::string& clientId, const std::string& clientSecret);

	std::pair<bool, auth_info> refreshAuth(const std::string& clientId, const std::string& clientSecret, const std::string& refreshToken);

	// https://developers.google.com/youtube/v3/live/docs/liveBroadcasts/list
	std::string listLiveBroadcasts(const std::string& clientSecret, const std::string& accessToken, uint8_t maxResults);

	// https://developers.google.com/youtube/v3/live/docs/liveChatMessages/list
	std::string listLiveChat(const std::string& clientSecret, const std::string& accessToken, const std::string& liveChatId, uint16_t maxResults);

	// https://developers.google.com/youtube/v3/docs/subscriptions/list
	std::string listSubscribers(const std::string& clientSecret, const std::string& accessToken, uint8_t maxResults);

	// https://developers.google.com/youtube/v3/docs/subscriptions/list
	std::string listRecentSubscribers(const std::string& clientSecret, const std::string& accessToken, uint8_t maxResults);

	std::string fetch(const std::string& url, const std::string& accessToken);
} // namespace yt::api