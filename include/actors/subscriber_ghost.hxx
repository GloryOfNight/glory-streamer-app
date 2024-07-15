#pragma once

#include "core/actor.hxx"
#include "core/timer_manager.hxx"

#include <string>

namespace gl::app
{
	struct ghost_box
	{
		double x{}, y{};
		double w{}, h{};
	};

	class subscriber_ghost : public actor
	{
	public:
		static ghost_box& getGhostBox();

		subscriber_ghost(const std::string& subTitle, const std::string& subId);
		~subscriber_ghost();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const std::string& getTitle() const { return mSubTitle; };

		const std::string& getChannelId() const { return mSubChannelId; };

		double getSpeed() const { return mSpeed; };
		void setSpeed(double speed);

		void setMessage(const std::string& message);

		void showMessage(bool bShow);

		void destroy();

	private:
		void generateNewForwardPos();

		class sprite_component* mSpriteComponent;
		class font_component* mGhostTitleFontComponent;
		class font_component* mGhostMessageFontComponent;

		std::string mSubTitle;
		std::string mSubChannelId;

		timer_handle mUpdateForwardPosTimer;
		double mFwX{}, mFwY{}; // forward pos

		timer_handle mDeathTimer;

		double mSpeed{60.0};

		timer_handle mHideMessageTimer;
	};
} // namespace gl::app