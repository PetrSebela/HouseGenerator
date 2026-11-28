#include "gabled_roof.hpp"

#include <algorithm>
#include <iostream>

#include "extruder.hpp"
#include "geometry.hpp"
#include "gizmos.hpp"
#include "scene_object.hpp"
#include "utils.hpp"

std::vector<glm::vec3> GabledRoof::FillRegions(glm::vec3 origin, glm::vec3 columnDirection, glm::vec3 rowDirection, std::vector<Segment> &segments) {
    std::vector<glm::vec3> positions;

    // TODO: parametrize tile size and spacing?
    glm::vec2 spacing(0.2,0.2);

    for (int y = 1; y < 100; y++) {
        auto root = origin + columnDirection * spacing.y * static_cast<float>(y);
        auto intersections = geo::GetIntersections(root, rowDirection, segments);
        if (intersections.size() <= 1)
            continue;

        for (int i = 0; i < intersections.size(); i += 2) {
            float start = round(intersections[i] * 10) / 10.0f + (y % 2 == 0 ? spacing.x/2: 0) ;
            float end = intersections[i + 1];

            for (float x = start; x < end; x += 0.2) {
                positions.push_back(root + rowDirection * x);
            }
        }
    }
    return positions;
}

std::shared_ptr<Mesh> GabledRoof::GetRoofBridgeWall() {
    std::vector<Segment> leftWallEdges, rightWallEdges;
    auto ridgeWallDistances = geo::GetIntersections(SplitPoint, ridge.GetDirection(), _perimeter);

    if (ridgeWallDistances.size() < 2)
        return std::make_shared<Mesh>();

    auto gizmos = Gizmos::GetInstance();

    for (int i = 0; i < ridgeWallDistances.size() - 1; i += 2) {
        glm::vec3 start = SplitPoint + ridge.GetDirection() * ridgeWallDistances[i];
        glm::vec3 end = SplitPoint + ridge.GetDirection() * ridgeWallDistances[i + 1];

        gizmos->DrawPoint(start);
        gizmos->DrawPoint(end);

        leftWallEdges.emplace_back(
            geo::Intersects(start, glm::vec3(0,1,0), leftPlane),
            geo::Intersects(end, glm::vec3(0,1,0), leftPlane)
        );

        rightWallEdges.emplace_back(
            geo::Intersects(start, glm::vec3(0,1,0), rightPlane),
            geo::Intersects(end, glm::vec3(0,1,0), rightPlane)
        );
    }
    return geo::BridgeSegments(leftWallEdges, rightWallEdges);
}

std::vector<Segment> GabledRoof::ProjectOnRoof(const std::vector<Segment> &segments) {
    std::vector<Segment> projected;
    for (const auto& segment : segments) {
        auto relativeStart = geo::Flatten(segment.Start - ridge.Start);
        auto relativeEnd = geo::Flatten(segment.End - ridge.End);
        auto startSide = geo::Cross(relativeStart, geo::Flatten(ridge.GetDirection()));
        auto endSide = geo::Cross(relativeEnd, geo::Flatten(ridge.GetDirection()));

        const auto& roofPlane = startSide + endSide < 0 ? leftPlane : rightPlane;
        auto startRoof = geo::Intersects(segment.Start, glm::vec3(0,1,0), roofPlane);
        auto endRoof = geo::Intersects(segment.End, glm::vec3(0,1,0), roofPlane);
        projected.emplace_back(startRoof, endRoof);
    }
    return projected;
}

std::vector<SceneObject> GabledRoof::GenerateGutters(std::vector<std::vector<Segment>> segments) {
    std::vector<SceneObject> gutters;
    for (auto& segment : segments) {
        Extruder gutter(segment);
        gutter.MoveHead(glm::vec2(0.0,-0.075));
        gutter.Extrude(glm::vec2(-0.25,0));
        gutter.Extrude(glm::vec2(0,0.125));
        gutter.Extrude(glm::vec2(0.3,0));
        auto drainMesh = gutter.GetWallSet().GetMesh();
        gutters.emplace_back(drainMesh, Shader::GetShaderByName("basalt"));
    }
    return gutters;
}

GabledRoof::GabledRoof() {
    LeftDescriptor = {
        .angle = 15.f,
        .minRidgeHeight = 0.f
    };
    RightDescriptor = {
        .angle = 15.f,
        .minRidgeHeight = 0.f
    };
    UpdateConfig();
}

void GabledRoof::UpdateConfig() {
    if (_perimeter.empty())
        return;

    float baseHeight = _perimeter[0].Start.y + 0.01f;
    glm::vec3 splitDirection = glm::rotate(glm::identity<glm::mat4>(), glm::radians(RidgeAzimuth), glm::vec3(0,1,0)) * glm::vec4(0,0,1,1);
    SplitPoint.y = baseHeight;
    ridge = Segment(SplitPoint + splitDirection * 25.0f, SplitPoint - splitDirection * 25.0f);

    float leftMax = 0;
    float rightMax = 0;
    for (const auto& segment : _perimeter) {
        float distance = geo::DistanceToLine(ridge.Start, ridge.End, segment.Start);
        auto relative = geo::Flatten(segment.Start - ridge.Start);
        auto left = geo::Cross(relative, geo::Flatten(ridge.GetDirection())) < 0;
        if (left)
            leftMax = std::max(leftMax, distance);
        else
            rightMax = std::max(rightMax, distance);
    }

    float leftMinHeight = glm::max(glm::tan( glm::radians(LeftDescriptor.angle)) * leftMax, LeftDescriptor.minRidgeHeight) + baseHeight;
    float rightMinHeight = glm::max(glm::tan( glm::radians(RightDescriptor.angle)) * rightMax, RightDescriptor.minRidgeHeight) + baseHeight;

    auto leftSplitPoint = glm::vec3(SplitPoint.x, MatchLeftRightRidge ? glm::max(leftMinHeight, rightMinHeight) : leftMinHeight, SplitPoint.z);
    auto rightSplitPoint = glm::vec3(SplitPoint.x, MatchLeftRightRidge ? glm::max(leftMinHeight, rightMinHeight) : rightMinHeight, SplitPoint.z);

    leftNormal = glm::angleAxis(glm::radians(-LeftDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    leftTangent = glm::cross(leftNormal, splitDirection);

    rightNormal = glm::angleAxis(glm::radians( RightDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    rightTangent = glm::cross(splitDirection, rightNormal);

    leftPlane = {leftSplitPoint, leftNormal};
    rightPlane = {rightSplitPoint, rightNormal};
}

std::vector<SceneObject> GabledRoof::GetRoof() {
    auto gizmos = Gizmos::GetInstance();

    UpdateConfig();
    auto perimeter = geo::BisectPolygon(_perimeter, ridge);

    BisectResult projectedPerimeter = {
        .left = ProjectOnRoof(perimeter.left),
        .right = ProjectOnRoof(perimeter.right),
        .combined = ProjectOnRoof(perimeter.combined)
    };

    gizmos->DrawSegments(projectedPerimeter.left);

    auto leftRegions = geo::JoinIntoRegions(projectedPerimeter.left, ridge);
    auto rightRegions = geo::JoinIntoRegions(projectedPerimeter.right, ridge);

    auto wall = GetRoofBridgeWall();
    auto splitWall = geo::BridgeSegments(projectedPerimeter.combined, perimeter.combined);

    std::vector roof = {
        SceneObject(splitWall, Shader::GetShaderByName("wall")),
        SceneObject(wall, Shader::GetShaderByName("wall")),
    };

    std::vector<std::vector<Segment>> regions;
    if (MatchLeftRightRidge) {
        regions.push_back(projectedPerimeter.combined);
    }
    else {
        regions.insert(regions.end(), leftRegions.begin(), leftRegions.end());
        regions.insert(regions.end(), rightRegions.begin(), rightRegions.end());
    }

    auto drains = GenerateGutters(regions);
    roof.insert(roof.end(), drains.begin(), drains.end());

    if (HighQuality) {
        auto yawTransform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(RidgeAzimuth), glm::vec3(0, 1, 0));
        glm::mat4 leftRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(LeftDescriptor.angle + 3), ridge.GetDirection());
        glm::mat4 rightRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(-RightDescriptor.angle - 3), ridge.GetDirection());

        auto leftRotated = Mesh::Transform(_tile, leftRotation * yawTransform);
        auto rightRotated = Mesh::Transform(_tile, rightRotation * yawTransform);

        // Compute roof tile positions
        std::vector<glm::vec3> leftTilePositions = FillRegions(leftPlane.point, leftTangent, ridge.GetDirection(), perimeter.left);
        std::vector<glm::vec3> rightTilePosition = FillRegions(rightPlane.point, rightTangent, ridge.GetDirection(), perimeter.right);

        auto leftMesh = Mesh::Stamp(leftRotated, leftTilePositions);
        auto rightMesh = Mesh::Stamp(rightRotated, rightTilePosition);
        roof.emplace_back(leftMesh, Shader::GetShaderByName("basalt"));
        roof.emplace_back(rightMesh, Shader::GetShaderByName("basalt"));
    }
    else {
        for (const auto& region :  leftRegions) {
            roof.emplace_back(geo::Fill(region, RidgeAzimuth), Shader::GetShaderByName("tiles"));
        }

        for (const auto& region : rightRegions) {
            roof.emplace_back(geo::Fill(region, RidgeAzimuth - 180), Shader::GetShaderByName("basalt"));
        }
    }

    return roof;
}
