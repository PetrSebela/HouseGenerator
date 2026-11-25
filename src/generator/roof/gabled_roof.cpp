#include "gabled_roof.hpp"

#include <iostream>
#include <ostream>

#include "extruder.hpp"
#include "geometry.hpp"
#include "gizmos.hpp"
#include "scene_object.hpp"

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

std::shared_ptr<Mesh> GabledRoof::GetMesh() {
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
        auto right = geo::Cross(relative, geo::Flatten(ridge.GetDirection())) > 0;
        if (right)
            rightMax = std::max(rightMax, distance);
        else
            leftMax = std::max(leftMax, distance);
    }

    float leftHeight = glm::max(glm::tan( glm::radians(LeftDescriptor.angle)) * leftMax, LeftDescriptor.minRidgeHeight) + baseHeight;
    float rightHeight = glm::max(glm::tan( glm::radians(RightDescriptor.angle)) * rightMax, RightDescriptor.minRidgeHeight) + baseHeight;

    gizmos->DrawLine(glm::vec3(0, leftHeight,0) + splitDirection * 25.0f, glm::vec3(0, leftHeight,0) - splitDirection * 25.0f);
    gizmos->DrawLine(glm::vec3(0, rightHeight,0) + splitDirection * 25.0f, glm::vec3(0, rightHeight,0) - splitDirection * 25.0f);

    SplitPoint.y = glm::max(SplitPoint.y, leftHeight);
    SplitPoint.y = glm::max(SplitPoint.y, rightHeight);


    auto leftNormal = glm::angleAxis(glm::radians(-LeftDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    auto leftTangent = glm::cross(leftNormal, splitDirection);
    auto rightNormal = glm::angleAxis(glm::radians( RightDescriptor.angle), splitDirection) * glm::vec3(0,1,0);
    auto rightTangent = glm::cross(splitDirection, rightNormal);

    gizmos->DrawLine(ridge, Gizmos::Color("Ridge"));
    gizmos->DrawVector(SplitPoint, leftNormal, Gizmos::BLUE);
    gizmos->DrawVector(SplitPoint, leftTangent, Gizmos::BLUE);
    gizmos->DrawVector(SplitPoint, rightNormal, Gizmos::RED);
    gizmos->DrawVector(SplitPoint, rightTangent, Gizmos::RED);

    Plane leftPlane = {SplitPoint, leftNormal};
    Plane rightPlane = {SplitPoint, rightNormal};

    std::vector<Segment> leftSegments;
    std::vector<Segment> rightSegments;
    std::vector<Segment> corrected;

    for (int segmentIndex = 0; segmentIndex < _perimeter.size(); segmentIndex++) {
        auto segment = _perimeter[segmentIndex];

        auto relativeStart = geo::Flatten(segment.Start - ridge.Start);
        auto relativeEnd = geo::Flatten(segment.End - ridge.End);

        auto startCross = geo::Cross(relativeStart, geo::Flatten(ridge.GetDirection()));
        auto endCross = geo::Cross(relativeEnd, geo::Flatten(ridge.GetDirection()));

        if (glm::sign(startCross) == glm::sign(endCross)) {
            if (glm::sign(startCross) < 0) {
                leftSegments.push_back(segment);
            }
            else {
                rightSegments.push_back(segment);
            }
            corrected.push_back(segment);
            continue;
        }

        bool parallel;
        glm::vec2 point;
        geo::Intersection(ridge, _perimeter[segmentIndex], parallel, point);

        Segment leftHalf(segment.Start, glm::vec3(point.x, segment.Start.y,point.y));
        Segment rightHalf(glm::vec3(point.x, segment.Start.y,point.y), segment.End);
        corrected.push_back(leftHalf);
        corrected.push_back(rightHalf);

        if (glm::sign(startCross) < 0) {
            leftSegments.push_back(leftHalf);
            rightSegments.push_back(rightHalf);
        }
        else {
            leftSegments.push_back(rightHalf);
            rightSegments.push_back(leftHalf);
        }
    }

    std::vector<Segment> ridges;

    for (const auto& segment : leftSegments) {
        gizmos->DrawLine(segment, Gizmos::Color("Left"));
        auto startRoof = geo::Intersects(segment.Start, glm::vec3(0,1,0), leftPlane);
        auto endRoof = geo::Intersects(segment.End, glm::vec3(0,1,0), leftPlane);
        gizmos->DrawLine(segment.Start, startRoof, Gizmos::BLACK);
        gizmos->DrawLine(segment.End, endRoof, Gizmos::BLACK);

        ridges.emplace_back(
            glm::vec3(segment.Start.x, startRoof.y, segment.Start.z),
            glm::vec3(segment.End.x, endRoof.y, segment.End.z)
        );
    }

    for (const auto& segment : rightSegments) {
        gizmos->DrawLine(segment, Gizmos::Color("Right"));
        auto startRoof = geo::Intersects(segment.Start, glm::vec3(0,1,0), rightPlane);
        auto endRoof = geo::Intersects(segment.End, glm::vec3(0,1,0), rightPlane);
        gizmos->DrawLine(segment.Start, startRoof, Gizmos::BLACK);
        gizmos->DrawLine(segment.End, endRoof, Gizmos::BLACK);

        ridges.emplace_back(
            glm::vec3(segment.Start.x, startRoof.y, segment.Start.z),
            glm::vec3(segment.End.x, endRoof.y, segment.End.z)
        );
    }



    std::vector<glm::vec3> leftPositions = FillRoof(SplitPoint, leftTangent, splitDirection, leftSegments);
    std::vector<glm::vec3> rightPosition = FillRoof(SplitPoint, rightTangent, splitDirection, rightSegments);

    auto right = glm::cross(glm::vec3(0,1,0), splitDirection);
    float angle = geo::Angle(glm::vec3(0,0,1), right);
    auto yawTransform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle-90.0f), glm::vec3(0, 1, 0));

    glm::mat4 leftRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(-LeftDescriptor.angle + 3), splitDirection);
    glm::mat4 rightRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(RightDescriptor.angle - 3), splitDirection);

    auto leftRotated = Mesh::Transform(_tile, leftRotation * yawTransform);
    auto rightRotated = Mesh::Transform(_tile, rightRotation * yawTransform);

    auto leftMesh = Mesh::Stamp(leftRotated, leftPositions);
    auto rightMesh = Mesh::Stamp(rightRotated, rightPosition);
    auto tiles = Mesh::Join(leftMesh, rightMesh);


    std::vector<Segment> outlinePerimeter;

    for (const auto& segment : corrected) {
        auto relativeStart = geo::Flatten(segment.Start - ridge.Start);
        auto relativeEnd = geo::Flatten(segment.End - ridge.End);
        auto startCross = geo::Cross(relativeStart, geo::Flatten(ridge.GetDirection()));
        auto endCross = geo::Cross(relativeEnd, geo::Flatten(ridge.GetDirection()));
        auto startRoof = geo::Intersects(segment.Start, glm::vec3(0,1,0), startCross < 0 ? leftPlane : rightPlane);
        auto endRoof = geo::Intersects(segment.End, glm::vec3(0,1,0), endCross < 0 ? leftPlane : rightPlane);
        outlinePerimeter.emplace_back(startRoof, endRoof);
    }

    gizmos->DrawSegments(outlinePerimeter);

    Extruder outline(outlinePerimeter);
    outline.MoveHead(glm::vec2(0.25,-0.075));
    outline.Extrude(glm::vec2(-0.5,0));
    outline.Extrude(glm::vec2(0,0.125));
    outline.Extrude(glm::vec2(0.3,0));

    auto roof = geo::BridgeSegments(outlinePerimeter, corrected);
    roof->Join(outline.GetWallSet().GetMesh());
    roof->Join(tiles);

    return roof;
}
