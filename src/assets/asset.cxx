#include "assets/asset.hxx"

#include "assets/sprite_asset.hxx"

#include <fstream>
#include <nlohmann/json.hpp>

gl::app::assets::sprite_sheet loadSpriteSheet(const gl::app::assets::asset_header& header, const nlohmann::json& json)
{
	using namespace gl::app::assets;
	sprite_sheet sheet = sprite_sheet(header);

	sheet.resourcePath = json["resourcePath"];
	sheet.size.w = json["size"]["w"];
	sheet.size.h = json["size"]["h"];
	sheet.offset.x = json["offset"]["x"];
	sheet.offset.y = json["offset"]["y"];

	if (json.contains("defaultAnimation"))
		sheet.defaultAnimation = json["defaultAnimation"];

	if (json.contains("animations"))
	{
		for (const auto& animationJson : json["animations"])
		{
			auto& anim = sheet.animations.emplace_back(sprite_sheet::animation());
			anim.name = animationJson["name"];

			if (animationJson.contains("repeats"))
				anim.repeats = animationJson["repeats"];

			if (animationJson.contains("allowInterrupt"))
				anim.bAllowInterrupt = animationJson["allowInterrupt"];

			if (animationJson.contains("next"))
				anim.next = animationJson["next"];

			for (const auto& frameJson : animationJson["frames"])
			{
				auto& frame = anim.frames.emplace_back(sprite_sheet::animation::frame());
				frame.index = frameJson["index"];
				frame.duration = frameJson["duration"];
			}
		}
	}

	return sheet;
}

gl::app::assets::eAssetType gl::app::assets::getAssetTypeFromString(const std::string& type)
{
	if (type == "sprite_sheet")
		return eAssetType::SpriteSheet;
	else
		return eAssetType::Unknown;
}

std::unique_ptr<gl::app::assets::asset_header> gl::app::assets::loadAssetJson(const std::string& path)
{
	std::ifstream ifs(path);
	if (ifs.is_open() == false)
	{
		return nullptr;
	}

	const std::string file = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	ifs.close();

	const auto json = nlohmann::json::parse(file);

	asset_header header{};
	header.type = getAssetTypeFromString(json["type"]);
	header.version = json["version"].get<uint32_t>();
	header.name = json["name"];

	if (header.type == eAssetType::SpriteSheet)
	{
		return std::make_unique<sprite_sheet>(loadSpriteSheet(header, json));
	}

	return std::unique_ptr<asset_header>();
}