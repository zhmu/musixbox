#include <exception>
#include <string>

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

//! \brief Base exception class
class MusixBoxException : public std::exception {
public:
	/*! \brief Construct an exception
	 *  \param errstr Message describing the exception
	 */
	MusixBoxException(const std::string errstr) {
		message = errstr;
	}

	//! \brief Destruct an exception
	virtual ~MusixBoxException() throw() { }

	//! \brief Obtain the exception message
	virtual const char* what() const throw() { return message.c_str(); }

private:
	//! \brief Exception message
	std::string message;
};

//! \brief Exception class used by decoder providers
class DecoderException : public MusixBoxException {
public:
	DecoderException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by mixer providers
class MixerException : public MusixBoxException {
public:
	MixerException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by interaction providers
class InteractionException : public MusixBoxException {
public:
	InteractionException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by input providers
class InputException : public MusixBoxException {
public:
	InputException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by output providers
class OutputException : public MusixBoxException {
public:
	OutputException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by visualization providers
class VisualizeException : public MusixBoxException {
public:
	VisualizeException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by folder providers
class FolderException : public MusixBoxException {
public:
	FolderException(const std::string errstr) : MusixBoxException(errstr) { }
};

//! \brief Exception class used by factories
class FactoryException : public MusixBoxException {
public:
	FactoryException(const std::string errstr) : MusixBoxException(errstr) { }
};

#endif /* __EXCEPTIONS_H__ */
