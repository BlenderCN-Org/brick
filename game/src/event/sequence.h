#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include "event.h"

class Sequence
{
public:
	Sequence();
	~Sequence();

	void init();
	void simulate(const float t);

	void start();
	void pause(bool p);
	void kill();

	bool is_paused();
private:
	bool paused;
	Event *root;
};

#endif // _SEQUENCE_H_
