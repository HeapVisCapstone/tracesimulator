#include "planar_tree_discrete.h"

Range range(int r, int c) { return std::make_pair(r, c); }
std::string to_string(Point p) {
    return "<" + std::to_string(p.row) + "," + std::to_string(p.col) + ">";  
}
