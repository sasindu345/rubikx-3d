#include "Move.h"
#include <sstream>

std::string Move::toString() const {
    std::string res = "";
    
    // For layer > 1, prefix with layer number
    if (layer > 1) {
        res += std::to_string(layer + 1);
    }

    char fChar = ' ';
    switch (face) {
        case Face::RIGHT: fChar = (layer == 1) ? 'r' : 'R'; break;
        case Face::LEFT:  fChar = (layer == 1) ? 'l' : 'L'; break;
        case Face::UP:    fChar = (layer == 1) ? 'u' : 'U'; break;
        case Face::DOWN:  fChar = (layer == 1) ? 'd' : 'D'; break;
        case Face::FRONT: fChar = (layer == 1) ? 'f' : 'F'; break;
        case Face::BACK:  fChar = (layer == 1) ? 'b' : 'B'; break;
    }
    res += fChar;

    if (dir == Direction::CCW) {
        res += "'";
    }
    
    return res;
}

Move Move::parse(const std::string& str) {
    if (str.empty()) return Move();

    int idx = 0;
    int layerVal = 0;

    // Parse leading layer number if any (e.g. "3R")
    if (std::isdigit(str[idx])) {
        int val = 0;
        while (idx < (int)str.length() && std::isdigit(str[idx])) {
            val = val * 10 + (str[idx] - '0');
            idx++;
        }
        layerVal = val - 1; // 1-indexed in string representation, 0-indexed in code
    }

    if (idx >= (int)str.length()) return Move();

    char fChar = str[idx++];
    Face f = Face::RIGHT;
    
    // Check for lowercase (inner slice turn, i.e. layer 1)
    if (std::islower(fChar)) {
        layerVal = 1;
        fChar = std::toupper(fChar);
    }

    switch (fChar) {
        case 'R': f = Face::RIGHT; break;
        case 'L': f = Face::LEFT; break;
        case 'U': f = Face::UP; break;
        case 'D': f = Face::DOWN; break;
        case 'F': f = Face::FRONT; break;
        case 'B': f = Face::BACK; break;
        default:  f = Face::RIGHT; break;
    }

    Direction d = Direction::CW;
    if (idx < (int)str.length() && str[idx] == '\'') {
        d = Direction::CCW;
    }

    return Move(f, d, layerVal);
}
