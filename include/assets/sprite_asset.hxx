#pragma once

#include "asset.hxx"

#include <vector>

namespace gl::app::assets
{
	struct sprite_sheet : public asset_header
	{
		sprite_sheet() = default;

		sprite_sheet(const asset_header& header)
			: asset_header(header)
		{
		}

		std::string resourcePath{};
		struct
		{
			uint16_t w{};
			uint16_t h{};
		} size{};
		struct
		{
			int32_t x{};
			int32_t y{};
		} offset{};

		struct animation
		{
			std::string name{};
			std::string next{};
			uint16_t repeats{};
			bool bAllowInterrupt{false};
			struct frame
			{
				uint32_t index;
				double duration;
			};

			std::vector<frame> frames{};
		};

		std::string defaultAnimation{};
		std::vector<animation> animations;
	};

} // namespace gl::app::assets