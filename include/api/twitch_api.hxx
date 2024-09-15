#pragma once

#include "nlohmann/json.hpp"

#include <format>
#include <string>
#include <utility>
#include <vector>

namespace ttv::api
{
	struct sendChatMessageRequest
	{
		sendChatMessageRequest()
		{
			url = "https://api.twitch.tv/helix/chat/messages";
			json["broadcaster_id"] = "null";
			json["sender_id"] = "null";
			json["message"] = "null";
		}

		sendChatMessageRequest& setBroadcasterId(const std::string broadcasterId)
		{
			json["broadcaster_id"] = broadcasterId;
			return *this;
		}

		sendChatMessageRequest& setSenderId(const std::string userId)
		{
			json["sender_id"] = userId;
			return *this;
		}

		sendChatMessageRequest& setMessage(const std::string message)
		{
			json["message"] = message;
			return *this;
		}

		sendChatMessageRequest& setReplyMessageId(const std::string replyMessageId)
		{
			if (!replyMessageId.empty())
				json["reply_parent_message_id"] = replyMessageId;
			return *this;
		}

		std::string url;

		nlohmann::json json;
	};

	// https://dev.twitch.tv/docs/api/reference/#get-users
	struct listUsers
	{
		listUsers()
		{
			url = "https://api.twitch.tv/helix/users";
		}

		listUsers& setLogin(const std::string& login)
		{
			url.append("?login=" + login);
			return *this;
		}

		std::string url;
	};

	// https://dev.twitch.tv/docs/api/reference/#get-chatters
	struct listChattersRequest
	{
		listChattersRequest(const std::string broadcasterId, const std::string moderatorId)
		{
			url = std::format("https://api.twitch.tv/helix/chat/chatters?broadcaster_id={0}&moderator_id={1}", broadcasterId, moderatorId);
		}

		std::string url;
	};

	struct auth_info
	{
		std::string accessToken{};
		int32_t expiresIn{};
		std::string refreshToken{};
		std::vector<std::string> scope{};
		std::string tokenType{};
	};

	struct eventSubClient
	{
		eventSubClient() = default;
		eventSubClient(const eventSubClient&) = delete;
		eventSubClient(eventSubClient&) = delete;
		~eventSubClient();

		bool connect();

		std::string recv();

	private:
		void* mCurl{};
	};

	std::pair<bool, auth_info> initialAuth(const std::string clientId, const std::string clientSecret);

	std::pair<bool, auth_info> refreshAuth(const std::string clientId, const std::string clientSecret, const std::string refreshToken);

	std::string fetch(const std::string url, const std::string accessToken, const std::string clientId);

	std::string post(const std::string url, const std::string accessToken, const std::string clientId, const std::string postJson);
} // namespace ttv::api