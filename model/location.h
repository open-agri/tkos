/**
 * @file location.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Location data model.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#pragma once

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

/**
 * @brief The possible states of the GPS system.
 * 
 */
typedef enum {
    TK_GPS_STATUS_OFF,
    TK_GPS_STATUS_CONNECTING,
    TK_GPS_STATUS_CONNECTED
} tk_gps_status_t;

