// Copyright 2022 oon00b (@oon00b)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "config_common.h"

#define MATRIX_ROWS 4
#define MATRIX_COLS 12

#define MATRIX_ROWS_PINS {F6, F7, B1, B3}
#define MATRIX_COLS_PINS {D3, D2, D1, D0, D4, C6, D7, E6, B4, B5, B2, B6}

#define ANALOG_JOYSTICK_X_AXIS_PIN F4
#define ANALOG_JOYSTICK_Y_AXIS_PIN F5

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
#ifndef NO_DEBUG
    #define NO_DEBUG
#endif

/* disable print */
#ifndef NO_PRINT
    #define NO_PRINT
#endif

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
