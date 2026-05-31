/*
 *  Asteroids.cpp
 *  CSPICE
 */

#include "Asteroids.h"
#include "AsteroidNames.h"

#include "SpiceUsr.h"
#include "SpiceZdf.h"
#include "SpiceZfc.h"
#include "SPKFileInfo.h"
#include "Directory.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT 2000000
#define NAIF_INTEGER_ID_CODES_COMETS_SHIFT 1000000

using namespace std;

std::string Asteroids_docsPath;

namespace {

bool loadObjectIdsFromJson(const string& path, vector<int>& objectIds)
{
    ifstream input(path);
    if (!input)
    {
        return false;
    }

    string content((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());

    const size_t objectsKey = content.find("\"objects\"");
    if (objectsKey == string::npos)
    {
        return false;
    }

    size_t pos = content.find('[', objectsKey);
    if (pos == string::npos)
    {
        return false;
    }

    ++pos;
    objectIds.clear();

    while (pos < content.size())
    {
        while (pos < content.size() &&
               (content[pos] == ' ' || content[pos] == '\n' ||
                content[pos] == '\r' || content[pos] == '\t' || content[pos] == ','))
        {
            ++pos;
        }

        if (pos >= content.size() || content[pos] == ']')
        {
            break;
        }

        if (content[pos] == '-' || isdigit(static_cast<unsigned char>(content[pos])))
        {
            char* end = nullptr;
            const long value = strtol(content.c_str() + pos, &end, 10);
            if (end == content.c_str() + pos)
            {
                break;
            }

            objectIds.push_back(static_cast<int>(value));
            pos = static_cast<size_t>(end - content.c_str());
        }
        else
        {
            break;
        }
    }

    return true;
}

bool saveObjectIdsToJson(const string& path, const vector<int>& objectIds)
{
    ofstream output(path, ios::binary);
    if (!output)
    {
        return false;
    }

    output << "{\n  \"objects\" : [\n";
    for (size_t i = 0; i < objectIds.size(); ++i)
    {
        if (i > 0)
        {
            output << ",\n";
        }
        output << "    " << objectIds[i];
    }
    output << "\n  ]\n}\n";

    return output.good();
}

bool vectorContainsId(const vector<int>& values, int value)
{
    return find(values.begin(), values.end(), value) != values.end();
}

} // namespace

class Asteroids
{
    vector<int> _recentlyAddedObjects;

    Asteroids();
    void LoadEpheFiles(string path);

public:
    vector<double> Calculate(double jd, int jplID, int& obsID, string& errorDescription) const;
    vector<int> recentlyAddedObjects() const { return _recentlyAddedObjects; }

    static Asteroids* GetInstancePtr();
    void loadEpheFiles(string path) { LoadEpheFiles(path); }
    static bool objectIsPresent(int naifId);
    static bool jdisPresent(int naifId, double jd);
    static string asteroidName(int naifId);
    static vector<int> loadedSpkIds();
};

const char* SPICEFilesFolder()
{
    return Asteroids_docsPath.c_str();
}

void printids(int ids)
{
    const string name = SPKFiles::GetInstance().GetAsteroidName(ids);
    printf("Loaded Asteroid id = %d , %s;\n", ids, name.c_str());
}

Asteroids::Asteroids()
{
    erract_c("SET", sizeof("RETURN") + 1, "RETURN");

    map<int, string> nameByNaifId;
    for (size_t i = 0; i < ASTEROID_NAMES_COUNT; ++i)
    {
        const AsteroidNameEntry& entry = asteroidNames[i];
        nameByNaifId[NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT + entry.id] = entry.name;
    }

    SPKFiles::GetInstance().setNames(nameByNaifId);

    const string idsListPath = Asteroids_docsPath + "/Asteroids.json";
    vector<int> previousObjectIds;
    loadObjectIdsFromJson(idsListPath, previousObjectIds);

    LoadEpheFiles(SPICEFilesFolder());

    const vector<int> ids = SPKFiles::GetInstance().GetIds();
    for_each(ids.begin(), ids.end(), printids);

    if (!previousObjectIds.empty())
    {
        for (const int id : ids)
        {
            if (!vectorContainsId(previousObjectIds, id))
            {
                _recentlyAddedObjects.push_back(id);
            }
        }
    }

    saveObjectIdsToJson(idsListPath, ids);
}

void Asteroids::LoadEpheFiles(string path)
{
    vector<string> lst;

    if (!Directory::GetEntriesList(path.c_str(), lst))
    {
        return;
    }

    for (size_t i = 0; i < lst.size(); ++i)
    {
        const string& currentFile = lst[i];

        if (currentFile.size() < 3)
        {
            continue;
        }

        const string ext2 = currentFile.substr(currentFile.size() - 3, 3);
        const string ext3 = currentFile.size() >= 4 ? currentFile.substr(currentFile.size() - 4, 4) : "";

        if (ext2 != ".bc" &&
            ext3 != ".bee" &&
            ext3 != ".bpc" &&
            ext3 != ".bsp")
        {
            continue;
        }

        const string targetFile = path + "/" + lst[i];

        reset_c();
        furnsh_c(targetFile.c_str());

        if (!failed_c())
        {
            SPKFiles::GetInstance().Load(targetFile.c_str());
        }
    }

    for (const int id : SPKFiles::GetInstance().GetIds())
    {
        try
        {
            const int idt = stoi(SPKFiles::GetInstance().GetAsteroidName(id));
            if (idt >= NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT &&
                idt < (NAIF_INTEGER_ID_CODES_ASTEROID_SHIFT + 1000000))
            {
                printf("Unnamed asteroid: %d\n", idt);
            }
        }
        catch (const invalid_argument&)
        {
        }
        catch (const out_of_range&)
        {
        }
        catch (const exception&)
        {
        }
    }

    for (size_t i = 0; i < lst.size(); ++i)
    {
        const string& currentFile = lst[i];

        if (currentFile.size() < 3)
        {
            continue;
        }

        const string ext2 = currentFile.substr(currentFile.size() - 3, 3);

        if (ext2 != ".tf")
        {
            continue;
        }

        const string targetFile = path + "/" + lst[i];

        reset_c();
        furnsh_c(targetFile.c_str());
    }

    reset_c();
}

static SpiceInt getOriginalObserver(double et, int objectId, string& errorDescription)
{
    logical found = 0;
    doublereal descr[5];
    char ident[40];
    integer handle;
    integer iBody = objectId;
    doublereal drET = et;

    spksfs_(&iBody, &drET, &handle, descr, ident, &found, (ftnlen)40);

    if (found)
    {
        SpiceInt body = objectId;
        SpiceInt center;
        SpiceInt frame;
        SpiceInt type;
        SpiceDouble first;
        SpiceDouble last;
        SpiceInt begin;
        SpiceInt end;
        spkuds_c(descr, &body, &center, &frame, &type, &first, &last, &begin, &end);

        return center;
    }

    const string name = SPKFiles::GetInstance().GetAsteroidName(objectId);
    errorDescription = string("Center of object ") + name + " is unavailiable.";
    return 0;
}

vector<double> Asteroids::Calculate(double jd, int jplID, int& obsID, string& errorDescription) const
{
    errorDescription.clear();

    double lt = 0.0;
    const double et = (jd - 2451545.0) * spd_c();

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
        const string name = SPKFiles::GetInstance().GetAsteroidName(jplID);
        errorDescription = string("Object ") + name + " is unavailiable.";
    }

    return result;
}

Asteroids* Asteroids::GetInstancePtr()
{
    if (Asteroids_docsPath.empty())
    {
        return nullptr;
    }

    static Asteroids ast;
    return &ast;
}

bool Asteroids::objectIsPresent(int naifId)
{
    if (!GetInstancePtr())
    {
        return false;
    }

    return SPKFiles::GetInstance().ObjectIsPresent(naifId);
}

bool Asteroids::jdisPresent(int naifId, double jd)
{
    if (!GetInstancePtr())
    {
        return false;
    }

    return SPKFiles::GetInstance().JDisPresent(naifId, jd);
}

string Asteroids::asteroidName(int naifId)
{
    if (!GetInstancePtr())
    {
        return string();
    }

    return SPKFiles::GetInstance().GetAsteroidName(naifId);
}

vector<int> Asteroids::loadedSpkIds()
{
    if (!GetInstancePtr())
    {
        return vector<int>();
    }

    return SPKFiles::GetInstance().GetIds();
}

static thread_local string g_lastAsteroidName;
static thread_local int g_lastObserverId = 0;

extern "C" {

void asteroids_init(const char* docsPath)
{
    if (docsPath)
    {
        Asteroids_docsPath = docsPath;
    }
    else
    {
        Asteroids_docsPath.clear();
    }
}

bool asteroids_load_ephe_files(const char* path)
{
    if (!path)
    {
        return false;
    }

    Asteroids* instance = Asteroids::GetInstancePtr();
    if (!instance)
    {
        return false;
    }

    instance->loadEpheFiles(path);
    return true;
}

AsteroidResult asteroids_calculate(double jd, int jpl_id, int obs_id,
                                   char* error_buffer, int error_buffer_size)
{
    AsteroidResult r = {0};

    if (error_buffer && error_buffer_size > 0)
    {
        error_buffer[0] = '\0';
    }

    Asteroids* instance = Asteroids::GetInstancePtr();
    if (!instance)
    {
        if (error_buffer && error_buffer_size > 0)
        {
            strncpy(error_buffer, "Asteroids not initialized.", error_buffer_size - 1);
            error_buffer[error_buffer_size - 1] = '\0';
        }
        return r;
    }

    int observer = obs_id;
    string errorDescription;
    const vector<double> result = instance->Calculate(jd, jpl_id, observer, errorDescription);
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

const char* asteroids_asteroid_name(int naif_id)
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
    return static_cast<int>(Asteroids::loadedSpkIds().size());
}

int asteroids_copy_loaded_spk_ids(int* ids, int max_count)
{
    const vector<int> loaded = Asteroids::loadedSpkIds();
    const int count = static_cast<int>(loaded.size());
    if (!ids || max_count <= 0)
    {
        return count;
    }

    const int toCopy = count < max_count ? count : max_count;
    for (int i = 0; i < toCopy; ++i)
    {
        ids[i] = loaded[i];
    }

    return count;
}

int asteroids_recently_added_count(void)
{
    Asteroids* instance = Asteroids::GetInstancePtr();
    if (!instance)
    {
        return 0;
    }

    return static_cast<int>(instance->recentlyAddedObjects().size());
}

int asteroids_copy_recently_added_ids(int* ids, int max_count)
{
    Asteroids* instance = Asteroids::GetInstancePtr();
    const vector<int> recent = instance ? instance->recentlyAddedObjects() : vector<int>();
    const int count = static_cast<int>(recent.size());
    if (!ids || max_count <= 0)
    {
        return count;
    }

    const int toCopy = count < max_count ? count : max_count;
    for (int i = 0; i < toCopy; ++i)
    {
        ids[i] = recent[i];
    }

    return count;
}

} /* extern "C" */
