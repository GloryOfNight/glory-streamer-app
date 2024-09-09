#pragma once

#include <string>
#include <utility>

namespace ttv::api
{
	struct auth_info
	{
		std::string accessToken{};
		int32_t expiresIn{};
		std::string scope{};
		std::string tokenType{};
	};

	std::pair<bool, auth_info> initialAuth(const std::string clientId, const std::string clientSecret);
} // namespace ttv::api