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

void Application::Init(void){
    quad_mesh = new Mesh();
    quad_mesh->CreateQuad();

    quad_shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
    if (!quad_shader) {
        exit(1);
    }

    quad_texture = Texture::Get("images/fruits.png");


    // Camera
    camera = new Camera();
    camera->LookAt(Vector3(0.f, 0.f, 1.5f), Vector3(0.f, 0.f, 0.f), Vector3::UP);
    camera->SetPerspective(45.f, window_width / (float)window_height, 0.01f, 100.f);

  // Meshes
  Mesh* anna_mesh = new Mesh();
  anna_mesh->LoadOBJ("meshes/anna.obj");

  Mesh* lee_mesh = new Mesh();
  lee_mesh->LoadOBJ("meshes/lee.obj");

  Mesh* cleo_mesh = new Mesh();
  cleo_mesh->LoadOBJ("meshes/cleo.obj");

    // Shaders
    gouraud_shader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
    if (!gouraud_shader) {
        exit(1);
    }

    phong_shader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
    if (!phong_shader) {
        exit(1);
    }

    // Textures
    Texture* lee_color   = Texture::Get("textures/lee_color_specular.tga");
    Texture* lee_normal  = Texture::Get("textures/lee_normal.tga");

    Texture* anna_color  = Texture::Get("textures/anna_color_specular.tga");
    Texture* anna_normal = Texture::Get("textures/anna_normal.tga");

    Texture* cleo_color  = Texture::Get("textures/cleo_color_specular.tga");
    Texture* cleo_normal = Texture::Get("textures/cleo_normal.tga");

    //  Materials 

    Material* anna_material = new Material();
    anna_material->shader = phong_shader;
    anna_material->color_texture = anna_color;
    anna_material->specular_texture = anna_color;
    anna_material->normal_texture = anna_normal;
    anna_material->use_color_texture = true;
    anna_material->use_normal_texture = true;
    anna_material->use_specular_texture = true;
    anna_material->Ka = Vector3(0.24725f, 0.1995f, 0.0745f);
    anna_material->Kd = Vector3(0.75164f, 0.60648f, 0.22648f);
    anna_material->Ks = Vector3(0.628281f, 0.555802f, 0.366065f);
    anna_material->shininess = 15.0f;

    Material* lee_material = new Material();
    lee_material->shader = phong_shader;
    lee_material->color_texture = lee_color;
    lee_material->specular_texture = lee_color;
    lee_material->normal_texture = lee_normal;
    lee_material->use_color_texture = true;
    lee_material->use_normal_texture = true;
    lee_material->use_specular_texture = true;
    lee_material->Ka = Vector3(0.24725f, 0.1995f, 0.0745f);
    lee_material->Kd = Vector3(0.75164f, 0.60648f, 0.22648f);
    lee_material->Ks = Vector3(0.628281f, 0.555802f, 0.366065f);
    lee_material->shininess = 32.0f;

    Material* cleo_material = new Material();
    cleo_material->shader = phong_shader;
    cleo_material->color_texture = cleo_color;
    cleo_material->specular_texture = cleo_color;
    cleo_material->normal_texture = cleo_normal;
    cleo_material->use_color_texture = true;
    cleo_material->use_normal_texture = true;
    cleo_material->use_specular_texture = true;
    cleo_material->Ka = Vector3(0.24725f, 0.1995f, 0.0745f);
    cleo_material->Kd = Vector3(0.75164f, 0.60648f, 0.22648f);
    cleo_material->Ks = Vector3(0.628281f, 0.555802f, 0.366065f);
    cleo_material->shininess = 32.0f;
    //  Lights 
    lights.clear();

    sLight light1;
    light1.position = Vector3(1.f, 1.5f, 1.5f);
    light1.color = Vector3(3.f, 3.f, 3.f);
    lights.push_back(light1);

    sLight light2;
    light2.position = Vector3(-1.f, 1.0f, 1.0f);
    light2.color = Vector3(0.f, 2.f, 0.f);
    lights.push_back(light2);

    sLight light3;
    light3.position = Vector3(0.f, -1.0f, 1.0f);
    light3.color = Vector3(2.f, 0.f, 0.f);
    lights.push_back(light3);

    sLight light4;
    light4.position = Vector3(0.f, 1.0f, -1.0f);
    light4.color = Vector3(0.f, 0.f, 2.f);
    lights.push_back(light4);

    //  Entities 
    entities.clear();

  Entity* e1 = new Entity();
  e1->SetMesh(anna_mesh);
  e1->material = anna_material;
  Matrix44 m1;
  m1.MakeTranslationMatrix(-0.8f, 0.0f, 0.0f);
  e1->SetModelMatrix(m1);
  entities.push_back(e1);

  Entity* e2 = new Entity();
  e2->SetMesh(lee_mesh);
  e2->material = lee_material;
  Matrix44 m2;
  m2.MakeTranslationMatrix(0.0f, 0.0f, 0.0f);
  e2->SetModelMatrix(m2);
  entities.push_back(e2);

  Entity* e3 = new Entity();
  e3->SetMesh(cleo_mesh);
  e3->material = cleo_material;
  Matrix44 m3;
  m3.MakeTranslationMatrix(0.8f, 0.0f, 0.0f);
  e3->SetModelMatrix(m3);
  entities.push_back(e3);

    lab_mode = 5;
}

void Application::Render(void)
{
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //  LAB 5
  if (lab_mode == 5)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if (!camera || entities.empty() || lights.empty()) {
      glDisable(GL_DEPTH_TEST);
      return;
    }

    // Common scene uniforms
    uniform_data.viewprojection_matrix = camera->GetViewProjectionMatrix();
    uniform_data.camera_position = camera->eye;

    int lights_to_render = num_active_lights;
    if (lights_to_render > (int)lights.size())
      lights_to_render = (int)lights.size();

    if (lights_to_render <= 0) {
      glDisable(GL_DEPTH_TEST);
      return;
    }

    // ambient + first light
    uniform_data.ambient_light = Vector3(0.1f, 0.1f, 0.1f);
    uniform_data.current_light = lights[0];

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    for (Entity* e : entities)
      e->Render(uniform_data);

    // add remaining lights
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);

    for (int i = 1; i < lights_to_render; ++i) {
      uniform_data.current_light = lights[i];
      uniform_data.ambient_light = Vector3(0.f, 0.f, 0.f); // ambient only once

      for (Entity* e : entities)
        e->Render(uniform_data);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    return;
  }

  // LAB 4
  glDisable(GL_DEPTH_TEST);

  if (!quad_shader || !quad_mesh)
    return;

  quad_shader->Enable();

  int u_mode_to_send = subtask_mode;
  if (formula_mode == 3)
    u_mode_to_send = 6 + (subtask_mode % 2);

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

    // Switch between Lab 4 / 5
    case 'l':
    case 'L':
      if (lab_mode == 4)
        lab_mode = 5;
      else
        lab_mode = 4;
      break;

    // LAB 5: shader selection
    // Gouraud computes illumination per vertex.
    // Phong computes illumination per fragment.
    case 'g':
    case 'G':
      if (lab_mode == 5 && gouraud_shader) {
        for (Entity* e : entities)
          if (e && e->material)
            e->material->shader = gouraud_shader;
      }
      break;

    case 'p':
    case 'P':
      if (lab_mode == 5 && phong_shader) {
        for (Entity* e : entities)
          if (e && e->material)
            e->material->shader = phong_shader;
      }
      break;


    // Keys shared by Lab 4 and Lab 5
    // -------------------------
    case '1':
      if (lab_mode == 4)
      {
        formula_mode = 1; // patterns
        show_image_filters = false;
      }
      else
      {
        num_active_lights = 1;
      }
      break;

    case '2':
      if (lab_mode == 4)
      {
        formula_mode = 2; // filters
        show_image_filters = true;
      }
      else
      {
        num_active_lights = 2;
      }
      break;

    case '3':
      if (lab_mode == 4)
      {
        formula_mode = 3; // animations
        show_image_filters = true;
      }
      else
      {
        num_active_lights = 3;
      }
      break;

    case '4':
      if (lab_mode == 5)
        num_active_lights = 4;
      break;

    case 'a':
    case 'A':
      if (lab_mode == 4)
        subtask_mode = 0;
      break;

    case 'b':
    case 'B':
      if (lab_mode == 4)
        subtask_mode = 1;
      break;

    case 'd':
    case 'D':
      if (lab_mode == 4)
        subtask_mode = 3;
      break;

    case 'e':
    case 'E':
      if (lab_mode == 4)
        subtask_mode = 4;
      break;

    case 'f':
    case 'F':
      if (lab_mode == 4)
        subtask_mode = 5;
      break;

    // LAB 5: texture toggles
  case 'c':
  case 'C':
    if (lab_mode == 4)
      subtask_mode = 2;
    else {
      for (Entity* e : entities) {
        if (e && e->material)
          e->material->use_color_texture = !e->material->use_color_texture;
      }
    }
    break;

  case 's':
  case 'S':
    if (lab_mode == 5) {
      for (Entity* e : entities) {
        if (e && e->material)
          e->material->use_specular_texture = !e->material->use_specular_texture;
      }
    }
    break;

  case 'n':
  case 'N':
    if (lab_mode == 5) {
      for (Entity* e : entities) {
        if (e && e->material)
          e->material->use_normal_texture = !e->material->use_normal_texture;
      }
    }
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
