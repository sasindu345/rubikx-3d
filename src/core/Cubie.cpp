#include "core/Cubie.h"

Cubie::Cubie() : ix(0), iy(0), iz(0) {
    for (int i = 0; i < 6; ++i) {
        colors[i] = Colors::NONE;
    }
}

Cubie::Cubie(int x, int y, int z, int size) : ix(x), iy(y), iz(z) {
    for (int i = 0; i < 6; ++i) {
        colors[i] = Colors::NONE;
    }

    // Set facelet colors based on standard Western Rubik's scheme
    if (ix == size - 1) colors[static_cast<int>(Face::RIGHT)] = Colors::RED;
    if (ix == 0)        colors[static_cast<int>(Face::LEFT)]  = Colors::ORANGE;
    if (iy == size - 1) colors[static_cast<int>(Face::UP)]    = Colors::WHITE;
    if (iy == 0)        colors[static_cast<int>(Face::DOWN)]  = Colors::YELLOW;
    if (iz == size - 1) colors[static_cast<int>(Face::FRONT)] = Colors::GREEN;
    if (iz == 0)        colors[static_cast<int>(Face::BACK)]  = Colors::BLUE;
}

Cubie::~Cubie() {}

void Cubie::rotate(Face face, Direction dir, int size) {
    int n = size;
    int r = static_cast<int>(Face::RIGHT);
    int l = static_cast<int>(Face::LEFT);
    int u = static_cast<int>(Face::UP);
    int d = static_cast<int>(Face::DOWN);
    int f = static_cast<int>(Face::FRONT);
    int b = static_cast<int>(Face::BACK);

    switch (face) {
        case Face::RIGHT:
            if (dir == Direction::CW) {
                int ny = n - 1 - iz;
                int nz = iy;
                iy = ny; iz = nz;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[b];
                colors[b] = colors[d];
                colors[d] = colors[f];
                colors[f] = tmp;
            } else {
                int ny = iz;
                int nz = n - 1 - iy;
                iy = ny; iz = nz;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[f];
                colors[f] = colors[d];
                colors[d] = colors[b];
                colors[b] = tmp;
            }
            break;

        case Face::LEFT:
            if (dir == Direction::CW) {
                // CW looking from left face (-X): Y' = Z, Z' = N - 1 - Y
                int ny = iz;
                int nz = n - 1 - iy;
                iy = ny; iz = nz;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[f];
                colors[f] = colors[d];
                colors[d] = colors[b];
                colors[b] = tmp;
            } else {
                // CCW looking from left face (-X): Y' = N - 1 - Z, Z' = Y
                int ny = n - 1 - iz;
                int nz = iy;
                iy = ny; iz = nz;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[b];
                colors[b] = colors[d];
                colors[d] = colors[f];
                colors[f] = tmp;
            }
            break;

        case Face::UP:
            if (dir == Direction::CW) {
                // CW looking from top (+Y): X' = N - 1 - Z, Z' = X
                int nx = n - 1 - iz;
                int nz = ix;
                ix = nx; iz = nz;

                Colors::ColorName tmp = colors[b];
                colors[b] = colors[l];
                colors[l] = colors[f];
                colors[f] = colors[r];
                colors[r] = tmp;
            } else {
                // CCW looking from top (+Y): X' = Z, Z' = N - 1 - X
                int nx = iz;
                int nz = n - 1 - ix;
                ix = nx; iz = nz;

                Colors::ColorName tmp = colors[b];
                colors[b] = colors[r];
                colors[r] = colors[f];
                colors[f] = colors[l];
                colors[l] = tmp;
            }
            break;

        case Face::DOWN:
            if (dir == Direction::CW) {
                // CW looking from bottom (-Y): X' = Z, Z' = N - 1 - X
                int nx = iz;
                int nz = n - 1 - ix;
                ix = nx; iz = nz;

                Colors::ColorName tmp = colors[f];
                colors[f] = colors[l];
                colors[l] = colors[b];
                colors[b] = colors[r];
                colors[r] = tmp;
            } else {
                // CCW looking from bottom (-Y): X' = N - 1 - Z, Z' = X
                int nx = n - 1 - iz;
                int nz = ix;
                ix = nx; iz = nz;

                Colors::ColorName tmp = colors[f];
                colors[f] = colors[r];
                colors[r] = colors[b];
                colors[b] = colors[l];
                colors[l] = tmp;
            }
            break;

        case Face::FRONT:
            if (dir == Direction::CW) {
                // CW looking from front (+Z): X' = Y, Y' = N - 1 - X
                int nx = iy;
                int ny = n - 1 - ix;
                ix = nx; iy = ny;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[l];
                colors[l] = colors[d];
                colors[d] = colors[r];
                colors[r] = tmp;
            } else {
                // CCW looking from front (+Z): X' = N - 1 - Y, Y' = X
                int nx = n - 1 - iy;
                int ny = ix;
                ix = nx; iy = ny;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[r];
                colors[r] = colors[d];
                colors[d] = colors[l];
                colors[l] = tmp;
            }
            break;

        case Face::BACK:
            if (dir == Direction::CW) {
                // CW looking from back (-Z): X' = N - 1 - Y, Y' = X
                int nx = n - 1 - iy;
                int ny = ix;
                ix = nx; iy = ny;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[r];
                colors[r] = colors[d];
                colors[d] = colors[l];
                colors[l] = tmp;
            } else {
                // CCW looking from back (-Z): X' = Y, Y' = N - 1 - X
                int nx = iy;
                int ny = n - 1 - ix;
                ix = nx; iy = ny;

                Colors::ColorName tmp = colors[u];
                colors[u] = colors[l];
                colors[l] = colors[d];
                colors[d] = colors[r];
                colors[r] = tmp;
            }
            break;
    }
}
