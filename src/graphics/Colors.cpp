#include "graphics/Colors.h"

namespace Colors {
    // Premium tailored colors for modern aesthetics
    const Vec3 RGB_WHITE(1.0f, 1.0f, 1.0f);
    const Vec3 RGB_YELLOW(0.95f, 0.85f, 0.1f);
    const Vec3 RGB_RED(0.85f, 0.15f, 0.15f);
    const Vec3 RGB_ORANGE(0.95f, 0.5f, 0.05f);
    const Vec3 RGB_BLUE(0.1f, 0.4f, 0.85f);
    const Vec3 RGB_GREEN(0.15f, 0.7f, 0.25f);
    const Vec3 RGB_BLACK(0.08f, 0.08f, 0.1f);

    Vec3 getColorVec(ColorName color) {
        switch (color) {
            case WHITE:  return RGB_WHITE;
            case YELLOW: return RGB_YELLOW;
            case RED:    return RGB_RED;
            case ORANGE: return RGB_ORANGE;
            case BLUE:   return RGB_BLUE;
            case GREEN:  return RGB_GREEN;
            case BLACK:  return RGB_BLACK;
            default:     return RGB_BLACK;
        }
    }

    const char* getColorNameString(ColorName color) {
        switch (color) {
            case WHITE:  return "WHITE";
            case YELLOW: return "YELLOW";
            case RED:    return "RED";
            case ORANGE: return "ORANGE";
            case BLUE:   return "BLUE";
            case GREEN:  return "GREEN";
            case BLACK:  return "BLACK";
            default:     return "NONE";
        }
    }
}
