#include "entity.h"
#include "camera.h"
#include "image.h"
#include "mesh.h"

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

// LAB 2 Task 2.2: Implement isInsideClipSpace to discard vertices outside the
// frustum
bool isInsideClipSpace(const Vector3 &v) {
  // Clip space is a 2x2x2 cube centered at (0,0,0) with range [-1, 1]
  return (v.x >= -1 && v.x <= 1 && v.y >= -1 && v.y <= 1 && v.z >= -1 &&
          v.z <= 1);
}

void Entity::Render(Image *framebuffer, Camera *camera, FloatImage *zbuffer,
                    const Color &c) {

  if (!mesh || !framebuffer || !camera)
    return;

  const std::vector<Vector3> &vertices = mesh->GetVertices();
  for (int i = 0; i < (int)vertices.size(); i += 3) {
    // LAB 2 Task 2.2: Transform vertices (Local -> World)
    Vector3 W1 = model * vertices[i];
    Vector3 W2 = model * vertices[i + 1];
    Vector3 W3 = model * vertices[i + 2];

    // LAB 2 Task 2.2: Project vertices (World -> Clip)
    Vector3 c1 = camera->ProjectVector(W1);
    Vector3 c2 = camera->ProjectVector(W2);
    Vector3 c3 = camera->ProjectVector(W3);

    // LAB 2 Task 2.2: Clipping
    // Discard triangles if any vertex is outside the view frustum [-1, 1]
    if (!isInsideClipSpace(c1) || !isInsideClipSpace(c2) ||
        !isInsideClipSpace(c3))
      continue;

    // LAB 2 Task 2.2: Viewport Transform
    // Convert from Clip Space [-1, 1] to Screen Space [0, width/height]
    // Formula: screen = (clip + 1) * 0.5 * viewport_size
    int sx1 = (c1.x + 1.0f) * 0.5f * (framebuffer->width - 1);
    int sy1 = (c1.y + 1.0f) * 0.5f * (framebuffer->height - 1);
    int sx2 = (c2.x + 1.0f) * 0.5f * (framebuffer->width - 1);
    int sy2 = (c2.y + 1.0f) * 0.5f * (framebuffer->height - 1);
    int sx3 = (c3.x + 1.0f) * 0.5f * (framebuffer->width - 1);
    int sy3 = (c3.y + 1.0f) * 0.5f * (framebuffer->height - 1);

    // Store screen coordinates and Z depth
    Vector3 sp0((float)sx1, (float)sy1, c1.z);
    Vector3 sp1((float)sx2, (float)sy2, c2.z);
    Vector3 sp2((float)sx3, (float)sy3, c3.z);

    // interactivity
    if (mode == eRenderMode::POINTCLOUD) {
      framebuffer->SetPixel((int)sp0.x, (int)sp0.y, c);
      framebuffer->SetPixel((int)sp1.x, (int)sp1.y, c);
      framebuffer->SetPixel((int)sp2.x, (int)sp2.y, c);
      continue;
    }

    if (mode == eRenderMode::WIREFRAME) {
      framebuffer->DrawLineDDA((int)sp0.x, (int)sp0.y, (int)sp1.x, (int)sp1.y,
                               c);
      framebuffer->DrawLineDDA((int)sp1.x, (int)sp1.y, (int)sp2.x, (int)sp2.y,
                               c);
      framebuffer->DrawLineDDA((int)sp2.x, (int)sp2.y, (int)sp0.x, (int)sp0.y,
                               c);
      continue;
    }

    //
    // LAB 3 Task 3.1: Rasterize Triangles (Filled)
    // If the mode is TRIANGLES, we use the interpolated drawing function
    if (mode == eRenderMode::TRIANGLES) {
      // Draw plain color triangle (no texture, no interpolated color)
      framebuffer->DrawTriangleInterpolated(sp0, sp1, sp2, c, c, c, zbuffer);
      continue;
    }

    // LAB 3 Task 3.4: Texture Mapping Prep
    // We pack all necessary data into sTriangleInfo struct to pass to the
    // rasterizer
    Image::sTriangleInfo tri;
    tri.p0 = sp0;
    tri.p1 = sp1;
    tri.p2 = sp2;
    tri.texture = nullptr;

    // Default: plain color per vertex
    tri.c0 = c;
    tri.c1 = c;
    tri.c2 = c;
    tri.uv0 = Vector2(0, 0);
    tri.uv1 = Vector2(0, 0);
    tri.uv2 = Vector2(0, 0);

    const std::vector<Vector2> &uvs = mesh->GetUVs();
    bool has_uvs = (uvs.size() == mesh->GetVertices().size());

    if (use_mesh_texture) { // T = texture
      if (use_interpolated_uvs && texture && texture->pixels &&
          has_uvs) { // C = UVs interpoladas
        tri.texture = texture;
        tri.uv0 = uvs[i];
        tri.uv1 = uvs[i + 1];
        tri.uv2 = uvs[i + 2];
      }
      // else: se queda color plano (PLAIN COLOR)
    } else { // T = color por vértice
      tri.c0 = Color::RED;
      tri.c1 = Color::GREEN;
      tri.c2 = Color::BLUE;
    }

    // LAB 3 Task 3.1 & 3.4: Call the full rasterizer with UVs/Texture info
    framebuffer->DrawTriangleInterpolated(tri, zbuffer);
  }
}

// GPU rendering: renders the entity's mesh using its shader ("material")
void Entity::Render(sUniformData &uniformData) {
  if (!mesh || !material)
    return;

  // Ejercicio 1.2 - Sube la matriz modelo actualizada al uniformData
  uniformData.model_matrix = model;

  // Habilitar el material
  material->Enable(uniformData);

  // Render the mesh on the GPU
  mesh->Render(GL_TRIANGLES);

  // Deshabilitar el material
  material->Disable();
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
  T.MakeTranslationMatrix(base_position.x + offset, base_position.y,
                          base_position.z);

  R.MakeRotationMatrix(angle, rot_axis);

  S.MakeScaleMatrix(scale, scale, scale);

  model = T * R * S;
}
