#include "core/timer_manager.hxx"

gl::app::timer_manager::timer_manager()
{
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
			timer.callback();
			if (timer.bInLoop)
				timer.remainingSeconds = timer.seconds;
			else
				timer.bActive = false;
		}
	}

	clearInactive();
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

bool gl::app::timer_manager::clearTimer(timer_handle handle)
{
	auto iter = std::find_if(mTimers.begin(), mTimers.end(), [handle](const timer& timer)
		{ return timer.handle == handle; });

	if (iter != mTimers.end())
	{
		iter->bActive = false;
		return true;
	}

	return false;
}

void gl::app::timer_manager::clearInactive()
{
	const auto removeIf = [](const timer& timer)
	{ return !timer.bActive; };

	mTimers.erase(std::remove_if(mTimers.begin(), mTimers.end(), removeIf), mTimers.end());
}
