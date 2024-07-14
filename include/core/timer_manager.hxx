#pragma once
#include <functional>
#include <vector>

namespace gl::app
{
	using timer_handle = uint32_t;
	using timer_callback = std::function<void()>;

	class timer_manager
	{
	public:
		timer_manager();
		~timer_manager() = default;

		void update(double delta);

		timer_handle addTimer(double seconds, timer_callback callback, bool bLoop = false);

		bool resetTimer(timer_handle handle);

		bool clearTimer(timer_handle handle);

	private:
		void clearInactive();

		struct timer
		{
			uint32_t handle{};
			double seconds{};
			double remainingSeconds{};
			timer_callback callback{};
			bool bInLoop{};
			bool bActive{};
		};

		std::vector<timer> mTimers{};

		uint32_t mHandleCount{};
	};
} // namespace gl::app