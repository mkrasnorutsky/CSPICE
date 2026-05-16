/*
 *  Asteroids.cpp
 *  Untitled2
 *
 *  Created by Misha K on 9/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Asteroids.h"

#include <stdio.h>
#include <cstring>
#include <sstream>
#include <map>
#include "SpiceUsr.h"
#include "SPKFileInfo.h"
#include "Directory.h"

#import "SpiceZdf.h"
#import "SpiceZfc.h"
#import "SpiceUsr.h"

#define NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT 2000000
#define NAIF_INTEGER_ID_CODES_COMETS_SHIFT 1000000

#import <Foundation/Foundation.h>

using namespace std;

class Asteroids
{
    vector<int> _recentlyAddedObjects;

    Asteroids();
    void LoadEpheFiles(string path);

public:
    vector<double> Calculate(double jd, int jplID, int &obsID, string &errorDescription) const;
    vector<int> recentlyAddedObjects() const { return _recentlyAddedObjects; }

    static Asteroids &GetInstance();
    void loadEpheFiles(string path) { LoadEpheFiles(path); }
    static bool objectIsPresent(int naifId);
    static bool jdisPresent(int naifId, double jd);
    static string asteroidName(int naifId);
    static vector<int> loadedSpkIds();
};

static NSString* documentsPath()
{
#if TARGET_OS_IPHONE
    NSString* path = [NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex:0];
    

#else
    
//Sandboxed
#  if 1
    NSString* path = NSHomeDirectory();
#  else
    NSString* path = [NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex:0];
    
    path = [path stringByAppendingPathComponent:PATH_TO_DOCUMENTS];
#  endif
    
#endif
    
    return path;
}


const char* SPICEFilesFolder()
{
	return [documentsPath() UTF8String];
}

int display_info(char* spk);

void planet_pv(int jplID, double jd, double* state)
{
	double lt;
	double et = (jd - 2451545.5/*2451545.5*/) * spd_c();
	
	spkez_c(jplID, et, "J2000", "NONE", 0, state, &lt );
}

void printids(int ids)
{	
	std::string name = SPKFiles::GetInstance().GetAsteroidName(ids);
	
	printf("Loaded Asteroid id = %d , %s;\n", ids, name.c_str());
}

Asteroids::Asteroids()
{
	erract_c("SET", sizeof("RETURN") + 1, "RETURN");
    
    map<int,string> asteroidNames;
    NSString* resourceName = [[NSBundle bundleWithIdentifier:@"co.krasnorutsky.CSPICE"]pathForResource:@"AsteroidNames" ofType:@"txt"];
    if (resourceName)
    {
        NSString* astNames = [[NSString alloc]initWithContentsOfFile:resourceName encoding:NSUTF8StringEncoding error:nil];
        if (astNames)
        {
            NSArray* list = [astNames componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]];
            
            for (NSString* s in list)
            {
                NSArray* components = [s componentsSeparatedByString:@"\t"];
                if (components.count == 2)
                {
                    asteroidNames[NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT + [components.firstObject intValue]] = [components.lastObject UTF8String];
                }
            }
        }
    }
    
    SPKFiles::GetInstance().setNames(asteroidNames);
	
    NSString* idsListFileName = [documentsPath() stringByAppendingPathComponent:@"Asteroids.json"];
    NSData* d = [NSData dataWithContentsOfFile:idsListFileName];
    NSArray* previousObjectIds = nil;
    if (d)
    {
        previousObjectIds = [[NSJSONSerialization JSONObjectWithData:d options:0 error:nil]objectForKey:@"objects"];
    }
    
	LoadEpheFiles(SPICEFilesFolder());
	
	vector<int> ids = SPKFiles::GetInstance().GetIds();
	for_each(ids.begin(), ids.end(), printids);
    
    if (previousObjectIds)
    {
        for (auto i : ids)
        {
            if (![previousObjectIds containsObject:@(i)])
            {
                _recentlyAddedObjects.push_back(i);
            }
        }
    }
    
    NSMutableArray* objects = [NSMutableArray new];
    for (auto i : ids)
    {
        [objects addObject:@(i)];
    }
    
    NSData* d_toSave = [NSJSONSerialization dataWithJSONObject:@{@"objects" : objects} options:NSJSONWritingPrettyPrinted error:nil];
    [d_toSave writeToFile:idsListFileName atomically:YES];
}

void Asteroids::LoadEpheFiles(string path)
{
	vector<string> lst;
	
	if (!Directory::GetEntriesList(path.c_str(), lst))
		return;
	
	for (int i=0; i<lst.size(); ++i)
	{
		{
			string currentFile = lst[i];
		
			if (currentFile.size()<3)
				continue;
			
			string ext2 = currentFile.substr(currentFile.size()-3, 3);
			
			string ext3 = currentFile.size()>=4 ?  currentFile.substr(currentFile.size()-4, 4) : "";
			
			if (ext2 != ".bc" && // ".bc", ".bee", ".bpc", ".bsp"
				ext3 != ".bee" &&
				ext3 != ".bpc" &&
				ext3 != ".bsp")
				continue;
		}
		
		string targetFile = path  + "/" + lst[i];
		
		reset_c();
		
		furnsh_c(targetFile.c_str());
		
		if (!failed_c())
			SPKFiles::GetInstance().Load(targetFile.c_str());
	};
	
    for (auto i : SPKFiles::GetInstance().GetIds())
    {
        try
        {
            auto idt = stoi(SPKFiles::GetInstance().GetAsteroidName(i));
            if (idt >= NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT &&
                idt < (NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT + 1000000))
            {
                NSLog(@"Unnamed asteroid: %d", idt);
            }
        }
        catch (const std::invalid_argument& ia)
        {
        }
        catch (const std::out_of_range& oor)
        {
        }
        catch (const std::exception& e)
        {
        }
    }
    
	for (int i=0; i<lst.size(); ++i)
	{
		string currentFile = lst[i];
		
		{
			if (currentFile.size()<3)
				continue;
			
			string ext2 = currentFile.substr(currentFile.size()-3, 3);
			
			string ext3 = currentFile.size()>=4 ?  currentFile.substr(currentFile.size()-4, 4) : "";
			
			if (ext2 != ".tf")
				continue;
		}
		
		string targetFile = path  + "/" + lst[i];
		
		reset_c();
		
		furnsh_c(targetFile.c_str());
		
		//if (!failed_c())
		{
		}
	};
	
	reset_c();
}

static SpiceInt getOriginalObserver(double et, int objectId, string& errorDescription)
{
	logical found = 0;
	doublereal descr[5];
	char ident[40];
	integer handle, iBody = objectId;
	doublereal drET = et;
	
	spksfs_(&iBody, &drET, &handle, descr, ident, &found, (ftnlen)40);
	
	if (found)
	{
		SpiceInt           body = objectId;
		SpiceInt           center;
		SpiceInt           frame;
		SpiceInt           type;
		SpiceDouble        first;
		SpiceDouble        last;
		SpiceInt           begin;
		SpiceInt           end;
		spkuds_c (descr,  &body, &center, &frame, &type,  &first, &last,  &begin, &end);
		
		return center;
	}
	else
	{
		string name = SPKFiles::GetInstance().GetAsteroidName(objectId);
		
		errorDescription = string("Center of object ") + name + " is unavailiable.";
        
        return 0;
	}
}

vector<double> Asteroids::Calculate(double jd, int jplID, int& obsID, string& errorDescription) const
{
    errorDescription.clear();
    
	double lt = 0.0;
	double et = (jd - 2451545.0/*2451545.0*/) * spd_c();
	
    vector<double> result(6, 0.0);
	obsID = getOriginalObserver(et, jplID, errorDescription);
    if (!errorDescription.empty())
    {
        return result;
    }
	
	reset_c();
	
	spkez_c(jplID, et, "J2000", "NONE", obsID, result.data(), &lt);
	
	if (failed_c())
	{
		string name = SPKFiles::GetInstance().GetAsteroidName(jplID);
		
        errorDescription = string("Object ") + name + " is unavailiable.";
	}

	return result;
}

Asteroids& Asteroids::GetInstance()
{
	static Asteroids ast;
	
	return ast;
}

bool Asteroids::objectIsPresent(int naifId)
{
	GetInstance();
	return SPKFiles::GetInstance().ObjectIsPresent(naifId);
}

bool Asteroids::jdisPresent(int naifId, double jd)
{
	GetInstance();
	return SPKFiles::GetInstance().JDisPresent(naifId, jd);
}

std::string Asteroids::asteroidName(int naifId)
{
	GetInstance();
	return SPKFiles::GetInstance().GetAsteroidName(naifId);
}

std::vector<int> Asteroids::loadedSpkIds()
{
	GetInstance();
	return SPKFiles::GetInstance().GetIds();
}

static thread_local string g_lastAsteroidName;
static thread_local int g_lastObserverId = 0;

extern "C" {

bool asteroids_load_ephe_files(const char *path)
{
	if (!path)
		return false;

	Asteroids::GetInstance().loadEpheFiles(path);
	return true;
}

AsteroidResult asteroids_calculate(double jd, int jpl_id, int obs_id,
                                   char *error_buffer, int error_buffer_size)
{
	AsteroidResult r = {0};

	if (error_buffer && error_buffer_size > 0)
		error_buffer[0] = '\0';

	int observer = obs_id;
	string errorDescription;
	vector<double> result = Asteroids::GetInstance().Calculate(jd, jpl_id, observer, errorDescription);
	g_lastObserverId = observer;

	if (result.size() >= 6)
	{
		r.pos_x = result[0];
		r.pos_y = result[1];
		r.pos_z = result[2];
		r.vel_x = result[3];
		r.vel_y = result[4];
		r.vel_z = result[5];
	}

	r.valid = errorDescription.empty();

	if (error_buffer && error_buffer_size > 0 && !errorDescription.empty())
	{
		strncpy(error_buffer, errorDescription.c_str(), error_buffer_size - 1);
		error_buffer[error_buffer_size - 1] = '\0';
	}

	return r;
}

bool asteroids_object_is_present(int naif_id)
{
	return Asteroids::objectIsPresent(naif_id);
}

bool asteroids_jd_is_present(int naif_id, double jd)
{
	return Asteroids::jdisPresent(naif_id, jd);
}

const char *asteroids_asteroid_name(int naif_id)
{
	g_lastAsteroidName = Asteroids::asteroidName(naif_id);
	return g_lastAsteroidName.c_str();
}

int asteroids_last_observer_id(void)
{
	return g_lastObserverId;
}

int asteroids_loaded_spk_id_count(void)
{
	return (int)Asteroids::loadedSpkIds().size();
}

int asteroids_copy_loaded_spk_ids(int *ids, int max_count)
{
	vector<int> loaded = Asteroids::loadedSpkIds();
	int count = (int)loaded.size();
	if (!ids || max_count <= 0)
		return count;

	int toCopy = count < max_count ? count : max_count;
	for (int i = 0; i < toCopy; ++i)
		ids[i] = loaded[i];

	return count;
}

int asteroids_recently_added_count(void)
{
	return (int)Asteroids::GetInstance().recentlyAddedObjects().size();
}

int asteroids_copy_recently_added_ids(int *ids, int max_count)
{
	vector<int> recent = Asteroids::GetInstance().recentlyAddedObjects();
	int count = (int)recent.size();
	if (!ids || max_count <= 0)
		return count;

	int toCopy = count < max_count ? count : max_count;
	for (int i = 0; i < toCopy; ++i)
		ids[i] = recent[i];

	return count;
}

} /* extern "C" */
