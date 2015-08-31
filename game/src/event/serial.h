#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "event.h"

class Serial : public Event {
	public:
		Serial();
		~Serial();

		void init();
		void simulate(const float t);

		void add_child(Event *e);
	private:
		Event *children;
};

#endif
