#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"

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
}

Application::~Application() {}

void Application::Init(void) { std::cout << "Initiating app..." << std::endl; }

// Render one frame
void Application::Render(void) {
  // ...
  framebuffer.SetPixel(0, 0, Color::GREEN);
  // Definimos los tres puntos del triángulo
  Vector2 p0(400, 150); // Arriba al centro
  Vector2 p1(150, 450); // Abajo a la izquierda
  Vector2 p2(650, 400); // Abajo a la derecha (un poco más arriba que p1)

  // Definimos los colores
  Color border = Color::WHITE;
  Color fill = Color::BLUE;

  // Llamada a tu función
  framebuffer.DrawTriangle(p0, p1, p2, border, true, fill);

  // Test DrawImage (Load once using static)
  static Image testImg;
  if (testImg.width == 0)
    testImg.LoadPNG("images/pencil.png");
  if (testImg.width > 0)
    framebuffer.DrawImage(testImg, 50, 50);

  framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed) {}

// keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event) {
  // KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
  switch (event.keysym.sym) {
  case SDLK_ESCAPE:
    exit(0);
    break; // ESC key, kill the app
  }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
  }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
  }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event) {}

void Application::OnWheel(SDL_MouseWheelEvent event) {
  float dy = event.preciseY;

  // ...
}

void Application::OnFileChanged(const char *filename) {
  Shader::ReloadSingleShader(filename);
}