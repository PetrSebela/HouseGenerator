#include "hipped_roof.hpp"

#include <iostream>
#include <ostream>

#include "geometry.hpp"
#include "gizmos.hpp"
#include "utils.hpp"

HippedRoof::HippedRoof() {

}

std::shared_ptr<Mesh> HippedRoof::GetMesh() {
    // https://en.wikipedia.org/wiki/Straight_skeleton
    // https://github.com/prochitecture/bpypolyskel

    std::vector<Segment> edges;
    std::vector<Segment> ridge;

    auto gizmos= Gizmos::GetInstance();
    for (int i = 0; i < _perimeter.size(); i++) {
        auto left = _perimeter[i % _perimeter.size()].GetDirection();
        auto right = -_perimeter[Utils::Mod(i - 1, _perimeter.size())].GetDirection();
        auto normal = glm::normalize((left + right) / 2.0f);
        edges.emplace_back(_perimeter[i].Start, _perimeter[i].Start + normal);
    }



    for (int i = 0; i < edges.size(); i++) {
        bool parallel;
        glm::vec2 intersection;
        geo::Intersection(edges[i], edges[(i + 1) % edges.size()], parallel, intersection);

        if (parallel)
            continue;

        glm::vec3 point(intersection.x, edges[i].End.y, intersection.y);
        std::cout << "dsf" << std::endl;
        gizmos->DrawPoint(point);
        gizmos->DrawLine(edges[i].Start, point);
        gizmos->DrawLine(edges[(i + 1) % edges.size()].Start, point);
    }

    return std::make_shared<Mesh>();
}