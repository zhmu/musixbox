#ifndef __VISUALIZE_H__
#define __VISUALIZE_H__

class Visualizer {
public:
	virtual int init() { };
	virtual void update(const char* input, unsigned int num) = 0;
	virtual void done() { };
};

#endif /* __VISUALIZE_H__ */
