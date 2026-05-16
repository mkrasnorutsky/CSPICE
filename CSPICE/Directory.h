/*
 *  Directory.h
 *  npapi_layout_test_plugin
 *
 *  Created by Misha Krasnorutsky on 8/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <string>

class Directory
{
public:
	static bool GetEntriesList(const char* dir_name, std::vector<std::string>& filesList);
};