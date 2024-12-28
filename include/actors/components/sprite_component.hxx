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

		const SDL_FRect& getSrcRect() const { return mSrcRect; }
		const SDL_FRect& getDstRect() const { return mDstRect; }

		void setFlipHorizontal(bool flip);

		void setAngle(double angle);

		void setNextAnimation(const std::string& name);

		void setColorMode(uint8_t r, uint8_t g, uint8_t b);

	private:
		assets::sprite_sheet mSpriteSheet{};

		SDL_Texture* mTexture{};
		float mWidth{}, mHeight{};

		SDL_FRect mSrcRect{};
		SDL_FRect mDstRect{};

		double mAngle{0.0};

		const assets::sprite_sheet::animation* mCurrentAnimation{};
		uint16_t mFrame{};
		uint16_t mLoopCounter{};
		double mRemainingDuration{};

		std::string mNextAnimationName{};

		uint8_t mColorModeR{255}, mColorModeG{255}, mColorModeB{255};

		bool bFlipHorizontal{false};
	};
} // namespace gl::app