#pragma once

#include <SDL2/SDL_render.h>

namespace gl::app
{
	class object_component
	{
	public:
		object_component() = default;
		object_component(const object_component&) = delete;
		object_component(object_component&&) = delete;
		virtual ~object_component() = default;

		object_component& operator=(const object_component&) = delete;
		object_component& operator=(object_component&&) = delete;

		virtual void update(double delta) = 0;
		virtual void draw(SDL_Renderer* renderer) = 0;

		void setOwner(class object* owner) { mOwnerObject = owner; }

	protected:
		class object* mOwnerObject{};
	};
} // namespace gl::app