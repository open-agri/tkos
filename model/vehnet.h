/**
 * @file vehnet.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Vehnet status model.
 * @version 0.1
 * @date 2020-09-14
 * 
 * 
 */

#pragma once

typedef enum {
    TK_VEHNET_DISABLED,
    TK_VEHNET_CONNECTING,
    TK_VEHNET_COMPLETE
} tk_vehnet_status_t;