#ifndef __CLOSEABLE_HPP__
#define __CLOSEABLE_HPP__

class Closeable{
public:
	Closeable();
	~Closeable();
	virtual void onClose() = 0;
};
#endif
