#pragma once

#include "core/actor_component.hxx"

#include <string>

namespace gl::app
{
	class sprite_component : public actor_component
	{
	public:
		sprite_component(const std::string& texPath);

		void setSrcSize(int32_t w, int32_t h);

		void setDstSize(int32_t w, int32_t h);

		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const SDL_Rect& getSrcRect() const { return mSrcRect; }

		void setFlipHorizontal(bool flip);

		void setAngle(double angle);

		void setAnimSpeed(double speed);

		void setAnimRepeat(bool repeat);

	private:
		SDL_Texture* mTexture{};
		int32_t mWidth{}, mHeight{};

		SDL_Rect mSrcRect{};
		SDL_Rect mDstRect{};

		double mAngle{0.0};

		double mAnimSpeed{0.0};
		double mAnimTimer{0.0};
		bool bAnimRepeat{true};

		bool bFlipHorizontal{false};
	};
} // namespace gl::app