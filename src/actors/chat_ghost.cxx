#include "actors/chat_ghost.hxx"

#include "actors/components/font_component.hxx"
#include "actors/components/sprite_component.hxx"
#include "core/engine.hxx"

#include <cmath>
#include <random>

gl::app::ghost_box& gl::app::chat_ghost::getGhostBox()
{
	static ghost_box ghostBox{0 + 60, 0 + 60, 2560 - 60, 1440 - 60};
	return ghostBox;
}

gl::app::chat_ghost::chat_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mGhostTitleFontComponent{}
	, mSubTitle(subTitle)
	, mSubChannelId(subId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/ghost_sprite.json");

	mPlatformLogoSprite = addComponent<sprite_component>("assets/platform_logo_sprite.json");
	mPlatformLogoSprite->setVisible(true);

	mPlatformLogoSprite->setNextAnimation("youtube");

	mGhostTitleFontComponent = addComponent<font_component>("assets/fonts/Arsenal-Bold.ttf", 22);
	mGhostMessageFontComponent = addComponent<font_component>("assets/fonts/Arsenal-BoldItalic.ttf", 20);
}

gl::app::chat_ghost::~chat_ghost()
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

void gl::app::chat_ghost::init()
{
	actor::init();

	mUpdateForwardPosTimer = timer_manager::get()->addTimer(6.0, std::bind(&chat_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	mDeathTimer = timer_manager::get()->addTimer(600.0, std::bind(&chat_ghost::destroy, this), false);

	const auto& ghostBox = getGhostBox();
	mX = ghostBox.w / 2;
	mY = ghostBox.h / 2;

	mSpriteComponent->setDstSize(60, 60);

	mPlatformLogoSprite->setDstSize(20, 20);

	mGhostTitleFontComponent->setText(mSubTitle);
	mGhostTitleFontComponent->setWrapping(360);
	mGhostTitleFontComponent->setPos(0, -50);

	mGhostMessageFontComponent->setWrapping(360);
	mGhostMessageFontComponent->setPos(0, 24);
}

void gl::app::chat_ghost::update(double delta)
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

	const auto& ghostBox = getGhostBox();
	mX = std::clamp(mX + movedX, ghostBox.x, ghostBox.w);
	mY = std::clamp(mY + movedY, ghostBox.y, ghostBox.h);

	if (mX == ghostBox.x || mX == ghostBox.w || mY == ghostBox.y || mY == ghostBox.h)
		generateNewForwardPos();

	mPlatformLogoSprite->setPos(mGhostTitleFontComponent->getDstRect().w / 2 + 16, -34);

	mSpriteComponent->setFlipHorizontal(mFwX < 0);
	actor::update(delta);
}

void gl::app::chat_ghost::draw(SDL_Renderer* renderer)
{
	actor::draw(renderer);
}

void gl::app::chat_ghost::setSpeed(double speed)
{
	mSpeed = speed;
}

void gl::app::chat_ghost::setMessage(const std::string& message)
{
	if (message == "!flip")
	{
		mSpriteComponent->setNextAnimation("flip");
		return;
	}

	mGhostMessageFontComponent->setText(message);
	mGhostMessageFontComponent->setVisible(true);

	auto timerManager = timer_manager::get();

	timerManager->clearTimer(mHideMessageTimer);
	timerManager->resetTimer(mDeathTimer);

	mHideMessageTimer = timerManager->addTimer(12.0, std::bind(&chat_ghost::showMessage, this, false), false);

	mSpriteComponent->setNextAnimation("talk");
}

void gl::app::chat_ghost::showMessage(bool bShow)
{
	mGhostMessageFontComponent->setVisible(bShow);
}

void gl::app::chat_ghost::destroy()
{
	engine::get()->removeObject(this);
}

void gl::app::chat_ghost::showYoutubeLogo()
{
	mPlatformLogoSprite->setNextAnimation("youtube");
}

void gl::app::chat_ghost::showTwitchLogo()
{
	mPlatformLogoSprite->setNextAnimation("twitch");
}

void gl::app::chat_ghost::generateNewForwardPos()
{
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> fwdDis(-1, 1);
	static std::uniform_real_distribution<double> timeDis(1.5, 10.0);

	mTargetFwX = fwdDis(gen);
	mTargetFwY = fwdDis(gen);

	mRemainingMoveTime = timeDis(gen);
}
