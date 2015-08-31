#include "create-force.h"
#include "../math/math-utility.h"

CreateForce::CreateForce() : Event()
{

}

CreateForce::~CreateForce() {}

void CreateForce::init()
{

}

void CreateForce::simulate(const float t)
{
	float pct = 0.0f;
	float F = Math::cerp(force_a, force_b, pct);
}

void CreateForce::set_range(const float a, const float b)
{
	force_a = a;
	force_b = b;
}

void CreateForce::get_range(float &a, float &b)
{
	a = force_a;
	b = force_b;
}