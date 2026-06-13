#include "Colors.h"

namespace Colors {
    static Colors::Scheme currentScheme = Colors::Scheme::CLASSIC;

    static const Vec3 PALETTES[3][6] = {
        /* CLASSIC */       { {1,1,1}, {1,0.85f,0}, {0.8f,0,0}, {1,0.45f,0}, {0,0.3f,1}, {0,0.6f,0.2f} },
        /* PASTEL */        { {0.95f,0.95f,0.95f}, {1,0.95f,0.6f}, {0.95f,0.6f,0.6f}, {1,0.75f,0.5f}, {0.6f,0.75f,1}, {0.65f,0.9f,0.7f} },
        /* HIGH_CONTRAST */ { {1,1,1}, {1,1,0}, {1,0,0}, {1,0.5f,0}, {0,0,1}, {0,1,0} },
    };

    void setScheme(Scheme s) { currentScheme = s; }
    Scheme getScheme() { return currentScheme; }

    Vec3 getColorVec(ColorName color) {
        if (color == BLACK) return {0.08f, 0.08f, 0.1f};
        if (color == NONE)  return {0,0,0};
        return PALETTES[(int)currentScheme][color];
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
