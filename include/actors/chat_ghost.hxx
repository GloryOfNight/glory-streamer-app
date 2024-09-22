#pragma once

#include "core/actor.hxx"
#include "subsystems/timer_manager.hxx"

#include <string>

namespace gl::app
{
	struct ghost_box
	{
		double x{}, y{};
		double w{}, h{};
	};

	class chat_ghost : public actor
	{
	public:
		static ghost_box& getGhostBox();

		chat_ghost(const std::string& subTitle, const std::string& subId);
		~chat_ghost();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const std::string& getUserName() const { return mUserName; };

		const std::string& getUserId() const { return mUserId; };

		double getSpeed() const { return mSpeed; };
		void setSpeed(double speed);

		void sayHi();

		void doFlipAnim();

		void setMessage(const std::string& message);

		void showMessage(bool bShow);

		void showYoutubeLogo();

		void showTwitchLogo();

		void resetDeathTimer();

	private:
		void generateNewForwardPos();

		void getGhostColor(uint8_t& r, uint8_t& g, uint8_t& b) const;

		class sprite_component* mSpriteComponent;
		class sprite_component* mPlatformLogoSprite;
		class font_component* mGhostTitleFontComponent;
		class font_component* mGhostMessageFontComponent;

		std::string mUserName;
		std::string mUserId;

		timer_handle mUpdateForwardPosTimer;
		double mFwX{}, mFwY{};			   // forward pos
		double mTargetFwX{}, mTargetFwY{}; // forward pos
		double mRemainingMoveTime{0.0};

		timer_handle mDeathTimer;

		double mSpeed{60.0};

		timer_handle mHideMessageTimer;
	};
} // namespace gl::app