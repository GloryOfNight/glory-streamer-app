#include "core/sprite_component.hxx"

#include "core/engine.hxx"

gl::app::sprite_component::sprite_component(const std::string& texPath)
{
	mTexture = engine::get()->LoadTexture(texPath);

	SDL_QueryTexture(mTexture, NULL, NULL, &mSrcRect.w, &mSrcRect.h);
}

gl::app::sprite_component::~sprite_component()
{
}

void gl::app::sprite_component::setSrcSize(int32_t w, int32_t h)
{
	mSrcRect.w = w;
	mSrcRect.h = h;
}

void gl::app::sprite_component::setSrcOffset(int32_t x, int32_t y)
{
	mSrcRect.x = x;
	mSrcRect.y = y;
}

void gl::app::sprite_component::setDstSize(int32_t w, int32_t h)
{
	mDstRect.w = w;
	mDstRect.h = h;
}

void gl::app::sprite_component::setDstOffset(int32_t x, int32_t y)
{
	mDstRect.x = x;
	mDstRect.y = y;
}

void gl::app::sprite_component::update(double delta)
{
}

void gl::app::sprite_component::draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_RenderCopy(renderer, mTexture, &mSrcRect, &mDstRect);
	}
}