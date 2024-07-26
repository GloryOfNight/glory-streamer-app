#include "actors/chat_ghost.hxx"

#include "actors/components/font_component.hxx"
#include "actors/components/sprite_component.hxx"
#include "core/engine.hxx"

#include <cmath>
#include <random>

gl::app::ghost_box& gl::app::subscriber_ghost::getGhostBox()
{
	static ghost_box ghostBox{0, 0, 2560, 1440};
	return ghostBox;
}

gl::app::subscriber_ghost::subscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mGhostTitleFontComponent{}
	, mSubTitle(subTitle)
	, mSubChannelId(subId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/sprites/ghost/misachi_ghost.png");
	mGhostTitleFontComponent = addComponent<font_component>("assets/fonts/Buran USSR.ttf", 16);
	mGhostMessageFontComponent = addComponent<font_component>("assets/fonts/Arsenal-BoldItalic.ttf", 18);

	mGhostTitleFontComponent->setText(mSubTitle);
	mGhostTitleFontComponent->setWrapping(360);
	mGhostMessageFontComponent->setWrapping(360);
}

gl::app::subscriber_ghost::~subscriber_ghost()
{
	auto eng = engine::get();
	auto timerManager = timer_manager::get();
	if (timerManager)
	{
		timerManager->clearTimer(mUpdateForwardPosTimer);
		timerManager->clearTimer(mHideMessageTimer);
		timerManager->clearTimer(mDeathTimer);
	}
}

void gl::app::subscriber_ghost::init()
{
	actor::init();

	mUpdateForwardPosTimer = timer_manager::get()->addTimer(5.0, std::bind(&subscriber_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	mDeathTimer = timer_manager::get()->addTimer(600.0, std::bind(&subscriber_ghost::destroy, this), false);

	const auto& ghostBox = getGhostBox();
	mX = ghostBox.w / 2;
	mY = ghostBox.h / 2;

	mSpriteComponent->setDstSize(64, 64);
	mGhostTitleFontComponent->setPos(0, -48);
	mGhostMessageFontComponent->setPos(0, 32);
}

void gl::app::subscriber_ghost::update(double delta)
{
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

	const auto& ghostBox = getGhostBox();
	mX = std::clamp(mX + movedX, ghostBox.x, ghostBox.w);
	mY = std::clamp(mY + movedY, ghostBox.y, ghostBox.h);

	if (mX == ghostBox.x || mX == ghostBox.w || mY == ghostBox.y || mY == ghostBox.h)
		generateNewForwardPos();

	mSpriteComponent->setFlipHorizontal(mFwX < 0);
	actor::update(delta);
}

void gl::app::subscriber_ghost::draw(SDL_Renderer* renderer)
{
	actor::draw(renderer);
}

void gl::app::subscriber_ghost::setSpeed(double speed)
{
	mSpeed = speed;
}

void gl::app::subscriber_ghost::setMessage(const std::string& message)
{
	mGhostMessageFontComponent->setText(message);
	mGhostMessageFontComponent->setVisible(true);

	auto timerManager = timer_manager::get();

	timerManager->clearTimer(mHideMessageTimer);
	timerManager->resetTimer(mDeathTimer);

	mHideMessageTimer = timerManager->addTimer(12.0, std::bind(&subscriber_ghost::showMessage, this, false), false);
}

void gl::app::subscriber_ghost::showMessage(bool bShow)
{
	mGhostMessageFontComponent->setVisible(bShow);
}

void gl::app::subscriber_ghost::destroy()
{
	engine::get()->removeObject(this);
}

void gl::app::subscriber_ghost::generateNewForwardPos()
{
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> dis(-1, 1);

	mTargetFwX = dis(gen);
	mTargetFwY = dis(gen);
}
