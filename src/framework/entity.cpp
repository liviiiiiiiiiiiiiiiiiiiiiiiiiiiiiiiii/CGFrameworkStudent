#include "entity.h"
#include "mesh.h"
#include "image.h"
#include "camera.h"

#include <cmath> 

Entity::Entity() {
  mesh = nullptr;
  model.SetIdentity();

  base_position = Vector3(0.f, 0.f, 0.f);
  rot_axis = Vector3::UP;
  rotation_speed = 1.0f;
  scale = 1.0f;
  time_acc = 0.0f;
  phase = 0.0f;
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
      continue; // Skip triangles that are outside the clip space
    }
    // clip to screen: map from [-1,1] to [0, width/height]
    // Origin (0,0) is at bottom-left corner
    int sx1 = (c1.x + 1.0f) * 0.5f * (framebuffer->width -1);
    int sy1 = (c1.y + 1.0f) * 0.5f * (framebuffer->height -1);
    int sx2 = (c2.x + 1.0f) * 0.5f * (framebuffer->width -1);
    int sy2 = (c2.y + 1.0f) * 0.5f * (framebuffer->height -1);
    int sx3 = (c3.x + 1.0f) * 0.5f * (framebuffer->width -1);
    int sy3 = (c3.y + 1.0f) * 0.5f * (framebuffer->height -1);

    // create the vectopr 2 to draw the triangle directly
    Vector2 p1(sx1, sy1);
    Vector2 p2(sx2, sy2);
    Vector2 p3(sx3, sy3);
    // framebuffer->DrawLineDDA(p1.x,p1.y,p2.x,p2.y,Color::CYAN);
    // framebuffer->DrawLineDDA(p2.x,p2.y,p3.x,p3.y,Color::CYAN);
    // framebuffer->DrawLineDDA(p1.x,p1.y,p3.x,p3.y,Color::CYAN);
    framebuffer->DrawLineDDA(p1.x, p1.y, p2.x, p2.y, c);
    framebuffer->DrawLineDDA(p2.x, p2.y, p3.x, p3.y, c);
    framebuffer->DrawLineDDA(p3.x, p3.y, p1.x, p1.y, c);
    // framebuffer->SetPixel(p1.x +20,p1.y+20,Color::CYAN);
    // framebuffer->SetPixel(p2.x+20,p2.y+20,Color::CYAN);
    // framebuffer->SetPixel(p3.x+20,p3.y+20,Color::CYAN);
  }
}

void Entity::SetMesh(Mesh *m) { mesh = m; }

void Entity::SetModelMatrix(const Matrix44 &mat) { model = mat; }

Mesh *Entity::GetMesh() const { return mesh; }

Matrix44 Entity::GetModelMatrix() const { return model; }

void Entity::Update(float seconds_elapsed) {

  time_acc += seconds_elapsed;

  float angle = time_acc * rotation_speed;         
  float offset = sinf(time_acc + phase) * 0.3f; 

  Matrix44 T, R, S;
  T.MakeTranslationMatrix(base_position.x + offset, base_position.y, base_position.z);

  R.MakeRotationMatrix(angle, rot_axis);

  S.MakeScaleMatrix(scale, scale, scale);

  model = T * R * S;
}
