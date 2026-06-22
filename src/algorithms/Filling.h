#pragma once

// Draws a solid filled 2D quadrilateral (implied scanline fill by graphics hardware)
void rasterizeQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a);

// Draws an unfilled wireframe border outline
void rasterizeBorder(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, float r, float g, float b, float a, float lineWidth);

// Simulates the scan-line polygon filling algorithm across a vertical span
void manualScanLineFill(int yStart, int yEnd, float r, float g, float b);
