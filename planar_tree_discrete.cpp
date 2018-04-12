#include "planar_tree_discrete.h"

Range range(int x, int y) { return std::make_pair(x, y); }
std::string to_string(Point p) {
    return "<" + std::to_string(p.x) + "," + std::to_string(p.y) + ">";  
}
