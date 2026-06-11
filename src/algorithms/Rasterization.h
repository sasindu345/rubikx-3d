#pragma once
#include <string>

// Rasterizes a text string at specified coordinates using GLUT fonts
void rasterizeFont(int x, int y, const std::string& str, void* font, float r, float g, float b);
