#pragma once

#include "core/actor_component.hxx"
#include "core/object.hxx"

#include <SDL2/SDL_render.h>
#include <memory>
#include <vector>

namespace gl::app
{
	class actor : public object
	{
	public:
		actor() = default;
		virtual ~actor() = default;

		virtual void init();
		virtual void update(double delta);
		virtual void draw(SDL_Renderer* renderer) const;

		void setPos(double x, double y);
		void setPosX(double x);
		void setPosY(double y);

		double getPosX() const { return mX; };
		double getPosY() const { return mY; };
		void getPos(double* x, double* y);

		void setVisible(bool value) { mVisible = value; };
		bool getVisible() { return mVisible; }

		template <typename T, typename... Args>
		T* addComponent(Args&&... args)
		{
			std::unique_ptr<actor_component> newComp = std::unique_ptr<actor_component>(new T(args...));
			newComp->setOwner(this);

			return dynamic_cast<T*>(mOwnedComponents.emplace_back(std::move(newComp)).get());
		}

	protected:
		std::vector<std::unique_ptr<actor_component>> mOwnedComponents;

		double mX, mY{};

		bool mVisible{true};
	};
} // namespace gl::app