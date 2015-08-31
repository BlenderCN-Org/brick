#ifndef _EVENT_H_
#define _EVENT_H_

//
// Abstract base class for all events
//
class Event
{
friend class Serial; //feel like I shouldn't have to do this...
public:
	Event();
	~Event();

	void set_duration(const float d);
	float get_duration() const;

	void set_delay(const float d);
	float get_delay() const;

	bool finished() const;

	virtual void init() = 0;
	virtual void simulate(const float t) = 0;
	
protected:
	float delay_time;
	float duration_time;
	float elapsed_time;

	Event *next;
};

#endif // _EVENT_H_
