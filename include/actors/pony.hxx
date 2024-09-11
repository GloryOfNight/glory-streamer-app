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

	private:
		void generateNewForwardPos();

		class sprite_component* mSpriteComponent;

		std::string mUserName;
		std::string mUserId;

		timer_handle mUpdateForwardPosTimer;
		double mFwX{}, mFwY{};			   // forward pos
		double mTargetFwX{}, mTargetFwY{}; // forward pos
		double mRemainingMoveTime{0.0};

		double mSpeed{60.0};
	};
} // namespace gl::app