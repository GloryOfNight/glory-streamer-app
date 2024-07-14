#include "objects/subscriber_ghost.hxx"

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
	mGhostMessageFontComponent->setWrapping(360);
}

gl::app::subscriber_ghost::~subscriber_ghost()
{
}

void gl::app::subscriber_ghost::init()
{
	mUpdateForwardPosTimer = engine::get()->getTimerManager()->addTimer(5.0, std::bind(&subscriber_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	int32_t w{}, h{};
	engine::get()->getWindowSize(&w, &h);

	mX = w / 2;
	mY = h / 2;
}

void gl::app::subscriber_ghost::update(double delta)
{
	int32_t w{}, h{};
	engine::get()->getWindowSize(&w, &h);

	const double movedX = (mFwX * mSpeed) * delta;
	const double movedY = (mFwY * mSpeed) * delta;

	const auto& ghostBox = getGhostBox();
	mX = std::clamp(mX + movedX, ghostBox.x, ghostBox.w);
	mY = std::clamp(mY + movedY, ghostBox.y, ghostBox.h);

	if (mX == 0. || mX == w || mY == 0. || mY == h)
		generateNewForwardPos();

	mSpriteComponent->setSrcOffset(0, 0);

	const int32_t destSize = 64;
	mSpriteComponent->setDstSize(destSize, destSize);
	mSpriteComponent->setDstOffset(mX - destSize / 2, mY - destSize / 2);

	auto Rect = mGhostTitleFontComponent->getDstRect();
	mGhostTitleFontComponent->setDstOffset(mX - Rect.w / 2, mY - (destSize / 2) - (Rect.h / 2) - 5);

	Rect = mGhostMessageFontComponent->getDstRect();
	mGhostMessageFontComponent->setDstOffset(mX - Rect.w / 2, mY + (destSize / 2));
}

void gl::app::subscriber_ghost::draw(SDL_Renderer* renderer)
{
	if (bIsHidden)
		return;

	mSpriteComponent->setFlipHorizontal(mFwX < 0);
	mSpriteComponent->draw(renderer);

	mGhostTitleFontComponent->draw(renderer);

	if (!bHideMessage)
		mGhostMessageFontComponent->draw(renderer);
}

void gl::app::subscriber_ghost::getPos(double* x, double* y) const
{
	if (x)
		*x = mX;
	if (y)
		*y = mY;
}

void gl::app::subscriber_ghost::setPos(double x, double y)
{
	mX = x;
	mY = y;
}

void gl::app::subscriber_ghost::setSpeed(double speed)
{
	mSpeed = speed;
}

void gl::app::subscriber_ghost::setHidden(bool hidden)
{
	bIsHidden = hidden;
}

void gl::app::subscriber_ghost::setMessage(const std::string& message)
{
	mGhostMessageFontComponent->setText(message);
	bHideMessage = false;

	auto timerManager = engine::get()->getTimerManager();

	if (mHideMessageTimer)
	{
		timerManager->clearTimer(mHideMessageTimer);
	}

	mHideMessageTimer = timerManager->addTimer(12.0, std::bind(&subscriber_ghost::showMessage, this, false), false);
}

void gl::app::subscriber_ghost::generateNewForwardPos()
{
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> dis(-1, 1);

	mFwX = dis(gen);
	mFwY = dis(gen);
}
