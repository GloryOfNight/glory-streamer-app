#include "actors/components/sprite_component.hxx"

#include "core/engine.hxx"

gl::app::sprite_component::sprite_component(const std::string& texPath)
{
	mTexture = engine::get()->LoadTexture(texPath);

	SDL_QueryTexture(mTexture, NULL, NULL, &mWidth, &mHeight);

	mSrcRect.w = mWidth;
	mSrcRect.h = mHeight;
}

void gl::app::sprite_component::setSrcSize(int32_t w, int32_t h)
{
	mSrcRect.w = w;
	mSrcRect.h = h;
}

void gl::app::sprite_component::setAnimSpeed(double speed)
{
	mAnimSpeed = speed;
}

void gl::app::sprite_component::setAnimRepeat(bool repeat)
{
	bAnimRepeat = repeat;
}

void gl::app::sprite_component::setDstSize(int32_t w, int32_t h)
{
	mDstRect.w = w;
	mDstRect.h = h;
}

void gl::app::sprite_component::update(double delta)
{
	if (mAnimSpeed <= 0.0)
		return;

	mAnimTimer += delta;
	if (mAnimTimer >= mAnimSpeed)
	{
		const uint16_t rows = mHeight / mSrcRect.h;
		const uint16_t cols = mWidth / mSrcRect.w;

		mSrcRect.x = ((mSrcRect.x / mSrcRect.w) * mSrcRect.w) + mSrcRect.w;
		if (mSrcRect.x >= mWidth)
		{
			mSrcRect.x = 0;

			mSrcRect.y = ((mSrcRect.y / mSrcRect.h) * mSrcRect.h) + mSrcRect.h;
			if (mSrcRect.y >= mHeight)
			{
				mSrcRect.y = 0;
				if (!bAnimRepeat)
					mAnimSpeed = 0.0;
			}
		}

		mAnimTimer = 0;
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