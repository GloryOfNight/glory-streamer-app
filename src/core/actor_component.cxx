#include "core/actor_component.hxx"

#include "core/actor.hxx"

void gl::app::actor_component::init()
{
}

void gl::app::actor_component::update(double delta)
{
}

void gl::app::actor_component::draw(SDL_Renderer* renderer)
{
}

void gl::app::actor_component::setPos(double x, double y)
{
	mX = x;
	mY = y;
}

void gl::app::actor_component::setOwner(const actor* owner)
{
	mOwner = owner;
}

double gl::app::actor_component::getWorldPosX() const
{
	return mOwner->getPosX() + mX;
}

double gl::app::actor_component::getWorldPosY() const
{
	return mOwner->getPosY() + mY;
}
