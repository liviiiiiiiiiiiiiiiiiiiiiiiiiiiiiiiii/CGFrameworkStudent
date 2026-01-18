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

// Init UI
void Application::InitUI(void) {
  // Draw background bar
  framebuffer.DrawRect(0, 0, window_width, 50, Color::GRAY, 1.f, true,
                       Color::GRAY);

  // Load all toolbar icons (static so they only load once)
  static Image pencilImg, eraserImg, triangleImg, lineImg, rectangleImg,
      clearImg, loadImg, saveImg;
  static Image whiteImg, blackImg, redImg, greenImg, blueImg, yellowImg,
      pinkImg;
  static bool imagesLoaded = false;

  if (!imagesLoaded) {
    pencilImg.LoadPNG("images/pencil.png");
    eraserImg.LoadPNG("images/eraser.png");
    triangleImg.LoadPNG("images/triangle.png");
    lineImg.LoadPNG("images/line.png");
    rectangleImg.LoadPNG("images/rectangle.png");
    clearImg.LoadPNG("images/clear.png");
    loadImg.LoadPNG("images/load.png");
    saveImg.LoadPNG("images/save.png");
    imagesLoaded = true;
    whiteImg.LoadPNG("images/white.png");
    blackImg.LoadPNG("images/black.png");
    redImg.LoadPNG("images/red.png");
    greenImg.LoadPNG("images/green.png");
    blueImg.LoadPNG("images/blue.png");
    yellowImg.LoadPNG("images/yellow.png");
    pinkImg.LoadPNG("images/pink.png");
  }

  // Display icons horizontally with spacing
  int x = 5;        // Starting x position
  int y = 5;        // Y position (a bit of padding from top)
  int spacing = 10; // Space between icons

  framebuffer.DrawImage(loadImg, x, y);
  x += loadImg.width + spacing;

  framebuffer.DrawImage(saveImg, x, y);
  x += saveImg.width + spacing;

  framebuffer.DrawImage(clearImg, x, y);
  x += clearImg.width + spacing;

  framebuffer.DrawImage(pencilImg, x, y);
  x += pencilImg.width + spacing;

  framebuffer.DrawImage(eraserImg, x, y);
  x += eraserImg.width + spacing;

  framebuffer.DrawImage(triangleImg, x, y);
  x += triangleImg.width + spacing;

  framebuffer.DrawImage(lineImg, x, y);
  x += lineImg.width + spacing;

  framebuffer.DrawImage(rectangleImg, x, y);
  x += rectangleImg.width + spacing;

  framebuffer.DrawImage(whiteImg, x, y);
  x += whiteImg.width + spacing;

  framebuffer.DrawImage(blackImg, x, y);
  x += blackImg.width + spacing;

  framebuffer.DrawImage(redImg, x, y);
  x += redImg.width + spacing;

  framebuffer.DrawImage(greenImg, x, y);
  x += greenImg.width + spacing;

  framebuffer.DrawImage(blueImg, x, y);
  x += blueImg.width + spacing;

  framebuffer.DrawImage(yellowImg, x, y);
  x += yellowImg.width + spacing;

  framebuffer.DrawImage(pinkImg, x, y);
  x += pinkImg.width + spacing;
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