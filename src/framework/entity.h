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

  // Constructor and destructor
  Entity();
  ~Entity();

  // Setters
  void SetMesh(Mesh *m);
  void SetModelMatrix(const Matrix44 &mat);

  // Getters
  Mesh *GetMesh() const;
  Matrix44 GetModelMatrix() const;
  void Render(Image *framebuffer, Camera *camera, const Color &c);
};

#endif // ENTITY_H