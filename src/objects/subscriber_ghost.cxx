#include "objects/subscriber_ghost.hxx"

#include "core/engine.hxx"

#include <random>

gl::app::subsubscriber_ghost::subsubscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mFontComponent{}
	, mSubTitle(subTitle)
	, mSubId(subId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/sprites/ghost/misachi_ghost.png");
	mFontComponent = addComponent<font_component>("assets/fonts/Buran USSR.ttf", 16);

	mFontComponent->setText(mSubTitle);
}

gl::app::subsubscriber_ghost::~subsubscriber_ghost()
{
}

void gl::app::subsubscriber_ghost::init()
{
	mUpdateForwardPosTimer = engine::get()->getTimerManager()->addTimer(5.0, std::bind(&subsubscriber_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	int32_t h{}, w{};
	engine::get()->getWindowSize(&h, &w);

	mX = w / 2;
	mY = h / 2;
}

void gl::app::subsubscriber_ghost::update(double delta)
{
	int32_t w{}, h{};
	engine::get()->getWindowSize(&h, &w);

	const int32_t speed = 60;

	const double movedX = (mFwX * speed) * delta;
	const double movedY = (mFwY * speed) * delta;

	mX = std::clamp(mX + movedX, 0., static_cast<double>(w));
	mY = std::clamp(mY + movedY, 0., static_cast<double>(h));

	mSpriteComponent->setSrcOffset(0, 0);

	const int32_t destSize = 64;
	mSpriteComponent->setDstSize(destSize, destSize);
	mSpriteComponent->setDstOffset(mX - destSize / 2, mY - destSize / 2);

	const auto Rect = mFontComponent->getDstRect();
	mFontComponent->setDstOffset(mX - Rect.w / 2, mY - (destSize / 2) - (Rect.h / 2));
}

void gl::app::subsubscriber_ghost::draw(SDL_Renderer* renderer)
{
	mSpriteComponent->draw(renderer);
	mFontComponent->draw(renderer);
}

void gl::app::subsubscriber_ghost::generateNewForwardPos()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<double> dis(-1, 1);

	mFwX = dis(gen);
	mFwY = dis(gen);
}
