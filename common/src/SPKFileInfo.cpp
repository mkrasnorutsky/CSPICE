/*
 *  SPKFileInfo.cpp
 *  Untitled2
 *
 *  Created by Misha K on 9/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "SpiceUsr.h"
#include "SpiceZfc.h"
#include "SpiceZmc.h"

#include "SPKFileInfo.h"

#include <stdio.h>
#include <sstream>

static double leapsecondsToJD(double ls)
{
	return (ls / (double)(spd_c())) + 2451545.5;
}

static double JDtoLeapseconds(double jd)
{
	return (jd - 2451545.5) * spd_c();
}

#define SpiceInt int

#define ASTEROID_MAX_NAME_LENGTH 30

using namespace std;

extern "C" /* Subroutine */ int cidfrm_(integer *cent, integer *frcode, char *frname, 
							 logical *found, ftnlen frname_len);

void myFunction()
{
#define  FILLEN   128
#define  TYPLEN   32
#define  SRCLEN   128
	
	SpiceInt        which;
	SpiceInt        handle;
	
	SpiceChar       file  [FILLEN];
	SpiceChar       filtyp[TYPLEN];
	SpiceChar       source[SRCLEN];
	
	SpiceBoolean    found;
	
	SpiceInt count = 0;
#undef SpiceInt
	ktotal_c ( "spk", (SpiceInt*)&count );

	if ( count == 0 )
	{
		printf ( "No SPK files loaded at this time.\n" );
	}
	else
	{
		printf ( "The loaded SPK files are: \n\n" );
	}
	
	for ( which = 0;  which < count;  which++ )
	{
		kdata_c ( which,  "spk",    FILLEN,   TYPLEN, SRCLEN,
				 file,  (SpiceChar*) filtyp,  source, (SpiceInt*)&handle,  &found );
#define SpiceInt int
		printf ( "%s\n",  file   );
	}
	
}

static string GetAsteroidNameById(int id)
{	
	std::string asteroidName;
	SpiceChar nm[ASTEROID_MAX_NAME_LENGTH];
	SpiceBoolean nameIsPresent = 1;	

	bodc2n_c (id, sizeof(nm), nm, &nameIsPresent);
	//bodc2s_c (id, sizeof(nm), nm/*, &nameIsPresent*/);
/*
	SpiceInt dim      = 3;
	
	SpiceDouble val[3];
	
	bodvcd_c(id, "CENTER", dim, &dim, val);
*/
	/*SpiceInt frcode, trgcde = 2000001;
	logical found;
	char str[80];
	cidfrm_(&trgcde, &frcode, str, &found, 80);
	
	SpiceInt cent = 0;
	SpiceInt      frclss;
	SpiceInt      clssid;
	SpiceBoolean  isFound;
	frinfo_c(frcode, &cent, &frclss, &clssid, &isFound);
	
	myFunction();
	*/
	if (nameIsPresent)
		asteroidName = nm;
	else
	{
		if (id == 2000002)
			return "PALLAS";
		
		stringstream str;
		
		str<<id;
		
		asteroidName = str.str();
	}
	
	return asteroidName;
}

F_Info::F_Info(const char* fname, double bg, double en):jdBegin(bg), jdEnd(en), filename(fname){};

bool F_Info::Within(double jd)
{
	return jdBegin<=jd && jd<=jdEnd;
}

Segments::Segments(std::string astName):asteroidName(astName){}

void Segments::Add(const char* fname, double bg, double en)
{
	data.push_back(F_Info(fname, bg, en));
}

bool Segments::JDisPresent(double jd)
{
	for (int i=0; i<data.size(); ++i)
		if (data[i].Within(jd))
			return true;
	
	return false;
}

std::string Segments::GetAsteroidName()
{
	return asteroidName;
}
		
void SPKFiles::Add(int ids, const char* filename, double jdBegin, double jdEnd)
{
	SFmapI i = objects.find(ids);
	
	if (i == objects.end())
	{
		objects.insert(make_pair(ids, Segments(GetAsteroidNameById(ids))));
		i = objects.find(ids);
	}
				   
	i->second.Add(filename, jdBegin, jdEnd);
}

extern "C" void callbackFunc (void*pt, int obj, const char* filename, double xfrom, double xto)
{
	((SPKFiles*)pt)->Add(obj, filename, leapsecondsToJD(xfrom), leapsecondsToJD(xto));
}

extern "C" void summarizeLoadedData(const char* filename, void* owner, void(*callb)(void*,int,const char*,double,double));

void SPKFiles::Load(const char* filename)
{
	summarizeLoadedData(filename, this, callbackFunc);
}
				   
vector<int> SPKFiles::GetIds()
{	
	vector<int> ids;
				   
	for (SFmapI i=objects.begin(); i!=objects.end(); ++i)
		ids.push_back(i->first);
	
	return ids;
}

string SPKFiles::GetAsteroidName(int ids)
{
	SFmapI i = objects.find(ids);
	
	if (i != objects.end())
    {
		string asteroidName = GetAsteroidNameById(ids);//;i->second.GetAsteroidName();
        if (!count_if(asteroidName.begin(),
                      asteroidName.end(),
                      ::isalpha))
        {
            if (names.find(ids) != names.end())
            {
                return names[ids];
            }
        }
        
        if (asteroidName.empty())
        {
            return asteroidName;
        }
        else if (asteroidName.size() == 1)
        {
            transform(asteroidName.begin(),
                      asteroidName.end(),
                      asteroidName.begin(),
                      ::toupper);
            
            return asteroidName;
        }
        
        transform(asteroidName.begin(),
                  asteroidName.begin()+1,
                  asteroidName.begin(),
                  ::toupper);
        transform(asteroidName.begin()+1,
                  asteroidName.end(),
                  asteroidName.begin()+1,
                  ::tolower);
        
        return asteroidName;
    }
	else
		return "";
}
				   
bool SPKFiles::ObjectIsPresent(int ids)
{
	SFmapI i = objects.find(ids);
				   
	return i != objects.end();
}

bool SPKFiles::JDisPresent(int ids, double jd)
{
	SFmapI i = objects.find(ids);
	
	if ( i != objects.end())
		return i->second.JDisPresent(jd);
	else
		return false;
}

SPKFiles& SPKFiles::GetInstance()
{
	static SPKFiles spkF_Info;
	
	return spkF_Info;
}
