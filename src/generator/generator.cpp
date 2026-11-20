#include "generator.hpp"

#include <iostream>
#include <ostream>

#include "renderer/camera.hpp"
#include "geometry.hpp"
#include "renderer/texture.hpp"

Generator::Generator() {

    auto brickAlbedo = Texture::LoadTexture("assets/textures/plaster/albedo.jpg");
    auto brickNormal = Texture::LoadTexture("assets/textures/plaster/normal.jpg");
    auto wallShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    wallShader->SetTexture2D(GL_TEXTURE1, brickAlbedo);
    wallShader->SetTexture2D(GL_TEXTURE2, brickNormal);


    _wall->SetShader(wallShader);
    _wall->SetPosition(glm::vec3(0.0f, -0.0f, 0.0f));
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

    auto foundationOuterEdge = _footprint;
    foundationOuterEdge = geo::ConnectNeighbours(foundationOuterEdge, true);
    auto foundationOuterEdgeTop = geo::Raise(foundationOuterEdge, FoundationHeight);

    auto foundationInnerEdge = geo::Inset(foundationOuterEdge, 0.2);
    foundationInnerEdge = geo::ConnectNeighbours(foundationInnerEdge, true);
    auto foundationInnerEdgeTop = geo::Raise(foundationInnerEdge, FoundationHeight);


    auto foundationSideOuter = geo::BridgeSegments(foundationOuterEdge, foundationOuterEdgeTop);
    auto foundationSideInner = geo::BridgeSegments(foundationInnerEdgeTop, foundationInnerEdge);
    auto foundationTop = geo::BridgeSegments(foundationOuterEdgeTop, foundationInnerEdgeTop);

    auto foundationMesh = Mesh::Join(foundationSideOuter, foundationTop);
    foundationMesh = Mesh::Join(foundationMesh, foundationSideInner);



    _wall->SetMesh(foundationMesh);
}

void Generator::Draw() {
    _wall->Draw();
}
