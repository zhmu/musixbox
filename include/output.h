#ifndef __OUTPUT_H__
#define __OUTPUT_H__

class Output {
public:
	virtual int init() = 0;
	virtual int done() = 0;
	virtual void play(char* buf, size_t len) = 0;
};

#endif /* __OUTPUT_H__ */
