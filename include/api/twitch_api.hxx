#pragma once

#include <format>
#include <string>
#include <utility>
#include <vector>

namespace ttv::api
{
	// https://dev.twitch.tv/docs/api/reference/#get-users
	struct ListUsers
	{
		ListUsers()
		{
			url = "https://api.twitch.tv/helix/users";
		}

		ListUsers& setLogin(const std::string& login)
		{
			url.append("?login=" + login);
			return *this;
		}

		std::string url;
	};

	// https://dev.twitch.tv/docs/api/reference/#get-chatters
	struct ListChattersRequest
	{
		ListChattersRequest(const std::string broadcasterId, const std::string moderatorId)
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

	std::pair<bool, auth_info> initialAuth(const std::string clientId, const std::string clientSecret);

	std::string fetch(const std::string url, const std::string accessToken, const std::string clientId);
} // namespace ttv::api