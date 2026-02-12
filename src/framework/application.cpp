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

  // Particle system
  this->showParticles = false;
  this->pS.Init(width, height);
  this->lab_mode = 2; // Start with one entity mode
  this->render_wireframe = false;
}

Application::~Application() {
  ActiveTool = ButtonType::LINE;
  isDrawing = false;
}

void Application::Init(void) {
  std::cout << "Initiating app..." << std::endl;

  // Init Camera
  camera = new Camera();
  camera->LookAt(Vector3(0.f, 0.f, 1.5f), Vector3(0.f, 0.f, 0.f), Vector3::UP);
  camera->SetPerspective(45.f, window_width / (float)window_height, 0.3f, 45.f); // Degrees for gluPerspective

  shared_mesh = new Mesh();
  shared_mesh->LoadOBJ("meshes/lee.obj");

  // LAB 3. Task 3.4: Color the mesh using a texture
  // Load Texture. We need to create a new Image object and load the TGA file.
  Image *texture = new Image();
  // We pass 'true' to flip_y because textures in OpenGL/Graphics are usually
  // stored inverted relative to the screen coordinates (bottom-left vs
  // top-left), solving upside-down textures. This is explicitly mentioned in
  // the lab slides.
  texture->LoadTGA("textures/lee_color_specular.tga", true);

  entities.clear();

  // --- Load 3 meshes ---
  Mesh *mesh_lee = new Mesh();
  mesh_lee->LoadOBJ("meshes/lee.obj");
  Mesh *mesh_anna = new Mesh();
  mesh_anna->LoadOBJ("meshes/anna.obj");
  Mesh *mesh_cleo = new Mesh();
  mesh_cleo->LoadOBJ("meshes/cleo.obj");

  // --- Load 3 textures (match each mesh) ---
  Image *tex_lee = new Image();
  tex_lee->LoadTGA("textures/lee_color_specular.tga", true);
  Image *tex_anna = new Image();
  tex_anna->LoadTGA("textures/anna_color_specular.tga", true);
  Image *tex_cleo = new Image();
  tex_cleo->LoadTGA("textures/cleo_color_specular.tga", true);

  entities.clear();

  // multiple entitites (lee, anna, cleo)
  for (int i = 0; i < 3; ++i) {
    Entity *e = new Entity();
    if (i == 0) {
      e->SetMesh(mesh_lee);
      e->texture = tex_lee;
    }
    if (i == 1) {
      e->SetMesh(mesh_anna);
      e->texture = tex_anna;
    }
    if (i == 2) {
      e->SetMesh(mesh_cleo);
      e->texture = tex_cleo;
    }

    e->base_position = Vector3(-0.6f + i * 0.6f, 0.f, 0.f);
    e->rot_axis = Vector3::UP;
    e->scale = 0.6f + 0.2f * i;
    e->phase = i * 1.0f;

    e->Update(0.0f);
    entities.push_back(e);
  }

  // single entity (lee)
  entity = new Entity();
  entity->SetMesh(mesh_lee);
  entity->texture = tex_lee;
  Matrix44 model;
  model.SetIdentity();
  entity->SetModelMatrix(model);
}

// Init UI
void Application::InitUI(void) {
  // No UI required for Lab 3
}

void Application::Render(void) {
  framebuffer.SetPixel(0, 0, Color::GREEN);
  // Clear the framebuffer first (black or dark gray)
  framebuffer.Fill(Color(40, 40, 40));

  // Z toggle
  // LAB 3 Task 3.3: Z-Buffer
  // We need to clear it every frame with a very large value (infinity)
  FloatImage *zb = nullptr;
  if (use_occlusions) {
    zbuffer.Fill(1e9f);
    zb = &zbuffer;
  }

  Entity::eRenderMode current_render_mode;
  if (render_wireframe) {
    current_render_mode = Entity::eRenderMode::WIREFRAME;
  } else {
    current_render_mode = Entity::eRenderMode::TRIANGLES_INTERPOLATED;
  }

  if (entity) {
    entity->use_mesh_texture = use_mesh_texture;
    entity->use_interpolated_uvs = use_interpolated_uvs;
    entity->mode = current_render_mode;
  }

  for (int i = 0; i < (int)entities.size(); ++i) {
    entities[i]->use_mesh_texture = use_mesh_texture;
    entities[i]->use_interpolated_uvs = use_interpolated_uvs;
    entities[i]->mode = current_render_mode;
  }

  // render
  if (lab_mode == 1 && entity) {
    entity->Render(&framebuffer, camera, zb, Color::BLUE);
  } else if (lab_mode == 2) {
    for (int i = 0; i < (int)entities.size(); ++i) {
      entities[i]->Render(&framebuffer, camera, zb, Color::BLUE);
    }
  }

  framebuffer.Render();
}

void Application::Update(float seconds_elapsed) {
  if (showParticles) {
    pS.Update(seconds_elapsed, window_width, window_height);
    framebuffer.Fill(Color::BLACK);
  }
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
  if ((mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)) && lab_mode != 0 && mouse_position.y > 50) {
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
    break; // ESC key, kill the app

  case '1':
    lab_mode = 1; // SINGLE ENTITY
    break;

  case '2':
    lab_mode = 2; // MULTIPLE ANIMATED ENTITIES
    break;

  case 'N':
  case 'n':
    current_prop = PROP_NEAR;
    break;

  case 'F':
  case 'f':
    current_prop = PROP_FAR;
    break;

  case 'V':
  case 'v':
    current_prop = PROP_FOV;
    break;

  case '+': {
    if (!camera)
      break;

    if (current_prop == PROP_NEAR) {      camera->near_plane += 1.0;
    } else if (current_prop == PROP_FAR)
      camera->far_plane += 1.0f;
    else if (current_prop == PROP_FOV)
      camera->fov += 1.0f;

    if (camera->near_plane < 0.001f)
      camera->near_plane = 0.001f;
    if (camera->far_plane <= camera->near_plane + 0.01f)
      camera->far_plane = camera->near_plane + 0.01f;
    if (camera->fov < 10.0f)
      camera->fov = 10.0f;
    if (camera->fov > 170.0f)
      camera->fov = 170.0f;

    camera->SetPerspective(camera->fov, window_width / (float)window_height, camera->near_plane, camera->far_plane);
    break;
  }

  case '-': {
    if (!camera)
      break;

    if (current_prop == PROP_NEAR)
      camera->near_plane -= 1.0f;
    else if (current_prop == PROP_FAR)
      camera->far_plane -= 1.0f;
    else if (current_prop == PROP_FOV)
      camera->fov -= 1.0f;

    if (camera->near_plane < 0.001f)
      camera->near_plane = 0.001f;
    if (camera->far_plane <= camera->near_plane + 0.01f)
      camera->far_plane = camera->near_plane + 0.01f;
    if (camera->fov < 10.0f)
      camera->fov = 10.0f;
    if (camera->fov > 170.0f)
      camera->fov = 170.0f;

    camera->SetPerspective(camera->fov, window_width / (float)window_height, camera->near_plane, camera->far_plane);
    break;
  }

  // LAB 3 Task 3.5: Interactivity
  // T: Toggle Mesh Texture
  case 'T':
  case 't':
    use_mesh_texture = !use_mesh_texture;
    break;

  // LAB 3 Task 3.5: Toggle Z-Buffer
  case 'Z':
  case 'z':
    use_occlusions = !use_occlusions;
    break;

  // LAB 3 Task 3.5: Toggle Interpolated UVs vs Plain Color
  case 'C':
  case 'c':
    use_interpolated_uvs = !use_interpolated_uvs;
    break;

  // LAB 3 Task 3.5: Toggle Wireframe (Extra)
  case 'W':
  case 'w':
    render_wireframe = !render_wireframe;
    break;
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