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

  // Initialize drawing state
  this->ActiveTool = ButtonType::Line;
  this->isDrawing = false;
  this->borderWidth = 2; // Default border width
}

Application::~Application() {
  ActiveTool = ButtonType::Line;
  isDrawing = false;
}

void Application::Init(void) {
  std::cout << "Initiating app..." << std::endl;
  // define and load image for line button. Then create button
  Image *lineImg = new Image();
  lineImg->LoadPNG("images/line.png");
  lineButton = Button(lineImg, 5, 5, ButtonType::Line);
  Image *RectangleImg = new Image();
  RectangleImg->LoadPNG("images/rectangle.png");
  rectangleButton = Button(RectangleImg, 40, 5, ButtonType::Rectangle);
}

// Init UI
void Application::InitUI(void) {
  // Draw buttons
  lineButton.Draw(framebuffer);
  rectangleButton.Draw(framebuffer);
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

  case '=': // = key (also + with shift)
  case '+': // + key
    borderWidth++;
    if (borderWidth > 20)
      borderWidth = 20; // Max limit
    std::cout << "Border width: " << borderWidth << std::endl;
    break;

  case '-': // - key
  case '_': // _ key
    borderWidth--;
    if (borderWidth < 1)
      borderWidth = 1; // Min limit
    std::cout << "Border width: " << borderWidth << std::endl;
    break;
  }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
    // check click on buttons
    if (lineButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::Line;
      isDrawing = false; // Reset state
      std::cout << "Line tool activated" << std::endl;
      return;
    }

    if (rectangleButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::Rectangle;
      isDrawing = false; // Reset state
      std::cout << "Rectangle tool activated" << std::endl;
      return;
    }

    // If clicked outside the toolbar
    if (mouse_position.y > 50) {
      if (ActiveTool == ButtonType::Line) {
        if (!isDrawing) {
          drawStartPoint = mouse_position;
          isDrawing = true;
          std::cout << "Line - Initial point" << std::endl;
        } else {
          framebuffer.DrawLineDDA(drawStartPoint.x, drawStartPoint.y,
                                  mouse_position.x, mouse_position.y,
                                  Color::WHITE);
          isDrawing = false;
          std::cout << "Line drawn" << std::endl;
        }
      } else if (ActiveTool == ButtonType::Rectangle) {
        if (!isDrawing) {
          drawStartPoint = mouse_position;
          isDrawing = true;
          std::cout << "Rectángulo - Primera esquina" << std::endl;
        } else {
          // Calculate width and height of the rectangle
          int x = std::min(drawStartPoint.x, mouse_position.x);
          int y = std::min(drawStartPoint.y, mouse_position.y);
          int w = std::abs(mouse_position.x - drawStartPoint.x);
          int h = std::abs(mouse_position.y - drawStartPoint.y);

          framebuffer.DrawRect(x, y, w, h, Color::WHITE, borderWidth, false,
                               Color::BLACK);
          isDrawing = false;
          std::cout << "Rectángulo dibujado" << std::endl;
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