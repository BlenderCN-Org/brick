#include <stdlib.h>
#include "event.h"

Event::Event()
{
	delay_time =		0.0f;
	duration_time = 	1.0f;
	elapsed_time =		0.0f;

	next = NULL;
}

Event::~Event()
{

}

void Event::set_duration(const float d)
{
	duration_time = d;
}

float Event::get_duration() const
{
	return duration_time;
}

void Event::set_delay(const float d)
{
	delay_time = d;
}

float Event::get_delay() const
{
	return delay_time;
}

bool Event::finished() const
{
	return elapsed_time > (delay_time + duration_time);
}