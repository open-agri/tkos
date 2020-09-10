/**
 * @file engine.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Engine data model.
 * @version 0.1
 * @date 2020-09-07
 * 
 * 
 */

#ifndef ENGINE_H
#define ENGINE_H

typedef struct {

    double rpm;
    bool rpm_available;

    double temp_c;
    bool temp_c_available;

} tk_engine_data_t;

tk_engine_data_t tk_engine_last_data;

#endif