#include "actors/components/font_component.hxx"

#include "core/engine.hxx"

gl::app::font_component::font_component(const std::string& fontPath, int32_t size)
{
	mFont = TTF_OpenFont(fontPath.c_str(), size);
}

gl::app::font_component::~font_component()
{
	if (mFont)
	{
		TTF_CloseFont(mFont);
	}
	if (mTexture)
	{
		SDL_DestroyTexture(mTexture);
	}
}

void gl::app::font_component::setText(const std::string& text)
{
	mText = text;
	bDirty = true;
}

void gl::app::font_component::update(double delta)
{
}

void gl::app::font_component::draw(SDL_Renderer* renderer)
{
	if (mFont == nullptr || mText.empty())
		return;

	if (bDirty)
	{
		if (mTexture)
		{
			SDL_DestroyTexture(mTexture);
		}

		TTF_SetFontOutline(mFont, mOutlineSize);
		SDL_Surface* textOutlineSurf = TTF_RenderUTF8_Blended_Wrapped(mFont, mText.c_str(), {0, 0, 0}, mWrapping);

		TTF_SetFontOutline(mFont, 0);
		SDL_Surface* textSurf = TTF_RenderUTF8_Blended_Wrapped(mFont, mText.c_str(), {255, 255, 255}, mWrapping);

		SDL_Rect rect = {mOutlineSize, mOutlineSize, textSurf->w, textSurf->h};

		SDL_SetSurfaceBlendMode(textSurf, SDL_BLENDMODE_BLEND);
		SDL_BlitSurface(textSurf, NULL, textOutlineSurf, &rect);

		mTexture = SDL_CreateTextureFromSurface(renderer, textOutlineSurf);

		mDstRect.w = textSurf->w;
		mDstRect.h = textSurf->h;

		SDL_FreeSurface(textOutlineSurf);
		SDL_FreeSurface(textSurf);
		bDirty = false;
	}

	mDstRect.x = getWorldPosX();
	mDstRect.y = getWorldPosY();

	mDstRect.x -= mDstRect.w / 2;
	//mDstRect.y -= mDstRect.h / 2;

	SDL_RenderCopy(renderer, mTexture, nullptr, &mDstRect);
}

void gl::app::font_component::setWrapping(uint16_t wrapping)
{
	mWrapping = wrapping;
	bDirty = true;
}
