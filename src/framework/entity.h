#ifndef ENTITY_H
#define ENTITY_H

#include "camera.h"
#include "framework.h"
#include "image.h"
#include "mesh.h"

class Entity {
public:
  // The mesh to render
  Mesh *mesh;

  // Model matrix for transformations (scale, rotation, translation)
  Matrix44 model;

  // Update variables for animation
  Vector3 base_position;
  Vector3 rot_axis;
  float rotation_speed;
  float scale;
  float time_acc;
  float phase;

  // Render properties
  Image *texture = nullptr; // Pointer to the texture image

  // Constructor and destructor
  Entity();
  ~Entity();

  // Setters
  void SetMesh(Mesh *m);
  void SetModelMatrix(const Matrix44 &mat);

  // Getters
  Mesh *GetMesh() const;
  Matrix44 GetModelMatrix() const;
  void Render(Image *framebuffer, Camera *camera, FloatImage *zbuffer,
              const Color &c);
  void Update(float seconds_elapsed);
};

#endif // ENTITY_H