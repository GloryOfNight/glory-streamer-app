#include "subsystems/timer_manager.hxx"

static gl::app::timer_manager* gTimerManager = nullptr;

gl::app::timer_manager* gl::app::timer_manager::get()
{
	return gTimerManager;
}

gl::app::timer_manager::~timer_manager()
{
	if (gTimerManager == this)
	{
		gTimerManager = nullptr;
	}
}

void gl::app::timer_manager::init()
{
	subsystem::init();
	gTimerManager = this;
}

void gl::app::timer_manager::update(double delta)
{
	for (auto& timer : mTimers)
	{
		if (!timer.bActive)
			continue;

		timer.remainingSeconds -= delta;
		if (timer.remainingSeconds <= 0.0)
		{
			try
			{
				std::invoke(timer.callback);
			}
			catch (std::bad_function_call badFuncCall)
			{
			}

			if (timer.bInLoop)
				timer.remainingSeconds = timer.seconds;
			else
				timer.bActive = false;
		}
	}

	clearInactive();
}

void gl::app::timer_manager::draw(SDL_Renderer* renderer)
{
}

gl::app::timer_handle gl::app::timer_manager::addTimer(double seconds, timer_callback callback, bool bLoop)
{
	++mHandleCount;

	timer newTimer{};
	newTimer.handle = mHandleCount;
	newTimer.seconds = seconds;
	newTimer.remainingSeconds = seconds;
	newTimer.callback = callback;
	newTimer.bInLoop = bLoop;
	newTimer.bActive = true;

	return mTimers.emplace_back(std::move(newTimer)).handle;
}

bool gl::app::timer_manager::resetTimer(timer_handle handle)
{
	if (handle == timer_handle())
		return false;

	auto iter = std::find_if(mTimers.begin(), mTimers.end(), [handle](const timer& timer)
		{ return timer.handle == handle; });

	const bool bFind = iter != mTimers.end();
	if (bFind)
	{
		iter->remainingSeconds = iter->seconds;
	}
	return bFind;
}

bool gl::app::timer_manager::clearTimer(timer_handle handle)
{
	if (handle == timer_handle())
		return false;

	auto iter = std::find_if(mTimers.begin(), mTimers.end(), [handle](const timer& timer)
		{ return timer.handle == handle; });

	const bool bFind = iter != mTimers.end();
	if (bFind)
	{
		iter->bActive = false;
	}
	return bFind;
}

void gl::app::timer_manager::clearInactive()
{
	const auto removeIf = [](const timer& timer)
	{ return !timer.bActive; };

	mTimers.erase(std::remove_if(mTimers.begin(), mTimers.end(), removeIf), mTimers.end());
}
