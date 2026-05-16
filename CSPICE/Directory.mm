/*
 *  Directory.cpp
 *  npapi_layout_test_plugin
 *
 *  Created by Misha Krasnorutsky on 8/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Directory.h"

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

#import <Foundation/Foundation.h>

bool Directory::GetEntriesList(const char* dir_name, std::vector<std::string>& filesList)
{
	filesList.clear();
	
	BOOL isDir = YES;
	
	if (![[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithFormat:@"%s", dir_name] isDirectory:&isDir])
		return false;
	
	NSArray* arr = [[NSFileManager defaultManager]contentsOfDirectoryAtPath:[NSString stringWithFormat:@"%s", dir_name] error:nil];
	
	for (NSString* s in arr)
		filesList.push_back([s UTF8String]);
	
	return true;
	
	DIR* dir;

	if((dir = opendir(dir_name)) == NULL)
	{
		//printf("Error:Directory not found: %s\n", dir_name);
		return false;
	}
	
	struct dirent* dir_ent;
		
	while((dir_ent = readdir(dir)))
	{
		printf ("Direntry %s\n", &dir_ent->d_name[0]);
		filesList.push_back(std::string(dir_ent->d_name));
	}

	closedir(dir);
	
	return true;
}

