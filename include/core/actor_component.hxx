#pragma once

#include "object.hxx"

namespace gl::app
{
	class actor_component : public object
	{
		friend class actor;

	public:
		actor_component() = default;
		virtual ~actor_component() = default;

		virtual void init() override;
		virtual void update(double delta) override;
		virtual void draw(SDL_Renderer* renderer) override;

		void setPosX(double x) { mX = x; }
		void setPosY(double y) { mY = y; }
		void setPos(double x, double y);

		// @return relative position to the owner
		double getPosX() const { return mX; }

		// @return relative position to the owner
		double getPosY() const { return mY; };

		// @return world position
		double getWorldPosX() const;

		// @return world position
		double getWorldPosY() const;

		void setVisible(bool value) { mVisible = value; }
		bool getVisible() const { return mVisible; }

	protected:
		void setOwner(const class actor* owner);

		double mX{}, mY{};

		const class actor* mOwner;

		bool mVisible{ true };
	};
} // namespace gl::app
