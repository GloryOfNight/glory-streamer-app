#pragma once

#include <SDL3/SDL_render.h>
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

		virtual void init() { bInitialized = true; }; // kinda for future use, maybe
		virtual void update(double delta) = 0;
		virtual void draw(SDL_Renderer* renderer) = 0;

		bool isInitialized() const { return bInitialized; }

	private:
		bool bInitialized = false;
	};
} // namespace gl::app