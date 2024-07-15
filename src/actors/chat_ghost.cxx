#include "actors/chat_ghost.hxx"

#include "actors/components/font_component.hxx"
#include "actors/components/sprite_component.hxx"
#include "core/engine.hxx"

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
	auto timerManager = eng->getTimerManager();
	timerManager->clearTimer(mUpdateForwardPosTimer);
	timerManager->clearTimer(mHideMessageTimer);
	timerManager->clearTimer(mDeathTimer);
}

void gl::app::subscriber_ghost::init()
{
	actor::init();

	mUpdateForwardPosTimer = engine::get()->getTimerManager()->addTimer(5.0, std::bind(&subscriber_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	mDeathTimer = engine::get()->getTimerManager()->addTimer(600.0, std::bind(&subscriber_ghost::destroy, this), false);

	const auto& ghostBox = getGhostBox();
	mX = ghostBox.w / 2;
	mY = ghostBox.h / 2;

	mSpriteComponent->setDstSize(64, 64);
	mGhostTitleFontComponent->setPos(0, -48);
	mGhostMessageFontComponent->setPos(0, 32);
}

void gl::app::subscriber_ghost::update(double delta)
{
	const double movedX = (mFwX * mSpeed) * delta;
	const double movedY = (mFwY * mSpeed) * delta;

	const auto& ghostBox = getGhostBox();
	mX = std::clamp(mX + movedX, ghostBox.x, ghostBox.w);
	mY = std::clamp(mY + movedY, ghostBox.y, ghostBox.h);

	if (mX == ghostBox.x || mX == ghostBox.w || mY == ghostBox.y || mY == ghostBox.h)
		generateNewForwardPos();

	actor::update(delta);
}

void gl::app::subscriber_ghost::draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawPoint(renderer, static_cast<int>(mX), static_cast<int>(mY));

	mSpriteComponent->setFlipHorizontal(mFwX < 0);
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

	auto timerManager = engine::get()->getTimerManager();

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

	mFwX = dis(gen);
	mFwY = dis(gen);
}
