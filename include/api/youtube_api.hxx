#pragma once

#include <string>
#include <utility>
#include <vector>

namespace yt::api::live
{
	struct listLiveBroadcastsRequest
	{
		listLiveBroadcastsRequest(const std::string& key)
			: url("https://www.googleapis.com/youtube/v3/liveBroadcasts?part=snippet&broadcastStatus=active&key=" + key)
		{
		}

		// values: id, snippet, contentDetails, monetizationDetails, and status.
		listLiveBroadcastsRequest& setParts(const std::vector<std::string>& parts)
		{
			const size_t totalParts = parts.size();
			url.append("&part=");
			for (size_t i = 0; i < totalParts; ++i)
			{
				url.append(parts[i]);
				if (i + 1 < totalParts)
				{
					url.append(",");
				}
			}
			return *this;
		}

		// values: active, all, completed, upcoming
		listLiveBroadcastsRequest& setBroadcastStatus(const std::string& status)
		{
			url.append("&broadcastStatus=" + status);
			return *this;
		}

		// values: event, persistent
		listLiveBroadcastsRequest& setBroadcastType(const std::string& type)
		{
			url.append("&broadcastType=" + type);
			return *this;
		}

		// values: 0 - 50
		listLiveBroadcastsRequest& setMaxResults(uint16_t maxResults)
		{
			url.append("&maxResults=" + std::to_string(maxResults));
			return *this;
		}

		std::string url;
	};
} // namespace yt::api::live

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