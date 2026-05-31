/*
 *  SPKFileInfo.h
 *  Internal to the CSPICE framework — not part of the public API.
 */

#include <map>
#include <vector>
#include <string>

class F_Info
{
	double jdBegin, jdEnd;
	std::string filename;
public:
	F_Info(const char* fname, double bg, double en);

	bool Within(double jd);
};

class Segments
{
	std::vector<F_Info> data;
	std::string asteroidName;
public:
	Segments(std::string astName);

	void Add(const char* fname, double bg, double en);
	bool JDisPresent(double jd);
	std::string GetAsteroidName();
};

class SPKFiles
{
	typedef std::map<unsigned, Segments> SFmap;
	typedef SFmap::iterator SFmapI;
	SFmap objects;
    std::map<int,std::string> names;

	SPKFiles(){};
public:

	void Add(int ids, const char* filename, double jdBegin, double jdEnd);
	void Load(const char* filename);
	std::vector<int> GetIds();
	bool ObjectIsPresent(int ids);
	bool JDisPresent(int ids, double jd);
	std::string GetAsteroidName(int ids);
    void setNames(std::map<int,std::string> names_)
    {
        names = names_;
    }

	static SPKFiles& GetInstance();
};
