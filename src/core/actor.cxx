#include "core/actor.hxx"

#include "core/engine.hxx"

void gl::app::actor::init()
{
	object::init();
	for (auto& comp : mOwnedComponents)
		comp->init();
}

void gl::app::actor::update(double delta)
{
	for (auto& comp : mOwnedComponents)
		comp->update(delta);
}

void gl::app::actor::draw(SDL_Renderer* renderer) const
{
	if (!mVisible)
		return;

	for (auto& comp : mOwnedComponents)
	{
		if (comp->getVisible())
			comp->draw(renderer);
	}
}

void gl::app::actor::destroy()
{
	engine::get()->removeObject(this);
}

void gl::app::actor::setPos(double x, double y)
{
	mX = x;
	mY = y;
}

void gl::app::actor::getPos(double* x, double* y)
{
	if (x)
		*x = mX;

	if (y)
		*y = mY;
}

void gl::app::actor::setPosX(double x)
{
	mX = x;
}

void gl::app::actor::setPosY(double y)
{
	mY = y;
}
