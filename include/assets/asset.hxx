#pragma once

#include <memory>
#include <stdint.h>
#include <string>

namespace gl::app::assets
{
	enum class eAssetType : uint8_t
	{
		Unknown,
		SpriteSheet
	};

	struct asset_header
	{
		eAssetType type{};
		uint32_t version{UINT32_MAX};
		std::string name{};
	};

	eAssetType getAssetTypeFromString(const std::string& type);

	std::unique_ptr<asset_header> loadAssetJson(const std::string& path);

} // namespace gl::app::assets