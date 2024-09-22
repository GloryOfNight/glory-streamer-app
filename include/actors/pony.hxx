#pragma once

#include "core/actor.hxx"
#include "subsystems/timer_manager.hxx"

#include <string>

namespace gl::app
{
	struct pony_box
	{
		double x{}, y{};
		double w{}, h{};
	};

	class pony : public actor
	{
	public:
		static pony_box& getBox();

		pony(const std::string& userName, const std::string& userId);
		~pony();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		float getScale() const { return mScale; }
		void setScale(float scale);

		const std::string& getUserId() const { return mUserId; }

		const std::string& getUserName() const { return mUserName; }

	private:
		void generateNewForwardPos();

		void updateScale();

		class sprite_component* mSpriteComponent;
		class sprite_component* mPlatformLogoSprite;
		class font_component* mGhostTitleFontComponent;
		class font_component* mGhostMessageFontComponent;

		std::string mUserName;
		std::string mUserId;

		double mFwX{}, mFwY{};			   // forward pos
		double mTargetFwX{}, mTargetFwY{}; // forward pos
		double mRemainingMoveTime{0.0};
		double mSpeed{60.0};

		timer_handle mUpdateForwardPosTimer;
		timer_handle mDeathTimer;
		timer_handle mHideMessageTimer;

		float mScale{1.0};
	};
} // namespace gl::app