#pragma once

#include "core/actor_component.hxx"

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace gl::app
{
	class font_component : public actor_component
	{
	public:
		font_component(const std::string& fontPath, int32_t size);
		~font_component();

		void setText(const std::string& text);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_FRect& getDstRect() const { return mDstRect; }

		float getSize() const { return mFont ? TTF_GetFontSize(mFont) : 0.f; }

		void setWrapping(uint16_t wrapping);

	private:
		TTF_Font* mFont{};

		SDL_Texture* mTexture{}; // owning!

		std::string mText{};
		SDL_FRect mDstRect{};

		uint16_t mWrapping{120};

		uint8_t mOutlineSize{1};

		bool bDirty{true};
	};
} // namespace gl::app