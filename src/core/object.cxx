#include "core/object.hxx"

#include <iostream>

gl::app::object::object()
	: bInitialized{false}
{
}

void gl::app::object::init()
{
	bInitialized = true;
}