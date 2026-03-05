#include "application.h"
#include "entity.h"
#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include <iostream>
#include <vector>

Application::Application(const char *caption, int width, int height) {
  this->window = createWindow(caption, width, height);

  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  this->mouse_state = 0;
  this->time = 0.f;
  this->window_width = w;
  this->window_height = h;
  this->keystate = SDL_GetKeyboardState(nullptr);

  this->framebuffer.Resize(w, h);
  this->zbuffer.Resize(w, h);
  this->zbuffer.Fill(1e9f);

  // Initialize drawing state
  this->ActiveTool = ButtonType::LINE;
  this->isDrawing = false;
  this->borderWidth = 2;             // Default border width
  this->triangleClickCount = 0;      // No triangle points yet
  this->currentColor = Color::WHITE; // Default drawing color
  this->fillColor = Color::WHITE;    // D efault fill color
  this->isFilled = false;            // Shapes not filled by default

  this->lastPencilPosition = Vector2(0, 0);

  this->lab_mode = 2; // Start with one entity mode
  this->render_wireframe = false;
}

Application::~Application() {
  ActiveTool = ButtonType::LINE;
  isDrawing = false;
}

void Application::Init(void) {

  // lab 4
  quad_mesh = new Mesh();
  quad_mesh->CreateQuad(); // generates 2 triangles covering clip-space [-1..1]

  quad_shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
  if (!quad_shader) {
    std::cout << "ERROR: quad_shader is NULL (shader load/compile failed)\n";
    exit(1);
  }

  quad_texture = Texture::Get("images/fruits.png");

  // lab 5: GPU mesh rendering
  // raster_shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
  // if (!raster_shader) {
  //   std::cout << "ERROR: raster_shader is NULL (shader load/compile
  //   failed)\n"; exit(1);
  // }

  // TODO: Cargar los shaders creados para Lab 5 (Gouraud y Phong)
  // Shader* gouraud_shader = Shader::Get("shaders/gouraud.vs",
  // "shaders/gouraud.fs"); Shader* phong_shader =
  // Shader::Get("shaders/phong.vs", "shaders/phong.fs");

  // Init Camera
  camera = new Camera();
  camera->LookAt(Vector3(0.f, 0.f, 1.5f), Vector3(0.f, 0.f, 0.f), Vector3::UP);
  camera->SetPerspective(45.f, window_width / (float)window_height, 0.01f,
                         100.f);

  // Load mesh
  shared_mesh = new Mesh();
  shared_mesh->LoadOBJ("meshes/lee.obj");

  // Load GPU texture
  // entity_texture = Texture::Get("textures/lee_color_specular.tga");

  // TODO: Ejercicio 1.1 y 1.5 - Crear el material y asignarle el shader y
  // texturas correspondientes material = new Material(); material->shader = ...
  // material->color_texture = Texture::Get("textures/lee_color_specular.tga");
  // material->specular_texture = ...
  // material->normal_texture = ...

  // Create animated entities
  entities.clear();
  for (int i = 0; i < 3; ++i) {
    Entity *e = new Entity();
    e->SetMesh(shared_mesh);
    // e->shader = raster_shader; // Ahora usamos Material
    // e->gpu_texture = entity_texture; // Ahora usamos Material
    e->material = material;

    e->base_position = Vector3(-0.6f + i * 0.6f, 0.f, 0.f);
    e->rot_axis = Vector3::UP;
    e->rotation_speed = 0.8f + 0.4f * i;
    e->scale = 0.6f + 0.2f * i;
    e->phase = i * 1.0f;

    e->Update(0.0f);
    entities.push_back(e);
  }

  // Single static entity
  entity->SetMesh(shared_mesh);
  // entity->shader = raster_shader; // Ahora usamos Material
  // entity->gpu_texture = entity_texture; // Ahora usamos Material
  entity->material = material;
  Matrix44 model;
  model.SetIdentity();
  entity->SetModelMatrix(model);
}

// Init UI
void Application::InitUI(void) {
  // No UI required for Lab 3
}

void Application::Render(void) {

  glClearColor(0.f, 0.f, 0.f, 1.f);                   // clear color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth buffer

  // Lab 5: GPU 3D mesh rendering
  if (lab_mode == 5) {
    glEnable(GL_DEPTH_TEST); // Enable occlusions

    // TODO: Ejercicio 1.2 - Llenar uniform_data con valores de la cámara y luz
    // ambiente uniform_data.viewprojection_matrix =
    // camera->GetViewProjectionMatrix(); uniform_data.camera_position =
    // camera->eye; uniform_data.ambient_light = Vector3(0.1f, 0.1f, 0.1f);

    // TODO: Ejercicio 1.6 - Implementar MultiPass (Renderizar con varias luces)
    // Para cada luz en la escena:
    // ... configura la luz en uniform_data.current_light
    // Si es la segunda luz o más, configura el modo de mezcla de OpenGL:
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Pista: para sumar colores en
    // framebuffer Cambia comportamiento si es la primera luz

    // Render all entities usando uniform_data
    for (Entity *e : entities) {
      e->Render(uniform_data);
    }

    // Al final del multipass:
    // glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    return;
  }

  // Lab 4: Quad Shader (Tasks 2, 3, 4)
  glDisable(GL_DEPTH_TEST);
  quad_shader->Enable();

  // Task 3 starts at mode 6 in the shader
  int u_mode_to_send = subtask_mode;
  if (formula_mode == 3) {
    u_mode_to_send = 6 + (subtask_mode % 2); // 6 and 7 are the transforms
  }

  quad_shader->SetUniform1("u_mode", u_mode_to_send);
  quad_shader->SetUniform1("u_show_texture", show_image_filters ? 1 : 0);
  quad_shader->SetUniform1("u_aspect", window_width / (float)window_height);
  quad_shader->SetUniform1("u_time", time);

  if (quad_texture)
    quad_shader->SetTexture("u_texture", quad_texture);

  quad_mesh->Render(GL_TRIANGLES);
  quad_shader->Disable();
}

void Application::Update(float seconds_elapsed) {
  this->time += seconds_elapsed;
  if (lab_mode == 2) {
    for (Entity *e : entities) {
      e->Update(seconds_elapsed);
    }
  }

  // Camera orbit logic
  if ((mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) && lab_mode != 0 &&
      mouse_position.y > 50) {
    if (camera) {
      /*El objetivo de este update es actualizar el vector eye para que en el
      update matrix se calcule la nueva ViewMatrix con los valores nueos de
      donde estamos posicionando la camara con el mouse*/
      // Rotate around UP

      // Basically creating the orthonormal camera frame: Zc,Xc
      Vector3 vector_eye_center =
          camera->eye - camera->center; // distance from camera to the center
      Matrix44 R;
      R.MakeRotationMatrix(-mouse_delta.x * 0.01f,
                           Vector3::UP); // Rotation matrix to rotate the
                                         // around the vertical axis
      vector_eye_center = R * vector_eye_center;
      camera->eye = camera->center + vector_eye_center;

      // Rotate around Right
      Vector3 front = (camera->center - camera->eye).Normalize(); // Zc
      Vector3 right = front.Cross(camera->up).Normalize();        // Xc
      R.MakeRotationMatrix(-mouse_delta.y * 0.01f,
                           right); // get the angle that we are gonna rotate
      // get the new eye vector so that it is updated and can be used by
      // UpdateViewMatrix()
      vector_eye_center = R * (camera->eye - camera->center);
      camera->eye = camera->center + vector_eye_center;

      camera->UpdateViewMatrix();
    }
  }
  if ((mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) && lab_mode != 0 &&
      mouse_position.y > 50) {
    if (camera) {
      // Rotate around UP (Yaw)
      Vector3 vector_eye_center = camera->eye - camera->center;
      Matrix44 R;
      R.MakeRotationMatrix(-mouse_delta.x * 0.01f, Vector3::UP);
      vector_eye_center = R * vector_eye_center;
      camera->eye = camera->center + vector_eye_center;

      camera->UpdateViewMatrix();
    }
  }
}

// keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event) {
  // KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
  switch (event.keysym.sym) {
  case SDLK_ESCAPE:
    exit(0);
    break;

  case '1':
    formula_mode = 1; // Task 1: Patterns
    show_image_filters = false;
    break;
  case '2':
    formula_mode = 2; // Task 2: Filters
    show_image_filters = true;
    break;
  case '3':
    formula_mode = 3; // Task 3: Animations
    show_image_filters = true;
    break;

  case 'a':
    subtask_mode = 0;
    break;
  case 'b':
    subtask_mode = 1;
    break;
  case 'c':
    subtask_mode = 2;
    break;
  case 'd':
    subtask_mode = 3;
    break;
  case 'e':
    subtask_mode = 4;
    break;
  case 'f':
    subtask_mode = 5;
    break;

  case 'l':
  case 'L':
    if (lab_mode != 5)
      lab_mode = 5;
    else
      lab_mode = 2;
    break;

    // TODO: 2 Interactivity - Añadir control de teclas
    // case 'p': case 'P': // Cambiar a shader de Phong
    // case 'g': case 'G': // Cambiar a shader de Gouraud
    // case 'c': case 'C': // Activar/Desactivar texturas de color
    // (material->use_color_texture) case 's': case 'S': // Activar/Desactivar
    // texturas especulares case 'n': case 'N': // Activar/Desactivar texturas
    // de normales case '1': case '2': ... // Cambiar número de luces a pintar
  }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
  // Camera orbit logic happens in Update() based on mouse_state
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {}

void Application::OnMouseMove(SDL_MouseButtonEvent event) {}

void Application::OnWheel(SDL_MouseWheelEvent event) {
  if (!camera)
    return;
  float dy = event.preciseY;
  Vector3 v = camera->eye - camera->center;
  float dist = v.Length();
  if (dist < 0.0001f)
    dist = 0.0001f;
  Vector3 dir = v / dist;
  float zoom = 1.0f - dy * 0.1f;
  if (zoom < 0.1f)
    zoom = 0.1f;
  dist *= zoom;

  // ADD MIN
  float min = 0.05f;
  float max = 50.0f;
  if (dist < min)
    dist = min;
  if (dist > max)
    dist = max;

  Vector3 new_eye = camera->center + dir * dist;
  camera->LookAt(new_eye, camera->center, Vector3::UP);
  // ...
}

void Application::OnFileChanged(const char *filename) {
  Shader::ReloadSingleShader(filename);
}
