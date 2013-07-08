
#import "msemaphore.h"

mSemaphore::mSemaphore(void)
{
	Lock = false;
}

mSemaphore::~mSemaphore(void)
{
}

bool mSemaphore::isLock(){
	return Lock;
}

bool mSemaphore::in(){
	if(Lock)
		return false;
	Lock = true;
	return true;
}

void mSemaphore::out(){
	Lock = false;
}