#include "objects/subscriber_ghost.hxx"

#include "core/engine.hxx"

gl::app::subsubscriber_ghost::subsubscriber_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mFontComponent{}
	, mSubTitle(subTitle)
	, mSubId(subId)
{
	mSpriteComponent = addComponent<sprite_component>("assets/sprites/ghost/ghost.png");
	mFontComponent = addComponent<font_component>("assets/fonts/Buran USSR.ttf", 16);

	int h, w;
	engine::get()->getWindowSize(&h, &w);

	mX = std::rand() % (h - 64);
	mY = std::rand() % (w - 64);

	mDstX = std::rand() % (h - 128);
	mDstY = std::rand() % (w - 128);

	mFontComponent->setText(mSubTitle);
}

gl::app::subsubscriber_ghost::~subsubscriber_ghost()
{
}

void gl::app::subsubscriber_ghost::init()
{
}

void gl::app::subsubscriber_ghost::update(double delta)
{
	const int32_t speed = 60;
	const double movedPixels = speed * delta;

	const int32_t destSize = 128;

	int32_t h{}, w{};
	engine::get()->getWindowSize(&h, &w);

	if (mX > mDstX)
	{
		mX -= movedPixels;
		if (mX <= mDstX)
		{
			mX = mDstX;
			mDstX = std::rand() % (h - destSize);
		}
	}
	else if (mX < mDstX)
	{
		mX += movedPixels;
		if (mX >= mDstX)
		{
			mX = mDstX;
			mDstX = std::rand() % (h - destSize);
		}
	}

	if (mY > mDstY)
	{
		mY -= movedPixels;
		if (mY <= mDstY)
		{
			mY = mDstY;
			mDstY = std::rand() % (w - destSize);
		}
	}
	else if (mY < mDstY)
	{
		mY += movedPixels;
		if (mY >= mDstY)
		{
			mY = mDstY;
			mDstY = std::rand() % (w - destSize);
		}
	}

	mSpriteComponent->setSrcOffset(0, 0);

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
