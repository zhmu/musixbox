#ifndef __OUTPUT_H__
#define __OUTPUT_H__

class Output {
public:
	virtual int init() { };
	virtual int done() { };
	virtual void play(char* buf, size_t len) = 0;
};

#endif /* __OUTPUT_H__ */
