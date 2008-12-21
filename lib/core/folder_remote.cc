#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "exceptions.h"
#include "folder_remote.h"
#include "input_remote.h"

using namespace std;

FolderRemote::FolderRemote(string r) :
	Folder(r)
{
	/* Skip over blanks in the XML file, we don't care about them */
	xmlKeepBlanksDefault(0);
	previous = r;

	/*
	 * Every derivative class has to call the rehash() function here - refer
	 * to folder.cc for an explanation why this is.
	 */
	rehash();
}

FolderRemote::~FolderRemote()
{
	clearInfo();
}

void
FolderRemote::retrieveEntries()
{
	std::string c;
	xmlDocPtr doc;
	xmlNodePtr cur;

	/*
	 * Ditch the current information; it will only take memory and we no
	 * longer need it.
	 */
	clearInfo();

	InputRemote::fetchURLtoString(current, c);
	doc = xmlReadMemory(c.c_str(), c.size(), NULL, NULL, 0);
	if (doc == NULL)
		throw FolderException(string("FolderRemote: cannot parse XML"));

	/*
	 * Walk through the XML structure - if we got here, we know it's a valid
	 * file so we can walk through it node-by-one.
	 *
	 * We are quite lenient while parsing the XML file:
	 *
	 * - Anything below the root node that has an 'url' attribute is
	 *   expected to be a folder entry. The name is the node's content.
	 * 
	 * The entry is assumed to be a folder if any of the following
	 * conditions hold:
	 *
	 * - An entry that a 'folder' attribute is assumed to be a folder.
	 * - The last charachter of the entry name is a '/'
	 */
	cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		const char* name = (const char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		const char* url = (const char*)xmlGetProp(cur, (const xmlChar*)"url");
		bool folder = false;
		if (xmlGetProp(cur, (const xmlChar*)"folder") != NULL) {
			/* There is a 'folder' attribute */
			folder = true;
		} else if (name != NULL) {
			char* ptr = strrchr(name, '/');
			if (ptr != NULL && ptr[1] == '\0') {
				/* The last charachter of the name is a / */
				folder = true;
			}
		}
		if (name != NULL && link != NULL) {
			entries.push_back(name);
			entry2info[name] = new RemoteFolderInfo(url, folder);
		}
		cur = cur->next;
	}

	/* If this is not the root folder, surgically insert a '..' location */
	if (current != root)
		entries.push_back("..");

	xmlFreeDoc(doc);
}

bool
FolderRemote::isFolder(string entry)
{
	return entry2info[entry]->isFolder();
}

void
FolderRemote::select(std::string dir)
{
	current = entry2info[dir]->getLocation();
	rehash();
}

void
FolderRemote::goUp()
{
	current = previous;
	rehash();
}

string
FolderRemote::getFullPath(string entry)
{
	return entry2info[entry]->getLocation();
}

void
FolderRemote::clearInfo()
{
	for (std::map<std::string, RemoteFolderInfo*>::const_iterator it = entry2info.begin(); it != entry2info.end(); ++it) {
		delete (*it).second;
	}
	entry2info.clear();
}
