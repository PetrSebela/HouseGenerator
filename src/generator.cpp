#include "generator.hpp"

#include <iostream>
#include <ostream>

#include "camera.hpp"
#include "geometry.hpp"
#include "texture.hpp"

Generator::Generator() {
    auto brickAlbedo = Texture::LoadTexture("assets/textures/plaster/albedo.jpg");
    auto brickNormal = Texture::LoadTexture("assets/textures/plaster/normal.jpg");
    auto wallShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    wallShader->SetTexture2D(GL_TEXTURE1, brickAlbedo);
    wallShader->SetTexture2D(GL_TEXTURE2, brickNormal);
    _wall->SetShader(wallShader);
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

    auto wallMesh = Mesh::Join(innerWall, a);
    _wall->SetMesh(wallMesh);
}

void Generator::Draw() {
    _wall->Draw();
}
