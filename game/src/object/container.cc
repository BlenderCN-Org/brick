#include <stdlib.h>
#include "container.h"

Container2D::Container2D() : Object2D()
{
	num_children = 0;
	children = NULL;
}

Container2D::~Container2D() {}

void Container2D::init()
{

}

void Container2D::simulate(const float t)
{
}