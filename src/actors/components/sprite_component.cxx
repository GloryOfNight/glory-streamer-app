#include "actors/components/sprite_component.hxx"

#include "core/engine.hxx"

gl::app::sprite_component::sprite_component(const std::string& texPath)
{
	mTexture = engine::get()->LoadTexture(texPath);

	SDL_QueryTexture(mTexture, NULL, NULL, &mSrcRect.w, &mSrcRect.h);
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
