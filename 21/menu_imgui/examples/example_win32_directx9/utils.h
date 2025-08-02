#ifndef UTILS_H
#define UTILS_H

inline float CustomClamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

#endif // UTILS_H

ImVec4 main_color = ImVec4(210.0f / 255.0f, 100.0f / 255.0f, 185.0f / 255.0f, 1.0f); // Default menu color 210, 100, 185
