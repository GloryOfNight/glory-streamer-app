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

	mPlatformLogoSprite = addComponent<sprite_component>("assets/platform_logo_sprite.json");
	mPlatformLogoSprite->setVisible(true);

	mPlatformLogoSprite->setNextAnimation("youtube");

	mGhostTitleFontComponent = addComponent<font_component>("assets/fonts/Arsenal-Bold.ttf", 22);
	mGhostMessageFontComponent = addComponent<font_component>("assets/fonts/Arsenal-BoldItalic.ttf", 20);
}

gl::app::pony::~pony()
{
	auto timerManager = timer_manager::get();
	if (timerManager)
	{
		timerManager->clearTimer(mUpdateForwardPosTimer);
		timerManager->clearTimer(mHideMessageTimer);
		timerManager->clearTimer(mDeathTimer);
	}
}

void gl::app::pony::init()
{
	actor::init();

	const auto& limitBox = getBox();

	std::random_device rd{};
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> randPosX(limitBox.x, limitBox.w);
	std::uniform_real_distribution<double> randPosY(limitBox.y, limitBox.h);

	mUpdateForwardPosTimer = timer_manager::get()->addTimer(6.0, std::bind(&pony::generateNewForwardPos, this), true);
	generateNewForwardPos();

	mDeathTimer = timer_manager::get()->addTimer(5400.0, std::bind(&pony::destroy, this), false);

	mX = randPosX(gen);
	mY = randPosY(gen);

	mPlatformLogoSprite->setDstSize(20, 20);

	mGhostTitleFontComponent->setText(mUserName);
	mGhostTitleFontComponent->setWrapping(360);
	mGhostTitleFontComponent->setPos(0, -60);

	mGhostMessageFontComponent->setWrapping(360);
	mGhostMessageFontComponent->setPos(0, 24);

	setScale(1.5f);
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

	mSpriteComponent->setFlipHorizontal(std::ceil(mFwX) > 0);

	updateScale();

	actor::update(delta);
}

void gl::app::pony::draw(SDL_Renderer* renderer)
{
	actor::draw(renderer);
}

void gl::app::pony::setScale(float scale)
{
	mScale = scale;
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

void gl::app::pony::updateScale()
{
	const auto& SrcRect = mSpriteComponent->getSrcRect();
	mSpriteComponent->setDstSize(SrcRect.w * mScale, SrcRect.h * mScale);

	int32_t offset = 12 * mScale;

	mGhostTitleFontComponent->setPosY(-mSpriteComponent->getDstRect().h / 2 - mGhostTitleFontComponent->getSize() / 2 + offset);

	mPlatformLogoSprite->setPosY(-mSpriteComponent->getDstRect().h / 2 + offset);
	mPlatformLogoSprite->setPosX(mGhostTitleFontComponent->getDstRect().w / 2 + 10);
}
