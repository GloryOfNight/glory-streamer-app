#include "objects/subscriber_ghost.hxx"

#include "core/engine.hxx"

#include <random>

gl::app::subscriber_ghost::subscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mFontComponent{}
	, mSubTitle(subTitle)
	, mSubChannelId(subId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/sprites/ghost/misachi_ghost.png");
	mFontComponent = addComponent<font_component>("assets/fonts/Buran USSR.ttf", 16);

	mFontComponent->setText(mSubTitle);
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

	mX = std::clamp(mX + movedX, 0., static_cast<double>(w));
	mY = std::clamp(mY + movedY, 0., static_cast<double>(h));

	if (mX == 0. || mX == w || mY == 0. || mY == h)
		generateNewForwardPos();

	mSpriteComponent->setSrcOffset(0, 0);

	const int32_t destSize = 64;
	mSpriteComponent->setDstSize(destSize, destSize);
	mSpriteComponent->setDstOffset(mX - destSize / 2, mY - destSize / 2);

	const auto Rect = mFontComponent->getDstRect();
	mFontComponent->setDstOffset(mX - Rect.w / 2, mY - (destSize / 2) - (Rect.h / 2));
}

void gl::app::subscriber_ghost::draw(SDL_Renderer* renderer)
{
	if (bIsHidden)
		return;

	mSpriteComponent->setFlipHorizontal(mFwX < 0);
	mSpriteComponent->draw(renderer);
	mFontComponent->draw(renderer);
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

void gl::app::subscriber_ghost::generateNewForwardPos()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<double> dis(-1, 1);

	mFwX = dis(gen);
	mFwY = dis(gen);
}
