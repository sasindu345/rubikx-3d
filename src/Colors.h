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

    enum class Scheme { CLASSIC, PASTEL, HIGH_CONTRAST, COUNT };

    void setScheme(Scheme s);
    Scheme getScheme();

    Vec3 getColorVec(ColorName color);
    const char* getColorNameString(ColorName color);
}
