/**
 * @file location.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Location data model.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#ifndef LOCATION_H
#define LOCATION_H

/**
 * @brief A struct for representing data related to the current location.
 * 
 */
typedef struct {

    /**
     * @brief The latitude in degrees.
     * 
     */
    double lat;
    bool lat_available;

    /**
     * @brief The longitude in degrees.
     * 
     */
    double lon;
    bool lon_available;

    /**
     * @brief The altitude in meters over sea level.
     * 
     */
    double altitude;
    bool altitude_available;

    /**
     * @brief The speed in km/h.
     * 
     */
    double speed;
    bool speed_available;

    /**
     * @brief The heading calculated by the GPS, in degrees.
     * 
     */
    double gps_heading;
    bool gps_heading_available;

    /**
     * @brief The heading from the compass, in degrees and referenced to the geographic north.
     * 
     */
    double heading;
    bool heading_available;



} tk_location_data_t;

// TODO: Move last data(s) to a separate file
/**
 * @brief The most updated data about the engine.
 * 
 */
tk_location_data_t tk_location_last_data;

#endif