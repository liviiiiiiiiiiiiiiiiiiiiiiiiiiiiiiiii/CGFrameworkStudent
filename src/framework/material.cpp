#include "material.h"

Material::Material() {
  // Valores por defecto sugeridos
  Ka = 0.2f;
  Kd = 0.8f;
  Ks = 0.5f;
  shininess = 30.0f;

  use_color_texture = false;
  use_specular_texture = false;
  use_normal_texture = false;
}

Material::~Material() {
  // No borramos las texturas ni shaders aquí ya que pueden estar compartidos
}

void Material::Enable(const sUniformData &uniformData) {
  if (!shader)
    return;

  // TODO: Ejercicio 1.1 y 1.2 - Habilitar el shader
  // shader->Enable();

  // TODO: Subir matrices
  // shader->SetMatrix44("u_model", uniformData.model_matrix);
  // shader->SetMatrix44("u_viewprojection", uniformData.viewprojection_matrix);

  // TODO: Subir posición de cámara
  // shader->SetVector3("u_camera_position", uniformData.camera_position);

  // TODO: Subir luz ambiente
  // shader->SetVector3("u_ambient_light", uniformData.ambient_light);

  // TODO: Subir luz actual (posición, color) desde uniformData.current_light
  // shader->SetVector3("u_light_position", uniformData.current_light.position);
  // shader->SetVector3("u_light_color", uniformData.current_light.color);

  // TODO: Subir las propiedades del material (Ka, Kd, Ks, shininess)
  // shader->SetFloat("u_ka", Ka);
  // ...

  // TODO: Ejercicio 1.5 - Subir texturas si están disponibles y usarlas según
  // los flags if (color_texture && use_color_texture)
  // shader->SetTexture("u_color_texture", color_texture, 0); // Slot 0
  // shader->SetUniform1("u_use_color_texture", use_color_texture ? 1 : 0);
  //
  // if (specular_texture && use_specular_texture)
  // shader->SetTexture("u_specular_texture", specular_texture, 1); // Slot 1
  // ...
  // if (normal_texture && use_normal_texture)
  // shader->SetTexture("u_normal_texture", normal_texture, 2); // Slot 2
  // ...
}

void Material::Disable() {
  if (!shader)
    return;

  // TODO: Deshabilitar shader
  // shader->Disable();
}
