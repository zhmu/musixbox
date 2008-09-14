#include <map>

//! \brief Maintains configuration settings
class Configuration {
public:
	/*! \brief Initialize configuration
	 *  \param fname File used to store / load the configuration
	 */
	Configuration(std::string fname);

	/*! \brief Retrieve a string value
	 *  \param key Key to use
	 *  \param def Default value
	 */
	std::string getString(const std::string key, const std::string def = "");

	/*! \brief Retrieve an integer value
	 *  \param key Key to use
	 *  \param def Default value
	 */
	unsigned int getInteger(const std::string key, unsigned int def = 0);

	/*! \brief Store a string value
	 *  \param key Key to use
	 *  \param val Value to store
	 */
	void setString(const std::string key, const std::string val);

	/*! \brief Store an integer value
	 *  \param key Key to use
	 *  \param val Value to store
	 */
	void setInteger(const std::string key, unsigned int val);

	/*! \brief Retrieve configuration from disk
	 *  \return true on success
	 */
	bool retrieve();

	/*! \brief Store configuration on disk
	 *  \return true on success
	 */
	bool store();

	//! \brief Remove all configuration settings
	void clear();

private:
	//! \brief Filename of the configuration file
	std::string fname;

	//! \brief Actual configuration
	std::map<std::string, std::string> config;
};
