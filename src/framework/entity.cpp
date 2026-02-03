#include "entity.h"

Entity::Entity() {
  mesh = nullptr;
  model.SetIdentity();
}

Entity::~Entity() {
  // Note: We don't delete mesh here as it might be shared between entities
}

bool isInsideClipSpace(const Vector3 &v) {
  return (v.x >= -1 && v.x <= 1 && v.y >= -1 && v.y <= 1 && v.z >= -1 &&
          v.z <= 1);
}

void Entity::Render(Image *framebuffer, Camera *camera, const Color &c) {
  if (!mesh)
    return;
  const std::vector<Vector3> &vertices = mesh->GetVertices();
  for (int i = 0; i < vertices.size(); i += 3) {
    Vector3 v = vertices[i];
    Vector3 v2 = vertices[i + 1];
    Vector3 v3 = vertices[i + 2];
    // local to world
    Vector3 W1 = model * v;
    Vector3 W2 = model * v2;
    Vector3 W3 = model * v3;
    // world to view to clip
    Vector3 c1 = camera->ProjectVector(W1);
    Vector3 c2 = camera->ProjectVector(W2);
    Vector3 c3 = camera->ProjectVector(W3);

    if (!isInsideClipSpace(c1) || !isInsideClipSpace(c2) ||
        !isInsideClipSpace(c3)) {
      return;
    }
    // clip to screen: map from [-1,1] to [0, width/height]
    // Origin (0,0) is at bottom-left corner
    int sx1 = (c1.x + 1.0f) * 0.5f * framebuffer->width;
    int sy1 = (c1.y + 1.0f) * 0.5f * framebuffer->height;
    int sx2 = (c2.x + 1.0f) * 0.5f * framebuffer->width;
    int sy2 = (c2.y + 1.0f) * 0.5f * framebuffer->height;
    int sx3 = (c3.x + 1.0f) * 0.5f * framebuffer->width;
    int sy3 = (c3.y + 1.0f) * 0.5f * framebuffer->height;

    // create the vectopr 2 to draw the triangle directly
    Vector2 p1(sx1, sy1);
    Vector2 p2(sx2, sy2);
    Vector2 p3(sx3, sy3);

    framebuffer->DrawTriangle(p1, p2, p3, c, 1, false, c);
  }
}

void Entity::SetMesh(Mesh *m) { mesh = m; }

void Entity::SetModelMatrix(const Matrix44 &mat) { model = mat; }

Mesh *Entity::GetMesh() const { return mesh; }

Matrix44 Entity::GetModelMatrix() const { return model; }