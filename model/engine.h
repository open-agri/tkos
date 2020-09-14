/**
 * @file engine.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Engine data model.
 * @version 0.1
 * @date 2020-09-07
 * 
 * 
 */

#pragma once

/**
 * @brief A struct for representing data related to the engine.
 * 
 */
typedef struct {

    /**
     * @brief The engine's RPM.
     * 
     */
    double rpm;
    bool rpm_available;

    /**
     * @brief The engine's external temperature in Celsius degrees.
     * 
     */
    double temp_c;
    bool temp_c_available;

} tk_engine_data_t;

