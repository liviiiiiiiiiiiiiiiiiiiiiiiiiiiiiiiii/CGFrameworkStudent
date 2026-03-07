#include "material.h"

Material::Material() {
  // Valores por defecto sugeridos
  Ka = Vector3(0.2f, 0.2f, 0.2f);
  Kd = Vector3(0.8f, 0.8f, 0.8f);
  Ks = Vector3(0.5f, 0.5f, 0.5f);
  shininess = 30.0f;

  use_color_texture = false;
  use_specular_texture = false;
  use_normal_texture = false;
}

Material::~Material() {
  // No borramos las texturas ni shaders aquí ya que pueden estar compartidos
}

void Material::Enable(const sUniformData &uniformData)
{
  if (!shader)
    return;

  shader->Enable();

  // Matrices
  shader->SetMatrix44("u_model", uniformData.model_matrix);
  shader->SetMatrix44("u_viewprojection", uniformData.viewprojection_matrix);

  // Camera and lights
  shader->SetUniform3("u_camera_position", uniformData.camera_position);
  shader->SetUniform3("u_ambient_light", uniformData.ambient_light);
  shader->SetUniform3("u_light_position", uniformData.current_light.position);
  shader->SetUniform3("u_light_color", uniformData.current_light.color);

  // Material
  shader->SetUniform3("u_ka", Ka);
  shader->SetUniform3("u_kd", Kd);
  shader->SetUniform3("u_ks", Ks);
  shader->SetUniform1("u_shininess", shininess);

  // Texture flags
  shader->SetUniform1("u_use_color_texture", (color_texture && use_color_texture) ? 1 : 0);
  shader->SetUniform1("u_use_normal_texture", (normal_texture && use_normal_texture) ? 1 : 0);
  shader->SetUniform1("u_use_specular_texture", (specular_texture && use_specular_texture) ? 1 : 0);
  // Textures
  if (color_texture && use_color_texture)
    shader->SetTexture("u_color_texture", color_texture);

  if (normal_texture && use_normal_texture)
    shader->SetTexture("u_normal_texture", normal_texture);


  if (specular_texture && use_specular_texture)
    shader->SetTexture("u_specular_texture", specular_texture);
    
}

void Material::Disable(){

  if (!shader)
    return;

  shader->Disable();
}