#include "actors/components/sprite_component.hxx"

#include "core/engine.hxx"

gl::app::sprite_component::sprite_component(const std::string& spriteSheetAssetJson)
{
	auto spriteSheetPtr = static_cast<const assets::sprite_sheet*>(engine::get()->loadAsset(spriteSheetAssetJson));

	if (spriteSheetPtr)
		mSpriteSheet = *spriteSheetPtr;

	mTexture = engine::get()->loadTexture(mSpriteSheet.resourcePath);

	SDL_GetTextureSize(mTexture, &mWidth, &mHeight);

	setSrcSize(mSpriteSheet.size.w, mSpriteSheet.size.h);
	setDstSize(mSpriteSheet.size.w, mSpriteSheet.size.h);

	if (!mSpriteSheet.defaultAnimation.empty())
		setNextAnimation(mSpriteSheet.defaultAnimation);
}

void gl::app::sprite_component::setSrcSize(int32_t w, int32_t h)
{
	mSrcRect.w = w;
	mSrcRect.h = h;
}

void gl::app::sprite_component::setDstSize(int32_t w, int32_t h)
{
	mDstRect.w = w;
	mDstRect.h = h;
}

void gl::app::sprite_component::update(double delta)
{
	const auto setFrameImageLam = [this](const assets::sprite_sheet::animation::frame& frame)
	{
		const uint16_t rows = static_cast<uint16_t>(mHeight / mSrcRect.h);
		const uint16_t cols = static_cast<uint16_t>(mWidth / mSrcRect.w);

		mSrcRect.x = (frame.index % cols) * mSrcRect.w;
		mSrcRect.y = (frame.index / cols) * mSrcRect.h;
		mRemainingDuration = frame.duration;
	};

	if (mCurrentAnimation == nullptr || mCurrentAnimation->bAllowInterrupt == true || mFrame == 0)
	{
		const auto pred = [&name = mNextAnimationName](const assets::sprite_sheet::animation& anim)
		{ return anim.name == name; };

		const auto findAnim = std::find_if(mSpriteSheet.animations.begin(), mSpriteSheet.animations.end(), pred);

		if (findAnim != mSpriteSheet.animations.end())
		{
			mCurrentAnimation = &(*findAnim);
			mFrame = 0;
			mLoopCounter = 0;

			setFrameImageLam(mCurrentAnimation->frames[mFrame]);
		}
		mNextAnimationName = "";
	}

	if (mCurrentAnimation)
	{
		mRemainingDuration -= delta;
		if (mRemainingDuration <= 0.0)
		{
			if (mFrame + 1 < mCurrentAnimation->frames.size())
			{
				mFrame++;
				setFrameImageLam(mCurrentAnimation->frames[mFrame]);
			}
			else
			{
				mFrame = 0;
				setFrameImageLam(mCurrentAnimation->frames[mFrame]);

				if (mLoopCounter < mCurrentAnimation->repeats)
				{
					++mLoopCounter;
				}
				else if (mCurrentAnimation->repeats != 0)
				{
					if (mNextAnimationName.empty())
						setNextAnimation(mCurrentAnimation->next);

					mCurrentAnimation = nullptr;
					mLoopCounter = 0;
				}
			}
		}
	}
}

void gl::app::sprite_component::draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_FlipMode Flip{};
		if (bFlipHorizontal)
			Flip = SDL_FLIP_HORIZONTAL;

		mDstRect.x = getWorldPosX();
		mDstRect.y = getWorldPosY();

		mDstRect.x -= mDstRect.w / 2;
		mDstRect.y -= mDstRect.h / 2;

		SDL_SetTextureColorMod(mTexture, mColorModeR, mColorModeG, mColorModeB);

		SDL_FPoint center = {mDstRect.w / 2, mDstRect.h / 2};
		SDL_RenderTextureRotated(renderer, mTexture, &mSrcRect, &mDstRect, mAngle, &center, Flip);
	}
}

void gl::app::sprite_component::setFlipHorizontal(bool flip)
{
	bFlipHorizontal = flip;
}

void gl::app::sprite_component::setAngle(double angle)
{
	mAngle = angle;
}

void gl::app::sprite_component::setNextAnimation(const std::string& name)
{
	mNextAnimationName = name;
}

void gl::app::sprite_component::setColorMode(uint8_t r, uint8_t g, uint8_t b)
{
	mColorModeR = r;
	mColorModeG = g;
	mColorModeB = b;
}
