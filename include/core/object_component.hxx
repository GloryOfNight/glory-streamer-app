#pragma once

#include <SDL2/SDL_render.h>

namespace gl::app
{
	class object_component
	{
	public:
		object_component() = default;
		virtual ~object_component() = default;

		virtual void update(double delta) = 0;
		virtual void draw(SDL_Renderer* renderer) = 0;

		void setOwner(class object* owner) { mOwnerObject = owner; }

	protected:
		class object* mOwnerObject{};
	};
} // namespace gl::app