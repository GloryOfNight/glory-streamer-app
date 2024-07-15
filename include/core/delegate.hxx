#pragma once

#include <functional>
#include <vector>

namespace gl::app
{
	template <typename... Args>
	struct delegate
	{
		using type = std::function<void(Args...)>;

		void bind(type func)
		{
			functions.push_back(func);
		}

		void execute(Args&&... args)
		{
			for (const auto& func : functions)
			{
				std::invoke(func, std::forward<Args>(args)...);
			}
		}

	public:
		std::vector<type> functions{};
	};
} // namespace gl::app