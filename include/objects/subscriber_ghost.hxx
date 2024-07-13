#pragma once

#include "core/font_component.hxx"
#include "core/object.hxx"
#include "core/sprite_component.hxx"
#include "core/timer_manager.hxx"

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

		const std::string& getChannelId() const
		{
			return mSubId;
		};

	private:
		void generateNewForwardPos();

		sprite_component* mSpriteComponent;
		font_component* mFontComponent;

		std::string mSubTitle;
		std::string mSubId;

		double mX{}, mY{}; // current screen pos

		timer_handle mUpdateForwardPosTimer;
		double mFwX{}, mFwY{}; // forward pos
	};
} // namespace gl::app