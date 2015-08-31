#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "object.h"

class Container2D : public Object2D
{
	public:
		Container2D();
		~Container2D();

		void init();
		void simulate(const float t);

		void add_thing(Thing *t);
	private:
		int num_children;
		Thing *children;
};

#endif //_CONTAINER_H_