#ifndef _CREATE_FORCE_H_
#define _CREATE_FORCE_H_

#include "event.h"

class CreateForce : public Event
{
	public:
		CreateForce();
		~CreateForce();

		void init();
		void simulate(const float t);

		void set_range(const float fa, const float fb);
		void get_range(float &fa, float &fb);
		
	private:
		//probably isn't how to do this for real
		//(use a function class)
		float force_a;
		float force_b;
};

#endif