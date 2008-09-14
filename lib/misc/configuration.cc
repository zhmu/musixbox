#include <map>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "misc/configuration.h"

using namespace std;

Configuration::Configuration(string fname)
{
	this->fname = fname;
	retrieve();
}

string
Configuration::getString(const string key, const string def)
{
	map<string, string>::iterator it = config.find(key);
	if (it == config.end())
		return def;

	return it->second;
}

void
Configuration::setString(const string key, const string val)
{
	config[key] = val;
}

unsigned int
Configuration::getInteger(const std::string key, unsigned int def)
{
	string str = getString(key, "*");
	if (str == "*")
		return def;

	return (unsigned int)strtol(str.c_str(), NULL, 10);
}

void
Configuration::setInteger(const std::string key, unsigned int val)
{
	stringstream ss;

	ss << val;
	setString(key, ss.str());
}

bool
Configuration::retrieve()
{
	ifstream f;
	string s;

	f.open(fname.c_str());
	if (!f.is_open())
		return false;

	while (!f.eof()) {
		getline(f, s);

		/* Ignore any line that does not contain a '=' seperator */
		size_t pos = s.find_first_of("=");
		if (pos == string::npos)
			continue;

		/* Split string in key=val pairs and store them */
		string key = string(s.begin(), s.begin() + pos);
		string val = string(s.begin() + pos + 1, s.end());
		setString(key, val);

	}
	f.close();
	return true;
}

bool
Configuration::store()
{
	ofstream f;

	f.open(fname.c_str());
	if (!f.is_open())
		return false;

	for (map<string, string>::iterator it = config.begin(); it != config.end(); it++) {
		f << it->first << "=" << it->second << endl;
	}
	f.close();
	return true;
}

void
Configuration::clear()
{
	config.clear();
}
