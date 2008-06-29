#ifndef __INPUT_H__
#define __INPUT_H__

class Input {
public:
	virtual size_t read(char* buffer, size_t len) = 0;
};

#endif /* __INPUT_H__ */
