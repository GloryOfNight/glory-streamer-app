#include "core/font_component.hxx"

#include "core/engine.hxx"

gl::app::font_component::font_component(const std::string& fontPath, int32_t size)
{
	mFont = engine::get()->LoadFont(fontPath, size);
}

gl::app::font_component::~font_component()
{
}

void gl::app::font_component::setText(const std::string& text)
{
	mText = text;
	bDirty = true;
}

void gl::app::font_component::setSrcOffset(int32_t x, int32_t y)
{
	mSrcRect.x = x;
	mSrcRect.y = y;
}

void gl::app::font_component::update(double delta)
{
}

void gl::app::font_component::draw(SDL_Renderer* renderer)
{
	if (bDirty)
	{
		if (mTexture)
		{
			SDL_DestroyTexture(mTexture);
		}

		SDL_Surface* surface = TTF_RenderUTF8_Blended(mFont, mText.c_str(), {255, 255, 255});
		mTexture = SDL_CreateTextureFromSurface(renderer, surface);

		mSrcRect.w = surface->w;
		mSrcRect.h = surface->h;

		SDL_FreeSurface(surface);
		bDirty = false;
	}

	SDL_RenderCopy(renderer, mTexture, nullptr, &mSrcRect);
}
