#pragma once

#include "object_component.hxx"

#include <SDL_ttf.h>
#include <string>

namespace gl::app
{
	class font_component : public object_component
	{
	public:
		font_component(const std::string& fontPath, int32_t size);
		~font_component();

		void setText(const std::string& text);

		void setSrcOffset(int32_t x, int32_t y);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_Rect& getSrcRect() const { return mSrcRect; }

	private:
		TTF_Font* mFont{};

		SDL_Texture* mTexture{}; // owning!

		std::string mText{};
		SDL_Rect mSrcRect{};

		bool bDirty{true};
	};
} // namespace gl::app