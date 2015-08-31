#include <stdlib.h>
#include "sequence.h"

Sequence::Sequence()
{
	root = NULL;
}

Sequence::~Sequence()
{
	if(root) {
		delete root;
	}
}

void Sequence::init() {}

void Sequence::simulate(const float t)
{
	if(root) {
		root->simulate(t);
	}
}

void Sequence::start() {}

void Sequence::pause(bool p) 
{
	paused = p;
}

bool Sequence::is_paused()
{
	return paused;
}

void Sequence::kill() {}