#pragma once

#include "core/actor_component.hxx"

#include <string>

namespace gl::app
{
	class sprite_component : public actor_component
	{
	public:
		sprite_component(const std::string& texPath);
		~sprite_component();

		void setSrcSize(int32_t w, int32_t h);

		void setDstSize(int32_t w, int32_t h);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_Rect& getSrcRect() const { return mSrcRect; }

		void setFlipHorizontal(bool flip);

	private:
		SDL_Texture* mTexture{};
		SDL_Rect mSrcRect{};
		SDL_Rect mDstRect{};

		bool bFlipHorizontal{ false };
	};
} // namespace gl::app