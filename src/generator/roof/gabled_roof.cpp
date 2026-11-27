#include "gabled_roof.hpp"

#include <algorithm>
#include <iostream>

#include "extruder.hpp"
#include "geometry.hpp"
#include "gizmos.hpp"
#include "scene_object.hpp"
#include "utils.hpp"

std::vector<glm::vec3> GabledRoof::FillRoof(glm::vec3 origin, glm::vec3 fillDirection, glm::vec3 rowDirection, std::vector<Segment> &segments) {
    std::vector<glm::vec3> positions;

    glm::vec2 spacing(0.2,0.2);

    for (int y = 1; y < 100; y++) {
        auto root = origin + fillDirection * spacing.y * static_cast<float>(y);
        auto intersections = geo::GetIntersections(root, rowDirection, segments);

        if (intersections.size() <= 1) {
            continue;
        }

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

BisectResult GabledRoof::BisectPerimeter(std::vector<Segment> &perimeter, Segment &bisector) {
    std::vector<Segment> leftSide;
    std::vector<Segment> rightSide;
    std::vector<Segment> merged;

    for (const auto & segment : perimeter) {
        auto relativeStart = geo::Flatten(segment.Start - bisector.Start);
        auto relativeEnd = geo::Flatten(segment.End - bisector.End);

        auto startCross = geo::Cross(relativeStart, geo::Flatten(bisector.GetDirection()));
        auto endCross = geo::Cross(relativeEnd, geo::Flatten(bisector.GetDirection()));

        bool onBisector = glm::abs(startCross) < 1e-3 || glm::abs(endCross) < 1e-3;
        if (onBisector) {
            if (glm::sign(startCross + endCross) < 0)
                leftSide.push_back(segment);
            else
                rightSide.push_back(segment);
            merged.push_back(segment);
            continue;
        }

        bool sameSide = glm::sign(startCross) == glm::sign(endCross);
        if (sameSide) {
            if (glm::sign(startCross) < 0)
                leftSide.push_back(segment);
            else
                rightSide.push_back(segment);
            merged.push_back(segment);
            continue;
        }

        bool parallel;
        glm::vec2 point;
        geo::Intersection(bisector, segment, parallel, point);

        Segment leftHalf(segment.Start, glm::vec3(point.x, segment.Start.y,point.y));
        Segment rightHalf(glm::vec3(point.x, segment.Start.y,point.y), segment.End);
        merged.push_back(leftHalf);
        merged.push_back(rightHalf);

        if (glm::sign(startCross) < 0) {
            leftSide.push_back(leftHalf);
            rightSide.push_back(rightHalf);
        }
        else {
            leftSide.push_back(rightHalf);
            rightSide.push_back(leftHalf);
        }
    }

    return {leftSide, rightSide, merged};
}

std::shared_ptr<Mesh> GabledRoof::GetRoofBridgeWall(glm::vec3 splitDirection, Plane leftPlane, Plane rightPlane) {
    std::vector<Segment> leftWallEdges, rightWallEdges;
    auto ridgeWallDistances = geo::GetIntersections(SplitPoint, splitDirection, _perimeter);
    std::cout << ridgeWallDistances.size() << std::endl;

    if (ridgeWallDistances.size() < 2)
        return std::make_shared<Mesh>();

    auto gizmos = Gizmos::GetInstance();

    for (int i = 0; i < ridgeWallDistances.size() - 1; i += 2) {
        glm::vec3 start = SplitPoint + splitDirection * ridgeWallDistances[i];
        glm::vec3 end = SplitPoint + splitDirection * ridgeWallDistances[i + 1];

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

std::vector<Segment> GabledRoof::ProjectSegmentOnRoof(const std::vector<Segment> &segments, Plane left, Plane right, Segment ridge) {
    std::vector<Segment> projected;
    for (const auto& segment : segments) {
        auto relativeStart = geo::Flatten(segment.Start - ridge.Start);
        auto relativeEnd = geo::Flatten(segment.End - ridge.End);
        auto startSide = geo::Cross(relativeStart, geo::Flatten(ridge.GetDirection()));
        auto endSide = geo::Cross(relativeEnd, geo::Flatten(ridge.GetDirection()));

        const auto& roofPlane = startSide + endSide < 0 ? left : right;
        auto startRoof = geo::Intersects(segment.Start, glm::vec3(0,1,0), roofPlane);
        auto endRoof = geo::Intersects(segment.End, glm::vec3(0,1,0), roofPlane);
        projected.emplace_back(startRoof, endRoof);
    }
    return projected;
}

std::vector<SceneObject> GabledRoof::GetSimplifiedRoofMesh(Segment ridge, const std::vector<Segment>& segments, Plane leftPlane, Plane rightPlane, bool left) {
    auto projected = ProjectSegmentOnRoof(segments, leftPlane, rightPlane, ridge);

    bool pureLoop = true;
    for (int i = 0; i < projected.size(); ++i) {
        pureLoop &= glm::distance(projected[i].End, projected[(i + 1)%projected.size()].Start) < 1e-3f;
    }

    if (pureLoop)
        return {SceneObject(geo::Fill(projected), Shader::GetShaderByName("tiles"))};

    glm::vec3 axis = ridge.GetDirection() * (left ? 1.0f : -1.0f);

    int segmentStart = 0;
    for (int i = 0; i < projected.size(); ++i) {
        if (glm::distance(projected[i].End, projected[(i + 1) % projected.size()].Start) > 1e-3){
            segmentStart = i + 1;
            break;
        }
    }

    std::vector<std::vector<Segment>> chains;
    std::vector<Segment> chain;
    for (size_t i = 0; i < projected.size(); ++i) {
        auto current  = projected[(segmentStart + i) % projected.size()];
        auto next = projected[(segmentStart + i + 1) % projected.size()];

        chain.push_back(current);
        if (glm::distance(current.End, next.Start) > 1e-3) {
            chains.push_back(chain);
            chain.clear();
        }
    }

    if (!chain.empty())
        chains.push_back(chain);

    struct Span {
        float start;
        float end;
        std::vector<Segment> segments;
    };

    std::vector<Span> outerSpans;
    std::vector<Span> innerSpans;

    for (auto continuous : chains) {
        auto start = continuous.front().Start - ridge.Start;
        auto end = continuous.back().End - ridge.Start;

        auto startDistance = glm::dot(start, axis);
        auto endDistance = glm::dot(end, axis);

        // For left just flip expression
        auto& span = startDistance < endDistance ? outerSpans : innerSpans;
        span.push_back({
            startDistance,
            endDistance,
            continuous
        });
    }

    sort(outerSpans.begin(), outerSpans.end(), [](const Span &a, const Span &b){return a.start < b.start;});
    sort(innerSpans.begin(), innerSpans.end(), [](const Span& a, const Span& b) { return a.start > b.start; });

    std::vector<std::vector<Segment>> weaved;
    for (auto span : outerSpans) {
        std::vector<Segment> weave = span.segments;

        for (auto inner : innerSpans) {
            if (span.start < inner.end && span.end > inner.start) {
                weave.emplace_back(weave.back().End, inner.segments.front().Start);
                weave.insert(weave.end(), inner.segments.begin(), inner.segments.end());
            }
        }

        weave.emplace_back(
            weave.back().End,
            weave.front().Start
        );
        weaved.push_back(weave);
    }

    std::vector<SceneObject> fills;
    for (auto island : weaved) {
        fills.emplace_back(geo::Fill(island), Shader::GetShaderByName("tiles"));
    }
    return fills;
}

std::vector<SceneObject> GabledRoof::GetRoof() {
    auto gizmos = Gizmos::GetInstance();
    float baseHeight = _perimeter[0].Start.y + 0.01f;
    glm::vec3 splitDirection = glm::rotate(glm::identity<glm::mat4>(), glm::radians(RidgeAzimuth), glm::vec3(0,1,0)) * glm::vec4(0,0,1,1);
    SplitPoint.y = baseHeight;
    Segment ridge(SplitPoint + splitDirection * 25.0f, SplitPoint - splitDirection * 25.0f);

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

    glm::vec3 leftSplitPoint(SplitPoint.x, MatchLeftRightRidge ? glm::max(leftMinHeight, rightMinHeight) : leftMinHeight, SplitPoint.z);
    glm::vec3 rightSplitPoint(SplitPoint.x, MatchLeftRightRidge ? glm::max(leftMinHeight, rightMinHeight) : rightMinHeight, SplitPoint.z);

    // gizmos->DrawLine(leftSplitPoint + splitDirection * 25.0f, leftSplitPoint - splitDirection * 25.0f);
    // gizmos->DrawLine(rightSplitPoint + splitDirection * 25.0f, rightSplitPoint - splitDirection * 25.0f);

    auto leftNormal = glm::angleAxis(glm::radians(-LeftDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    auto leftTangent = glm::cross(leftNormal, splitDirection);
    auto rightNormal = glm::angleAxis(glm::radians( RightDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    auto rightTangent = glm::cross(splitDirection, rightNormal);

    auto bisectedPerimeter = BisectPerimeter(_perimeter, ridge);

    // Compute roof tile positions
    std::vector<glm::vec3> leftTilePositions = FillRoof(leftSplitPoint, leftTangent, splitDirection, bisectedPerimeter.left);
    std::vector<glm::vec3> rightTilePosition = FillRoof(rightSplitPoint, rightTangent, splitDirection, bisectedPerimeter.right);

    // Project bisected perimeter onto roof planes
    Plane leftPlane = {leftSplitPoint, leftNormal};
    Plane rightPlane = {rightSplitPoint, rightNormal};
    std::vector<Segment> outlinePerimeter = ProjectSegmentOnRoof(bisectedPerimeter.combined, leftPlane, rightPlane, ridge);

    Extruder drain(outlinePerimeter);
    drain.MoveHead(glm::vec2(0.0,-0.075));
    drain.Extrude(glm::vec2(-0.25,0));
    drain.Extrude(glm::vec2(0,0.125));
    drain.Extrude(glm::vec2(0.3,0));
    auto drainMesh = drain.GetWallSet().GetMesh();
    auto wall = GetRoofBridgeWall(splitDirection, leftPlane, rightPlane);
    auto splitWall = geo::BridgeSegments(outlinePerimeter, bisectedPerimeter.combined);

    std::vector roof = {
        SceneObject(drainMesh, Shader::GetShaderByName("basalt")),
        SceneObject(splitWall, Shader::GetShaderByName("wall")),
        SceneObject(wall, Shader::GetShaderByName("wall")),
    };

    if (HighQuality) {
        auto right = glm::cross(glm::vec3(0,1,0), splitDirection);
        float angle = geo::Angle(glm::vec3(0,0,1), right);
        auto yawTransform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle-90.0f), glm::vec3(0, 1, 0));

        glm::mat4 leftRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(-LeftDescriptor.angle + 3), splitDirection);
        glm::mat4 rightRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(RightDescriptor.angle - 3), splitDirection);

        auto leftRotated = Mesh::Transform(_tile, leftRotation * yawTransform);
        auto rightRotated = Mesh::Transform(_tile, rightRotation * yawTransform);

        auto leftMesh = Mesh::Stamp(leftRotated, leftTilePositions);
        auto rightMesh = Mesh::Stamp(rightRotated, rightTilePosition);
        roof.emplace_back(leftMesh, Shader::GetShaderByName("tiles"));
        roof.emplace_back(rightMesh, Shader::GetShaderByName("tiles"));
    }
    else {
        auto tilesLeft = GetSimplifiedRoofMesh(ridge, bisectedPerimeter.left, leftPlane, rightPlane, true);
        roof.insert(roof.end(), tilesLeft.begin(), tilesLeft.end());
        auto tilesRight = GetSimplifiedRoofMesh(ridge, bisectedPerimeter.right, leftPlane, rightPlane, false);
        roof.insert(roof.end(), tilesRight.begin(), tilesRight.end());
    }
    return roof;
}
