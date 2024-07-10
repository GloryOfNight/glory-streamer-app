#pragma once

#include "core/font_component.hxx"
#include "core/object.hxx"
#include "core/sprite_component.hxx"

namespace gl::app
{
	class subsubscriber_ghost : public object
	{
	public:
		subsubscriber_ghost(const std::string& subTitle, const std::string& subId);
		~subsubscriber_ghost();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

	private:
		sprite_component mSpriteComponent;
		font_component mFontComponent;

		std::string mSubTitle;
		std::string mSubId;

		int32_t mX, mY; // screen pos
	};
} // namespace gl::app