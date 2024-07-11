#include "objects/subscriber_ghost.hxx"

#include "core/engine.hxx"

gl::app::subsubscriber_ghost::subsubscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent("assets/sprites/ghost/first_test_ghost.png")
	, mFontComponent("assets/fonts/Buran USSR.ttf", 10)
	, mSubTitle(subTitle)
	, mSubId(subId)
{
	int h, w;
	engine::get()->getWindowSize(&h, &w);

	mX = std::rand() % (h - 64);
	mY = std::rand() % (w - 64);

	mFontComponent.setText(mSubTitle);
}

gl::app::subsubscriber_ghost::~subsubscriber_ghost()
{
}

void gl::app::subsubscriber_ghost::init()
{
}

void gl::app::subsubscriber_ghost::update(double delta)
{
	const int destSize = 64;

	mSpriteComponent.setSrcOffset(0, 0);

	mSpriteComponent.setDstSize(destSize, destSize);
	mSpriteComponent.setDstOffset(mX - destSize / 2, mY - destSize / 2);

	const auto Rect = mFontComponent.getDstRect();
	mFontComponent.setDstOffset(mX - Rect.w / 2, mY - (destSize / 2) - (Rect.h / 2));
}

void gl::app::subsubscriber_ghost::draw(SDL_Renderer* renderer)
{
	mSpriteComponent.draw(renderer);
	mFontComponent.draw(renderer);
}
