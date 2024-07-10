#pragma once

#include <string>
#include <utility>

namespace yt::data::api
{
	struct auth_info
	{
		std::string accessToken{};
		int32_t expiresIn{};
		std::string refreshToken{};
		std::string scope{};
		std::string tokenType{};
	};

	std::pair<bool, auth_info> beginAuth(const std::string& clientId, const std::string& clientSecret);

	std::string fetchSubscribers(const std::string& apiKey, const std::string& accessToken, bool bRecent, uint8_t maxResults);
} // namespace yt::data::api