/*
 *  Directory.cpp
 *  CSPICE
 */

#include "Directory.h"

#include <cstring>
#include <dirent.h>

bool Directory::GetEntriesList(const char* dir_name, std::vector<std::string>& filesList)
{
    filesList.clear();

    if (dir_name == nullptr || dir_name[0] == '\0')
    {
        return false;
    }

    DIR* dir = opendir(dir_name);
    if (dir == nullptr)
    {
        return false;
    }

    while (dirent* entry = readdir(dir))
    {
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        filesList.emplace_back(entry->d_name);
    }

    closedir(dir);
    return true;
}
