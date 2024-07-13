#pragma once

#include "core/font_component.hxx"
#include "core/object.hxx"
#include "core/sprite_component.hxx"
#include "core/timer_manager.hxx"

namespace gl::app
{
	class subscriber_ghost : public object
	{
	public:
		subscriber_ghost(const std::string& subTitle, const std::string& subId);
		~subscriber_ghost();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		void getPos(double* x, double* y) const;
		void setPos(double x, double y);

		const std::string& getTitle() const { return mSubTitle; };

		const std::string& getChannelId() const { return mSubChannelId; };

		double getSpeed() const { return mSpeed; };
		void setSpeed(double speed);

		bool isHidden() const { return bIsHidden; };
		void setHidden(bool hidden);

	private:
		void generateNewForwardPos();

		sprite_component* mSpriteComponent;
		font_component* mFontComponent;

		std::string mSubTitle;
		std::string mSubChannelId;

		double mX{}, mY{}; // current screen pos

		timer_handle mUpdateForwardPosTimer;
		double mFwX{}, mFwY{}; // forward pos

		double mSpeed{60.0};

		bool bIsHidden{};
	};
} // namespace gl::app