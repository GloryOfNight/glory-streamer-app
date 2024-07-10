#pragma once

#include "object_component.hxx"

#include <string>

namespace gl::app
{
	class sprite_component : public object_component
	{
	public:
		sprite_component(const std::string& texPath);
		~sprite_component();

		void setSrcSize(int32_t w, int32_t h);
		void setSrcOffset(int32_t x, int32_t y);

		void setDstSize(int32_t w, int32_t h);
		void setDstOffset(int32_t x, int32_t y);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_Rect& getSrcRect() const { return mSrcRect; }

	private:
		SDL_Texture* mTexture{};
		SDL_Rect mSrcRect{};
		SDL_Rect mDstRect{};
	};
} // namespace gl::app