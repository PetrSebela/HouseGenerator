#include "generator.hpp"

#include "extruder.hpp"
#include "renderer/camera.hpp"
#include "geometry.hpp"
#include "roof/roof.hpp"
#include "roof/flat_roof.hpp"
#include "renderer/texture.hpp"
#include "roof/gabled_roof.hpp"
#include "roof/hipped_roof.hpp"

Generator::Generator() {
    auto plasterAlbedo = Texture::LoadTexture("assets/textures/plaster/albedo.jpg");
    auto plasterNormal = Texture::LoadTexture("assets/textures/plaster/normal.jpg");
    auto wallShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    wallShader->SetTexture2D(GL_TEXTURE1, plasterAlbedo);
    wallShader->SetTexture2D(GL_TEXTURE2, plasterNormal);
    _wall->SetShader(wallShader);

    auto concreteAlbedo = Texture::LoadTexture("assets/textures/concrete/albedo.jpg");
    auto concreteNormal = Texture::LoadTexture("assets/textures/concrete/normal.jpg");
    auto foundationShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    foundationShader->SetTexture2D(GL_TEXTURE1, concreteAlbedo);
    foundationShader->SetTexture2D(GL_TEXTURE2, concreteNormal);

    _foundation->SetShader(foundationShader);

    // TODO: add propper roof tile material
    _roof->SetShader(wallShader);
    _roofGenerator = std::make_shared<GabledRoof>();
}

void Generator::SetFootprint(std::vector<glm::vec3> footprint) {
    _footprint.clear();

    for (int i = 0; i < footprint.size(); i++) {
        Segment segment(footprint[i], footprint[(i + 1) % footprint.size()]);
        _footprint.push_back(segment);
    }

    RefreshModel();
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
    _roofGenerator->SetPerimeter(roofPerimeter);
    _roof->SetMesh(_roofGenerator->GetMesh());
}

void Generator::Draw() {
    _foundation->Draw();
    _wall->Draw();
    _roof->Draw();
}
