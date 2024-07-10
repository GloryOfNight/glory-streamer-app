#include "objects/subscriber_ghost.hxx"

gl::app::subsubscriber_ghost::subsubscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent("assets/sprites/ghost/first_test_ghost.png")
	, mFontComponent("assets/fonts/Buran USSR.ttf", 20)
	, mSubTitle(subTitle)
	, mSubId(subId)
{
	mX = std::rand() % 1920;
	mY = std::rand() % 1080;

	mSpriteComponent.setSrcOffset(0, 0);
	mSpriteComponent.setDstSize(128, 128);
	mSpriteComponent.setDstOffset(mX, mY);


	mFontComponent.setSrcOffset(mX, mY);
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
}

void gl::app::subsubscriber_ghost::draw(SDL_Renderer* renderer)
{
	mSpriteComponent.draw(renderer);
	mFontComponent.draw(renderer);
}
