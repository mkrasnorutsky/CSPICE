#include <jni.h>

#include <cstring>
#include <string>
#include <vector>

#include <CSPICE/Asteroids.h>

namespace {

jstring NewStringAscii(JNIEnv* env, const char* text)
{
    if (text == nullptr)
    {
        text = "";
    }
    return env->NewStringUTF(text);
}

std::string JStringToStdString(JNIEnv* env, jstring value)
{
    if (value == nullptr)
    {
        return {};
    }

    const char* utf = env->GetStringUTFChars(value, nullptr);
    if (utf == nullptr)
    {
        return {};
    }

    std::string result(utf);
    env->ReleaseStringUTFChars(value, utf);
    return result;
}

jintArray NewIntArray(JNIEnv* env, const std::vector<int>& values)
{
    jintArray array = env->NewIntArray(static_cast<jsize>(values.size()));
    if (array == nullptr)
    {
        return nullptr;
    }

    if (!values.empty())
    {
        env->SetIntArrayRegion(array, 0, static_cast<jsize>(values.size()), values.data());
    }

    return array;
}

jobject NewAsteroidResult(JNIEnv* env, const AsteroidResult& result, const char* errorText)
{
    jclass resultClass = env->FindClass("co/krasnorutsky/cspice/Cspice$AsteroidResult");
    if (resultClass == nullptr)
    {
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(
        resultClass,
        "<init>",
        "(DDDDDDZLjava/lang/String;)V");
    if (constructor == nullptr)
    {
        return nullptr;
    }

    jstring error = NewStringAscii(env, errorText);
    return env->NewObject(
        resultClass,
        constructor,
        result.pos_x,
        result.pos_y,
        result.pos_z,
        result.vel_x,
        result.vel_y,
        result.vel_z,
        result.valid ? JNI_TRUE : JNI_FALSE,
        error);
}

} // namespace

extern "C" {

JNIEXPORT void JNICALL
Java_co_krasnorutsky_cspice_Cspice_init(JNIEnv* env, jclass, jstring docsPath)
{
    const std::string path = JStringToStdString(env, docsPath);
    asteroids_init(path.empty() ? nullptr : path.c_str());
}

JNIEXPORT jboolean JNICALL
Java_co_krasnorutsky_cspice_Cspice_loadEpheFiles(JNIEnv* env, jclass, jstring pathValue)
{
    const std::string path = JStringToStdString(env, pathValue);
    if (path.empty())
    {
        return JNI_FALSE;
    }

    return asteroids_load_ephe_files(path.c_str()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_co_krasnorutsky_cspice_Cspice_objectIsPresent(JNIEnv*, jclass, jint naifId)
{
    return asteroids_object_is_present(naifId) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_co_krasnorutsky_cspice_Cspice_jdIsPresent(JNIEnv*, jclass, jint naifId, jdouble jd)
{
    return asteroids_jd_is_present(naifId, jd) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_co_krasnorutsky_cspice_Cspice_asteroidName(JNIEnv* env, jclass, jint naifId)
{
    return NewStringAscii(env, asteroids_asteroid_name(naifId));
}

JNIEXPORT jint JNICALL
Java_co_krasnorutsky_cspice_Cspice_lastObserverId(JNIEnv*, jclass)
{
    return asteroids_last_observer_id();
}

JNIEXPORT jintArray JNICALL
Java_co_krasnorutsky_cspice_Cspice_loadedSpkIds(JNIEnv* env, jclass)
{
    const int count = asteroids_loaded_spk_id_count();
    if (count <= 0)
    {
        return env->NewIntArray(0);
    }

    std::vector<int> ids(static_cast<size_t>(count));
    asteroids_copy_loaded_spk_ids(ids.data(), count);
    return NewIntArray(env, ids);
}

JNIEXPORT jintArray JNICALL
Java_co_krasnorutsky_cspice_Cspice_recentlyAddedIds(JNIEnv* env, jclass)
{
    const int count = asteroids_recently_added_count();
    if (count <= 0)
    {
        return env->NewIntArray(0);
    }

    std::vector<int> ids(static_cast<size_t>(count));
    asteroids_copy_recently_added_ids(ids.data(), count);
    return NewIntArray(env, ids);
}

JNIEXPORT jobject JNICALL
Java_co_krasnorutsky_cspice_Cspice_calculate(
    JNIEnv* env,
    jclass,
    jdouble jd,
    jint jplId,
    jint obsId)
{
    char errorBuffer[512] = {0};
    const AsteroidResult result =
        asteroids_calculate(jd, jplId, obsId, errorBuffer, static_cast<int>(sizeof(errorBuffer)));
    return NewAsteroidResult(env, result, errorBuffer);
}

} // extern "C"
