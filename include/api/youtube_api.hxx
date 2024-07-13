#pragma once

#include <string>
#include <utility>
#include <vector>

namespace yt::api::live
{
	static void appendArray(std::string& source, const std::vector<std::string>& values)
	{
		const size_t num = values.size();
		source.append("&part=");
		for (size_t i = 0; i < num; ++i)
		{
			source.append(values[i]);
			if (i + 1 < num)
			{
				source.append(",");
			}
		}
	}

	static std::string boolToString(bool value)
	{
		return value ? "true" : "false";
	}

	// https://developers.google.com/youtube/v3/live/docs/liveBroadcasts/list
	struct listLiveBroadcastsRequest
	{
		listLiveBroadcastsRequest(const std::string& key)
		{
			url = std::string("https://www.googleapis.com/youtube/v3/liveBroadcasts?key=") + key;
		}

		// values: id, snippet, contentDetails, monetizationDetails, and status.
		listLiveBroadcastsRequest& setParts(const std::vector<std::string>& parts)
		{
			appendArray(url, parts);
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

	// https://developers.google.com/youtube/v3/live/docs/liveChatMessages/list
	struct listLiveChatMessagesRequest
	{
		listLiveChatMessagesRequest(const std::string& key)
		{
			url = std::string("https://www.googleapis.com/youtube/v3/liveChat/messages?key=") + key;
		}

		// values: id, snippet, authorDetails.
		listLiveChatMessagesRequest& setParts(const std::vector<std::string>& parts)
		{
			appendArray(url, parts);
			return *this;
		}

		listLiveChatMessagesRequest& setLiveChatId(const std::string& id)
		{
			url.append("&liveChatId=" + id);
			return *this;
		}

		// value: 200 - 2000
		listLiveChatMessagesRequest& setMaxResults(uint16_t maxResults)
		{
			url.append("&maxResults=" + std::to_string(maxResults));
			return *this;
		}

		std::string url;
	};

	// https://developers.google.com/youtube/v3/docs/subscriptions/list
	struct listSubscribtionsRequest
	{
		listSubscribtionsRequest(const std::string& key)
		{
			url = std::string("https://youtube.googleapis.com/youtube/v3/subscriptions?key=") + key;
		}

		// values: contentDetails, id, snippet, subscriberSnippet.
		listSubscribtionsRequest& setParts(const std::vector<std::string>& parts)
		{
			appendArray(url, parts);
			return *this;
		}

		listSubscribtionsRequest& setMyRecentSubscribers(bool value)
		{
			url.append("&myRecentSubscribers=" + boolToString(value));
			return *this;
		}

		listSubscribtionsRequest& setMySubscribers(bool value)
		{
			url.append("&mySubscribers=" + boolToString(value));
			return *this;
		}

		// values: alphabetical, relevance, unread
		listSubscribtionsRequest& setOrder(const std::string& order)
		{
			url.append("&order=" + order);
			return *this;
		}

		// value: 0 - 50
		listSubscribtionsRequest& setMaxResults(uint16_t maxResults)
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

	std::pair<bool, auth_info> initalAuth(const std::string clientId, const std::string clientSecret);

	std::pair<bool, auth_info> refreshAuth(const std::string clientId, const std::string clientSecret, const std::string refreshToken);

	std::string fetch(const std::string url, const std::string accessToken, const std::string eTag);
} // namespace yt::api