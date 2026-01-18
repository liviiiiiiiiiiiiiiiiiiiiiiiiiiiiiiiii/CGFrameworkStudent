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

Application::~Application() {
  ActiveTool = ButtonType::Line;
  isDrawingLine = false;
}

void Application::Init(void) {
  std::cout << "Initiating app..." << std::endl;
  // define and load image for line button
  Image *lineImg = new Image();
  lineImg->LoadPNG("images/line.png");

  // define line button
  lineButton = Button(lineImg, 5, 5, ButtonType::Line);
}

// Init UI
void Application::InitUI(void) {
  // Draw background bar
  framebuffer.DrawRect(0, 0, window_width, 50, Color::GRAY, 1.f, true,
                       Color::GRAY);
  lineButton.Draw(framebuffer);
}

// Render one frame
void Application::Render(void) {

  InitUI();
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
    // Comprobar si clickeaste el botón de línea
    if (lineButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::Line; // Activar herramienta línea
      std::cout << "Herramienta LÍNEA activada" << std::endl;
      return; // No hacer nada más
    }

    // Si no clickeaste ningún botón, entonces estás dibujando
    if (mouse_position.y > 50) { // Fuera de la barra de herramientas
      if (ActiveTool == ButtonType::Line) {
        if (!isDrawingLine) {
          // Primer click: guardar punto inicial
          lineStartingPoint = mouse_position;
          isDrawingLine = true;
          std::cout << "Punto inicial: (" << mouse_position.x << ", "
                    << mouse_position.y << ")" << std::endl;
        } else {
          // Segundo click: dibujar la línea
          framebuffer.DrawLineDDA(lineStartingPoint.x, lineStartingPoint.y,
                                  mouse_position.x, mouse_position.y,
                                  Color::WHITE); // O el color que quieras
          isDrawingLine = false;
          std::cout << "Línea dibujada hasta: (" << mouse_position.x << ", "
                    << mouse_position.y << ")" << std::endl;
        }
      }
    }
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