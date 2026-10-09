#ifndef _SCENE_OBJECT_HPP_
#define _SCENE_OBJECT_HPP_

#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"

class SceneObject {
    Mesh _mesh;
    Shader _shader;
    glm::mat4x4 _modelMatrix;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale = glm::vec3(1.0f);

    void UpdateModelMatrix();

public:
    SceneObject(Mesh mesh, Shader s);
    ~SceneObject() = default;
    void SetPosition(glm::vec3 rotation);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void Draw(Camera camera);
};


#endif
