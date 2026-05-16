/*
 *  Asteroids.h
 *  Untitled2
 *
 *  Created by Misha K on 9/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <vector>

class Asteroids
{
    std::vector<int> _recentlyAddedObjects;

	Asteroids();
	void LoadEpheFiles(std::string path);
public:

	std::vector<double> Calculate(double jd, int jplID, int& obsID, std::string& errorDescription) const;
    std::vector<int> recentlyAddedObjects() const
    {
        return _recentlyAddedObjects;
    }

	static Asteroids& GetInstance();

	static bool objectIsPresent(int naifId);
	static bool jdisPresent(int naifId, double jd);
	static std::string asteroidName(int naifId);
	static std::vector<int> loadedSpkIds();
};
