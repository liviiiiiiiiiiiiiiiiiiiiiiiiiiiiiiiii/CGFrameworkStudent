/*
        + This class encapsulates the application, is in charge of creating the
   data, getting the user input, process the update and render.
*/

#pragma once

#include "button.h"
#include "camera.h"
#include "entity.h"
#include "framework.h"
#include "image.h"
#include "main/includes.h"
#include "material.h" // Añadido para Lab 5
#include "shader.h"
#include "texture.h"

class Application {
public:
  // Window

  SDL_Window *window = nullptr;
  int window_width;
  int window_height;

  float time;

  // Input
  const Uint8 *keystate;
  int mouse_state;        // Tells which buttons are pressed
  Vector2 mouse_position; // Last mouse position
  Vector2 mouse_delta;    // Mouse movement in the last frame

  void OnKeyPressed(SDL_KeyboardEvent event);
  void OnMouseButtonDown(SDL_MouseButtonEvent event);
  void OnMouseButtonUp(SDL_MouseButtonEvent event);
  void OnMouseMove(SDL_MouseButtonEvent event);
  void OnWheel(SDL_MouseWheelEvent event);
  void OnFileChanged(const char *filename);

  // CPU Global framebuffer
  Image framebuffer;

  // Constructor and main methods
  Application(const char *caption, int width, int height);
  ~Application();

  // Buttons
  Button lineButton;
  Button triangleButton;
  Button rectangleButton;

  Button pencilButton;
  Button eraserButton;
  Button clearButton;
  Button saveButton;
  Button loadButton;

  // Color buttons
  Button whiteColorButton;
  Button blackColorButton;
  Button redColorButton;
  Button greenColorButton;
  Button blueColorButton;
  Button yellowColorButton;
  Button pinkColorButton;
  Button cyanColorButton;

  // drawing state
  ButtonType ActiveTool;
  bool isDrawing;         // Are we drawing something?
  Vector2 drawStartPoint; // Initial point (for line, rectangle, etc.)

  // Triangle-specific state (needs 3 points)
  int triangleClickCount; // 0, 1, or 2 (how many points we have)
  Vector2 trianglePoint1;
  Vector2 trianglePoint2;

  // Drawing properties
  int borderWidth;            // Border thickness for rectangles
  Color currentColor;         // Current drawing color (border)
  Color fillColor;            // Current fill color
  bool isFilled;              // Whether shapes should be filled
  Vector2 lastPencilPosition; // Last position when drawing with the pencil

  // UI functions
  void InitUI();

  void Init(void);
  void Render(void);
  void Update(float dt);

  Entity *entity;
  Camera *camera;
  int lab_mode =
      0; // 0: no entity, 1: single entity, 2: multiple animated entities
  std::vector<Entity *> entities;
  Mesh *shared_mesh = nullptr;

  // Lights
  std::vector<sLight> lights;
  int num_active_lights = 1;

  enum CameraProp { PROP_NONE, PROP_NEAR, PROP_FAR, PROP_FOV };
  CameraProp current_prop = PROP_NONE;

  FloatImage zbuffer;

  Entity::eRenderMode render_mode = Entity::eRenderMode::TRIANGLES_INTERPOLATED;

  bool use_mesh_texture = true;     // T
  bool use_occlusions = true;       // Z
  bool use_interpolated_uvs = true; // C
  bool render_wireframe = false;    // W

  // lab 4
  Mesh *quad_mesh = nullptr;
  Shader *quad_shader = nullptr;
  Texture *quad_texture = nullptr;

  int formula_mode = 0;
  int subtask_mode = 0;
  bool show_image_filters = false;

  // lab 5
  Material *material = nullptr; // TODO: Puedes tener un material para Gouraud y otro para Phong,
               // o actualizar el shader del material actual

  // TODO: Variable para guardar todos los uniforms que vayamos a pasar en el
  // lab 5
  sUniformData uniform_data;

  Shader* gouraud_shader = nullptr;
  Shader* phong_shader = nullptr;

  // Add initialization method
  void InitParticleButton();


  // Other methods to control the app
  void SetWindowSize(int width, int height) {
    glViewport(0, 0, width, height);
    this->window_width = width;
    this->window_height = height;
    this->framebuffer.Resize(width, height);
    this->zbuffer.Resize(width, height);
    this->zbuffer.Fill(1e9f);
    if (camera)
      camera->SetPerspective(camera->fov, width / (float)height,
                             camera->near_plane, camera->far_plane);
  }

  Vector2 GetWindowSize() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return Vector2(float(w), float(h));
  }
};
