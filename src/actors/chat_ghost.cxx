#include "actors/chat_ghost.hxx"

#include "actors/components/font_component.hxx"
#include "actors/components/sprite_component.hxx"
#include "core/engine.hxx"

#include <array>
#include <cmath>
#include <random>

// clang-format off
static std::array<uint8_t[3], 8> gGhostColors =
{

		{
			{255,250,250},		// Snow
			{240,248,255},		// Alice Blue
			{240,255,255},		// Azure Mist
			{248,248,255},		// Ghost White
			{245,255,250},		// Mint Cream
			{255,255,240},		// Ivory
			{249,255,227},		// Pomelo White
			{240,255,240}		// Honeydew
		}
};
// clang-format on

gl::app::ghost_box&
gl::app::chat_ghost::getGhostBox()
{
	static ghost_box ghostBox{0 + 60, 0 + 60, 2560 - 60, 1440 - 60};
	return ghostBox;
}

gl::app::chat_ghost::chat_ghost(const std::string& subTitle, const std::string& subId)
	: mSpriteComponent{}
	, mGhostTitleFontComponent{}
	, mUserName(subTitle)
	, mUserId(subId)
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

	const auto& ghostBox = getGhostBox();

	std::random_device rd{};
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> randPosX(ghostBox.x, ghostBox.w);
	std::uniform_real_distribution<double> randPosY(ghostBox.y, ghostBox.h);

	mUpdateForwardPosTimer = timer_manager::get()->addTimer(6.0, std::bind(&chat_ghost::generateNewForwardPos, this), true);
	generateNewForwardPos();

	mDeathTimer = timer_manager::get()->addTimer(5400.0, std::bind(&chat_ghost::destroy, this), false);

	mX = randPosX(gen);
	mY = randPosY(gen);

	mSpriteComponent->setDstSize(60, 60);

	uint8_t r, g, b;
	getGhostColor(r, g, b);
	mSpriteComponent->setColorMode(r, g, b);

	mPlatformLogoSprite->setDstSize(20, 20);

	mGhostTitleFontComponent->setText(mUserName);
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

	mPlatformLogoSprite->setPos(mGhostTitleFontComponent->getDstRect().w / 2 + 10, -34);

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

void gl::app::chat_ghost::sayHi()
{
	const std::vector<std::string> greetings = {"Привет стрим!", "Привет всем!", "Привет чат!"};
	setMessage(greetings[std::rand() % greetings.size()]);
}

void gl::app::chat_ghost::doFlipAnim()
{
	mSpriteComponent->setNextAnimation("flip");
}

void gl::app::chat_ghost::setMessage(const std::string& message)
{
	mGhostMessageFontComponent->setText(message);
	mGhostMessageFontComponent->setVisible(true);

	auto timerManager = timer_manager::get();

	timerManager->clearTimer(mHideMessageTimer);
	resetDeathTimer();

	mHideMessageTimer = timerManager->addTimer(18.0, std::bind(&chat_ghost::showMessage, this, false), false);
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

void gl::app::chat_ghost::resetDeathTimer()
{
	timer_manager::get()->resetTimer(mDeathTimer);
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

void gl::app::chat_ghost::getGhostColor(uint8_t& r, uint8_t& g, uint8_t& b) const
{
	uint8_t* rgb = nullptr;

	if (mUserId.empty())
	{
		rgb = gGhostColors[std::rand() % gGhostColors.size()];
	}
	else
	{
		uint64_t hash = std::hash<std::string>{}(mUserId);
		rgb = gGhostColors[hash % gGhostColors.size()];
	}

	r = rgb[0];
	g = rgb[1];
	b = rgb[2];
}
