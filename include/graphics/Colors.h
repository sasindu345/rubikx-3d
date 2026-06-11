#pragma once
#include "utils/MathUtils.h"

namespace Colors {
    enum ColorName {
        WHITE = 0,
        YELLOW,
        RED,
        ORANGE,
        BLUE,
        GREEN,
        BLACK,
        NONE
    };

    extern const Vec3 RGB_WHITE;
    extern const Vec3 RGB_YELLOW;
    extern const Vec3 RGB_RED;
    extern const Vec3 RGB_ORANGE;
    extern const Vec3 RGB_BLUE;
    extern const Vec3 RGB_GREEN;
    extern const Vec3 RGB_BLACK;

    Vec3 getColorVec(ColorName color);
    const char* getColorNameString(ColorName color);
}
