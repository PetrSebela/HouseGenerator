#include "handle.hpp"

float Handle::Distance(glm::vec3 from) {
    return glm::distance(_position, from);
}

void Handle::SetPosition(glm::vec3 position) {
    _position = position + glm::vec3(0.0f, 0.01f, 0.0f);
}

glm::vec3 Handle::GetPosition() {
    return _position;
}

void Handle::Draw() {
    _shader->Bind();
    _shader->SetMatrix4x4("model", glm::translate(glm::identity<glm::mat4>(), _position));
    _mesh->Draw();
}
