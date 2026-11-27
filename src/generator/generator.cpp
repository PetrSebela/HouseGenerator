#include "generator.hpp"

#include <iostream>

#include "extruder.hpp"
#include "renderer/camera.hpp"
#include "geometry.hpp"
#include "roof/roof.hpp"
#include "roof/flat_roof.hpp"
#include "renderer/texture.hpp"
#include "roof/gabled_roof.hpp"
#include "roof/hipped_roof.hpp"

Generator::Generator() {

    auto wallShader = Shader::GetShaderByName("wall");
    _wall->SetShader(wallShader);

    auto foundationShader = Shader::GetShaderByName("foundation");

    _foundation->SetShader(foundationShader);

    // TODO: add propper roof tile material
    RoofGenerator = std::make_shared<GabledRoof>();
}

void Generator::SetFootprint(std::vector<glm::vec3> footprint) {
    _footprint.clear();

    for (int i = 0; i < footprint.size(); i++) {
        Segment segment(footprint[i], footprint[(i + 1) % footprint.size()]);
        _footprint.push_back(segment);
    }
}

void Generator::RefreshModel() {
    Extruder foundation(_footprint);
    foundation.Extrude(glm::vec2(0, FoundationHeight));
    foundation.Extrude(glm::vec2(0.2, 0));
    foundation.Extrude(glm::vec2(0, -FoundationHeight));

    auto foundationSet = foundation.GetWallSet();
    _foundation->SetMesh(foundationSet.GetMesh());

    Extruder wall(_footprint);
    wall.MoveHead(glm::vec2(0.1, FoundationHeight));
    wall.Extrude(glm::vec2(0, LevelHeight));
    auto wallSet = wall.GetWallSet();
    _wall->SetMesh(wallSet.GetMesh());

    auto roofPerimeter = wall.GetExtruderHead();
    RoofGenerator->SetPerimeter(roofPerimeter);
    _roof = RoofGenerator->GetRoof();
}

void Generator::Draw() {
    _foundation->Draw();
    _wall->Draw();

    for (auto& part: _roof)
        part.Draw();
}
