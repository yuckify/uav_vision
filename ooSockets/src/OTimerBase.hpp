#ifndef OTimerBase_H
#define OTimerBase_H

#include<functional>

#include<OTime.hpp>

class OTimerBase {
public:
	
	virtual OO::TimerType type() = 0;
	virtual void runLoop() = 0;
	virtual OTime period() = 0;
	
};

#endif // OTimerBase_H
