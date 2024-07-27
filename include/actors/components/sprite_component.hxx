#pragma once

#include "assets/sprite_asset.hxx"
#include "core/actor_component.hxx"

#include <string>

namespace gl::app
{
	class sprite_component : public actor_component
	{
	public:
		sprite_component(const std::string& spriteSheetAssetJson);

		void setSrcSize(int32_t w, int32_t h);

		void setDstSize(int32_t w, int32_t h);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_Rect& getSrcRect() const { return mSrcRect; }

		void setFlipHorizontal(bool flip);

		void setAngle(double angle);

		void setNextAnimation(const std::string& name);

	private:
		assets::sprite_sheet mSpriteSheet{};

		SDL_Texture* mTexture{};
		int32_t mWidth{}, mHeight{};

		SDL_Rect mSrcRect{};
		SDL_Rect mDstRect{};

		double mAngle{0.0};

		const assets::sprite_sheet::animation* mCurrentAnimation{};
		uint16_t mFrame{};
		uint16_t mLoopCounter{};
		double mRemainingDuration{};

		std::string mNextAnimationName{};

		bool bFlipHorizontal{false};
	};
} // namespace gl::app