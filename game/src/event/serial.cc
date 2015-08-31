#include <stdlib.h>
#include "serial.h"

Serial::Serial() : Event()
{
	children = NULL;
}

Serial::~Serial() {}

void Serial::init() {}

void Serial::simulate(const float t)
{
	//only simulate the children one at a time, in order
	Event *e = children;
	while(e != NULL) {
		if(e->finished()) {
			e = e->next;
		} else {
			e->simulate(t);
			return;
		}
	}
}

void Serial::add_child(Event *child)
{
	Event *ep = children;
	if(children == NULL) {
		children = child;
	} else {
		while(ep->next != NULL) {
			ep = ep->next;
		}
		ep->next = child;
	}
}