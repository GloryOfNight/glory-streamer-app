#include "actors/pony.hxx"

#include "actors/components/font_component.hxx"
#include "actors/components/sprite_component.hxx"
#include "core/engine.hxx"

#include <cmath>
#include <random>

gl::app::pony_box& gl::app::pony::getBox()
{
	static pony_box box{0 + 60, 0 + 60, 2560 - 60, 1440 - 60};
	return box;
}

gl::app::pony::pony(const std::string& userName, const std::string& userId)
	: mSpriteComponent{}
	, mUserName(userName)
	, mUserId(userId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/bread_pony.json");
}

gl::app::pony::~pony()
{
}

void gl::app::pony::init()
{
	actor::init();

	mUpdateForwardPosTimer = timer_manager::get()->addTimer(6.0, std::bind(&pony::generateNewForwardPos, this), true);
	generateNewForwardPos();

	const auto& ghostBox = getBox();
	mX = ghostBox.w / 2;
	mY = ghostBox.h / 2;
}

void gl::app::pony::update(double delta)
{
	if (mRemainingMoveTime > 0.0)
	{
		mRemainingMoveTime -= delta;
		if (mRemainingMoveTime <= 0.0)
		{
			mTargetFwX = 0.0;
			mTargetFwY = 0.0;
		}
	}

	const double rotSpeed = delta;
	if (mFwX > mTargetFwX)
	{
		mFwX -= rotSpeed;
		if (mFwX < mTargetFwX)
			mFwX = mTargetFwX;
	}
	else if (mFwX < mTargetFwX)
	{
		mFwX += rotSpeed;
		if (mFwX > mTargetFwX)
			mFwX = mTargetFwX;
	}

	if (mFwY > mTargetFwY)
	{
		mFwY -= rotSpeed;
		if (mFwY < mTargetFwY)
			mFwY = mTargetFwY;
	}
	else if (mFwY < mTargetFwY)
	{
		mFwY += rotSpeed;
		if (mFwY > mTargetFwY)
			mFwY = mTargetFwY;
	}

	const double movedX = (mFwX * mSpeed) * delta;
	const double movedY = (mFwY * mSpeed) * delta;

	const auto& ghostBox = getBox();
	mX = std::clamp(mX + movedX, ghostBox.x, ghostBox.w);
	mY = std::clamp(mY + movedY, ghostBox.y, ghostBox.h);

	if (mX == ghostBox.x || mX == ghostBox.w || mY == ghostBox.y || mY == ghostBox.h)
		generateNewForwardPos();

	mSpriteComponent->setFlipHorizontal(mFwX > 0);
	actor::update(delta);
}

void gl::app::pony::draw(SDL_Renderer* renderer)
{
	actor::draw(renderer);
}

void gl::app::pony::generateNewForwardPos()
{
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> fwdDis(-1, 1);
	static std::uniform_real_distribution<double> timeDis(1.5, 10.0);

	mTargetFwX = fwdDis(gen);
	mTargetFwY = fwdDis(gen);

	mRemainingMoveTime = timeDis(gen);
}
