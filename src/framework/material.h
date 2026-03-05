#pragma once

#include "framework.h"
#include "shader.h"
#include "texture.h"

// TODO: Ejercicio 1.1 - Define la estructura de la luz (posición, intensidad de
// color/color, etc.)
struct sLight {
  Vector3 position;
  Vector3 color;
  // ...
};

// TODO: Ejercicio 1.2 - Añade a esta estructura todas las variables uniformes
// que necesitas pasar al shader (matrices, luz de ambiente, luces de la escena,
// posición de la cámara, etc.) Nota: La matriz del modelo la puedes añadir aquí
// y actualizarla en Entity::Render()
struct sUniformData {
  Matrix44 viewprojection_matrix;
  Matrix44 model_matrix;
  Vector3 camera_position;
  Vector3 ambient_light;

  // Para multipass o si pasas una luz concreta en cada pasada
  sLight current_light;

  // ... otros uniforms
};

class Material {
public:
  Shader *shader = nullptr;

  // Propiedades del material según figura 1 y Ejercicio 1.5
  Texture *color_texture = nullptr;
  Texture *specular_texture = nullptr;
  Texture *normal_texture = nullptr;

  float Ka;        // ambient factor
  float Kd;        // diffuse factor
  float Ks;        // specular factor
  float shininess; // alpha

  // Switches para activar/desactivar texturas desde el teclado (Lab 5, sección
  // 2)
  bool use_color_texture = false;
  bool use_specular_texture = false;
  bool use_normal_texture = false;

  Material();
  ~Material();

  // TODO: Ejercicios 1.1 y 1.2 - Declaración de Enable y Disable
  void Enable(const sUniformData &uniformData);
  void Disable();
};
