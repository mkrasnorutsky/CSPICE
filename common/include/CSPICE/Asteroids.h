#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double pos_x; /* km */
    double pos_y;
    double pos_z;
    double vel_x; /* km/s */
    double vel_y;
    double vel_z;
    bool valid;
} AsteroidResult;

void asteroids_init(const char* docsPath);

bool asteroids_load_ephe_files(const char* path);

AsteroidResult asteroids_calculate(
    double jd,
    int jpl_id,
    int obs_id,
    char* error_buffer,
    int error_buffer_size
);

bool asteroids_object_is_present(int naif_id);

bool asteroids_jd_is_present(int naif_id, double jd);

const char* asteroids_asteroid_name(int naif_id);

int asteroids_last_observer_id(void);

int asteroids_loaded_spk_id_count(void);
int asteroids_copy_loaded_spk_ids(int* ids, int max_count);

int asteroids_recently_added_count(void);
int asteroids_copy_recently_added_ids(int* ids, int max_count);

enum AsteroidCategory : unsigned
{
    AsteroidCategory_DwarfPlanets = 0,
    AsteroidCategory_KuiperBeltObjects,
    AsteroidCategory_Centaurs,
    AsteroidCategory_ClassicalAsteroids,
    AsteroidCategory_AstrologicalAsteroids
};

unsigned asteroid_category_count(void);
const char* asteroid_category_name(AsteroidCategory category);
unsigned asteroid_count_for_category(AsteroidCategory category);
unsigned asteroid_number(AsteroidCategory category, unsigned asteroidIndex);
unsigned asteroid_naif_id(AsteroidCategory category, unsigned asteroidIndex);
void asteroid_generate_tf_file_for_asteroid_naif_id(unsigned naif_id);

#ifdef __cplusplus
}
#endif

#endif /* ASTEROIDS_H */
