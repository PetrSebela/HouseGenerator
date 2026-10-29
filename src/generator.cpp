#include "generator.hpp"

#include <iostream>
#include <ostream>

#include "camera.hpp"
#include "geometry.hpp"

void Generator::SetFootprint(std::vector<glm::vec3> footprint) {
    _footprint.clear();

    for (int i = 0; i < footprint.size(); i++) {
        Segment segment(footprint[i], footprint[(i + 1) % footprint.size()]);
        _footprint.push_back(segment);
    }

    RefreshModel();
}

void Generator::RefreshModel() {
    auto base = geo::Inset(_footprint, 0.1f);
    base = geo::ConnectNeighbours(base, true);

    auto baseInset = geo::Inset(base, 0.5);
    baseInset = geo::ConnectNeighbours(baseInset, true);

    auto raised = geo::Raise(base, LevelHeight);
    auto roof = geo::Inset(raised,0.5);
    roof = geo::Raise(roof, 0.5);
    roof = geo::ConnectNeighbours(roof, true);

    auto wall = geo::BridgeSegments(base, raised);
    auto root = geo::BridgeSegments(raised, roof);
    auto innerWall = geo::BridgeSegments( roof, baseInset);
    auto a = Mesh::Join(wall, root);
    _walls = Mesh::Join(innerWall, a);
}

void Generator::Draw(Camera camera) {
    _walls->Draw();
}
