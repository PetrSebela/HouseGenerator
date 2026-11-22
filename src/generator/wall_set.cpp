#include "wall_set.hpp"
#include "geometry.hpp"

WallSet::WallSet(std::vector<std::vector<Segment>> &frames) {
    for (int frameIndex = 0; frameIndex < frames.size() - 1; frameIndex++) {
        auto bottom = frames[frameIndex];
        auto top = frames[frameIndex + 1];

        for (int wallIndex = 0; wallIndex < bottom.size(); wallIndex++) {
            Wall wall(bottom[wallIndex], top[wallIndex]);
            _walls.push_back(wall);
        }
    }
}

WallSet::WallSet(std::vector<Wall> &walls) {
    _walls = walls;
}

std::shared_ptr<Mesh> WallSet::GetMesh() {
    return geo::WallsToMesh(_walls);
}
