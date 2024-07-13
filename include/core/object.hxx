#pragma once

#include "object_component.hxx"

#include <SDL2/SDL_render.h>
#include <memory>
#include <vector>

namespace gl::app
{
	class object
	{
	public:
		object() = default;
		object(const object&) = delete;
		object(object&&) = delete;
		virtual ~object() = default;

		object& operator=(const object&) = delete;
		object& operator=(object&&) = delete;

		virtual void init() = 0;
		virtual void update(double delta) = 0;
		virtual void draw(SDL_Renderer* renderer) = 0;

		template <typename T, typename... Args>
		T* addComponent(Args&&... args)
		{
			std::unique_ptr<object_component> newComp = std::unique_ptr<object_component>(new T(args...));
			newComp->setOwner(this);

			return dynamic_cast<T*>(mOwnedComponents.emplace_back(std::move(newComp)).get());
		}

	private:
		std::vector<std::unique_ptr<object_component>> mOwnedComponents{};
	};
} // namespace gl::app