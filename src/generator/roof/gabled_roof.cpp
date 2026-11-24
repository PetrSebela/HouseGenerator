#include "gabled_roof.hpp"

#include <iostream>
#include <ostream>

#include "extruder.hpp"
#include "geometry.hpp"
#include "gizmos.hpp"
#include "scene_object.hpp"

std::vector<glm::vec3> GabledRoof::FillRoof(glm::vec3 origin, glm::vec3 fillDirection, glm::vec3 rowDirection, std::vector<Segment> &segments) {
    std::vector<glm::vec3> positions;
    for (float y = 0.2f; y < 10; y+=0.24) {
        auto root = origin + fillDirection * y;
        auto intersections = geo::GetIntersections(root, rowDirection, segments);

        if (intersections.size() <= 1) {
            continue;
        }

        for (int i = 0; i < intersections.size(); i += 2) {
            float start = round(intersections[i] * 10) / 10.0f;
            float end = intersections[i + 1];

            for (float x = start; x < end; x += 0.35) {
                positions.push_back(root + rowDirection * x);
            }
        }
    }
    return positions;
}

std::shared_ptr<Mesh> GabledRoof::GetMesh() {
    auto gizmos = Gizmos::GetInstance();

    std::vector<Segment> corrected;
    glm::vec3 splitPoint(0,5,0);
    glm::vec3 splitDirection = glm::normalize(glm::vec3(1,0,1));

    Segment ridge(splitPoint + splitDirection * 25.0f, splitPoint - splitDirection * 25.0f);

    auto leftNormal = glm::angleAxis(glm::radians( -10.0f), splitDirection) * glm::vec3(0,1,0);
    auto leftTangent = glm::cross(leftNormal, splitDirection);
    auto rightNormal = glm::angleAxis(glm::radians( 10.0f), splitDirection) * glm::vec3(0,1,0);
    auto rightTangent = glm::cross(splitDirection, rightNormal);

    gizmos->DrawLine(ridge, Gizmos::Color("Ridge"));
    gizmos->DrawVector(splitPoint, leftNormal, Gizmos::BLUE);
    gizmos->DrawVector(splitPoint, leftTangent, Gizmos::BLUE);
    gizmos->DrawVector(splitPoint, rightNormal, Gizmos::RED);
    gizmos->DrawVector(splitPoint, rightTangent, Gizmos::RED);

    Plane leftPlane = {splitPoint, leftNormal};
    Plane rightPlane = {splitPoint, rightNormal};

    std::vector<Segment> leftSegments;
    std::vector<Segment> rightSegments;

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



    std::vector<glm::vec3> leftPositions = FillRoof(splitPoint, leftTangent, splitDirection, leftSegments);
    std::vector<glm::vec3> rightPosition = FillRoof(splitPoint, rightTangent, splitDirection, rightSegments);

    auto right = glm::cross(glm::vec3(0,1,0), splitDirection);
    float angle = geo::Angle(glm::vec3(0,0,1), right);
    auto yawTransform = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle-90.0f), glm::vec3(0, 1, 0));

    glm::mat4 leftRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(-3.0f), splitDirection);
    glm::mat4 rightRotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(3.0f), splitDirection);

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
    outline.Extrude(glm::vec2(0.2,0));
    auto roof = Mesh::Join(outline.GetWallSet().GetMesh(), tiles);
    return roof;
    return std::make_shared<Mesh>();
}
