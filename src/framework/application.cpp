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

  //lab 4
  quad_mesh = new Mesh();
  quad_mesh->CreateQuad(); // generates 2 triangles covering clip-space [-1..1]

  quad_shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
  if (!quad_shader)
{
    std::cout << "ERROR: quad_shader is NULL (shader load/compile failed)\n";
    exit(1);
}
}


// Init UI
void Application::InitUI(void) {
  // No UI required for Lab 3
}

void Application::Render(void) {

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear screen

    glDisable(GL_DEPTH_TEST); 

    quad_shader->Enable();

    quad_shader->SetUniform1("u_mode", formula_mode);
    quad_shader->SetUniform1("u_aspect", window_width / (float)window_height); //send values from CPU to GPU

    quad_mesh->Render(GL_TRIANGLES);

    quad_shader->Disable();
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

    case 'a': formula_mode = 0; break; // a
    case 'b': formula_mode = 1; break; // b
    case 'c': formula_mode = 2; break; // c
    case 'd': formula_mode = 3; break; // d
    case 'e': formula_mode = 4; break; // e
    case 'f': formula_mode = 5; break; // f
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
