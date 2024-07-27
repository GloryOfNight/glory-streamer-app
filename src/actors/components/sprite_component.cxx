#include "actors/components/sprite_component.hxx"

#include "core/engine.hxx"

gl::app::sprite_component::sprite_component(const std::string& spriteSheetAssetJson)
{
	auto spriteSheetPtr = static_cast<const assets::sprite_sheet*>(engine::get()->loadAsset(spriteSheetAssetJson));

	if (spriteSheetPtr)
		mSpriteSheet = *spriteSheetPtr;

	mTexture = engine::get()->LoadTexture(mSpriteSheet.resourcePath);

	SDL_QueryTexture(mTexture, NULL, NULL, &mWidth, &mHeight);

	mSrcRect.w = mSpriteSheet.size.w;
	mSrcRect.h = mSpriteSheet.size.h;
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
	const uint16_t rows = mHeight / mSrcRect.h;
	const uint16_t cols = mWidth / mSrcRect.w;

	if (mCurrentAnimation == nullptr || mCurrentAnimation->bFinishBeforeTransition == false || mFrame == 0)
	{
		const auto pred = [&name = mNextAnimationName](const assets::sprite_sheet::animation& anim)
		{ return anim.name == name; };

		const auto findAnim = std::find_if(mSpriteSheet.animations.begin(), mSpriteSheet.animations.end(), pred);

		if (findAnim != mSpriteSheet.animations.end())
		{
			mCurrentAnimation = &(*findAnim);
			mFrame = 0;

			const auto currentFrame = &mCurrentAnimation->frames[mFrame];

			mRemainingDuration = currentFrame->duration;
			mSrcRect.x = (currentFrame->index * mSrcRect.w) % mWidth;
			mSrcRect.y = (currentFrame->index * mSrcRect.h) % mHeight;
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
			}
			else
			{
				if (mCurrentAnimation->bLoop)
				{
					mFrame = 0;
				}
				else
				{
					mCurrentAnimation = nullptr;
					mFrame = 0;
				}
			}

			const auto currentFrame = &mCurrentAnimation->frames[mFrame];
			mRemainingDuration = currentFrame->duration;
			mSrcRect.x = (currentFrame->index * mSrcRect.w) % mWidth;
			mSrcRect.y = (currentFrame->index * mSrcRect.h) % mHeight;
		}
	}
}

void gl::app::sprite_component::draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_RendererFlip Flip{};
		if (bFlipHorizontal)
			Flip = SDL_FLIP_HORIZONTAL;

		mDstRect.x = getWorldPosX();
		mDstRect.y = getWorldPosY();

		mDstRect.x -= mDstRect.w / 2;
		mDstRect.y -= mDstRect.h / 2;

		SDL_Point center = {mDstRect.w / 2, mDstRect.h / 2};
		SDL_RenderCopyEx(renderer, mTexture, &mSrcRect, &mDstRect, mAngle, &center, Flip);
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
