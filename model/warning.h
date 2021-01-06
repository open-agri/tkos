/**
 * @file warning.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief General warning model.
 * @version 0.1
 * @date 2020-09-14
 * 
 * 
 */

#pragma once

typedef enum {
    TK_WARNING_NONE,
    TK_WARNING_INFO,
    TK_WARNING_ATTENTION,
    TK_WARNING_ATTENTION_FLASHING,
    TK_WARNING_CRITICAL,
    TK_WARNING_CRITICAL_FLASHING
} tk_warning_level_t;