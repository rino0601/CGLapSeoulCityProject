#pragma once

class mSemaphore
{
private:
	bool Lock;

public:
	mSemaphore(void);
	~mSemaphore(void);

	bool isLock();
	bool in();
	void out();
};
