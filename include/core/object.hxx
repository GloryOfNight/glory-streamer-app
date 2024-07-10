#pragma once

#include <SDL2/SDL_render.h>

namespace gl::app
{
	class object
	{
	public:
		object() = default;
		virtual ~object() = default;

		virtual void init() = 0;
		virtual void update(double delta) = 0;
		virtual void draw(SDL_Renderer* renderer) = 0;
	};
} // namespace gl::app