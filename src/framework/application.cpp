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
  this->ActiveTool = ButtonType::LINE;
  this->isDrawing = false;
  this->borderWidth = 2;             // Default border width
  this->triangleClickCount = 0;      // No triangle points yet
  this->currentColor = Color::WHITE; // Default drawing color
  this->fillColor = Color::BLACK;    // Default fill color
  this->isFilled = false;            // Shapes not filled by default
  this->colorMode = BORDER;          // Start in border color mode

  this->lastPencilPosition = Vector2(0, 0);

  // Particle system
  this->showParticles = true;
  this->pS.Init();

}

Application::~Application() {
  ActiveTool = ButtonType::LINE;
  isDrawing = false;
}

void Application::Init(void) {
  std::cout << "Initiating app..." << std::endl;

  // define and load image for line button. Then create button
  Image *lineImg = new Image();
  lineImg->LoadPNG("images/line.png");
  lineButton = Button(lineImg, 5, 5, ButtonType::LINE);

  // Rectangle button
  Image *RectangleImg = new Image();
  RectangleImg->LoadPNG("images/rectangle.png");
  rectangleButton = Button(RectangleImg, 40, 5, ButtonType::RECTANGLEB);

  // Triangle button
  Image *TriangleImg = new Image();
  TriangleImg->LoadPNG("images/triangle.png");
  triangleButton = Button(TriangleImg, 75, 5, ButtonType::TRIANGLE);

  // Color buttons (positioned after tool buttons)
  int colorX = 120; // Starting X position for colors
  int colorY = 5;
  int colorSpacing = 35;

  Image *whiteImg = new Image();
  whiteImg->LoadPNG("images/white.png");
  whiteColorButton = Button(whiteImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *blackImg = new Image();
  blackImg->LoadPNG("images/black.png");
  blackColorButton = Button(blackImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *redImg = new Image();
  redImg->LoadPNG("images/red.png");
  redColorButton = Button(redImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *greenImg = new Image();
  greenImg->LoadPNG("images/green.png");
  greenColorButton = Button(greenImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *blueImg = new Image();
  blueImg->LoadPNG("images/blue.png");
  blueColorButton = Button(blueImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *yellowImg = new Image();
  yellowImg->LoadPNG("images/yellow.png");
  yellowColorButton = Button(yellowImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *pinkImg = new Image();
  pinkImg->LoadPNG("images/pink.png");
  pinkColorButton = Button(pinkImg, colorX, colorY, ButtonType::COLORS);
  colorX += colorSpacing;

  Image *cyanImg = new Image();
  cyanImg->LoadPNG("images/cyan.png");
  cyanColorButton = Button(cyanImg, colorX, colorY, ButtonType::COLORS);

  // Pencil button
  Image *pencilImg = new Image();
  pencilImg->LoadPNG("images/pencil.png");
  pencilButton = Button(pencilImg, 435, 5, ButtonType::PENCIL);

  // Eraser button
  Image *eraserImg = new Image();
  eraserImg->LoadPNG("images/eraser.png");
  eraserButton = Button(eraserImg, 470, 5, ButtonType::ERASER);

  // Clear button
  Image *clearImg = new Image();
  clearImg->LoadPNG("images/clear.png");
  clearButton = Button(clearImg, 505, 5, ButtonType::ClearImage);

  // Save button
  Image *saveImg = new Image();
  saveImg->LoadPNG("images/save.png");
  saveButton = Button(saveImg, 540, 5, ButtonType::SaveImage);

  // Load button
  Image *loadImg = new Image();
  loadImg->LoadPNG("images/load.png");
  loadButton = Button(loadImg, 575, 5, ButtonType::LoadImageBtn);
}

// Init UI
void Application::InitUI(void) {
  // Draw buttons
  lineButton.Draw(framebuffer);
  rectangleButton.Draw(framebuffer);
  triangleButton.Draw(framebuffer);
  pencilButton.Draw(framebuffer);
  eraserButton.Draw(framebuffer);
  clearButton.Draw(framebuffer);

  // Draw color buttons
  whiteColorButton.Draw(framebuffer);
  blackColorButton.Draw(framebuffer);
  redColorButton.Draw(framebuffer);
  greenColorButton.Draw(framebuffer);
  blueColorButton.Draw(framebuffer);
  yellowColorButton.Draw(framebuffer);
  pinkColorButton.Draw(framebuffer);
  cyanColorButton.Draw(framebuffer);

  // Draw save and load buttons
  saveButton.Draw(framebuffer);
  loadButton.Draw(framebuffer);
}

// Render one frame
void Application::Render(void) {
  framebuffer.SetPixel(0, 0, Color::GREEN);
  InitUI();

  showParticles = true;
  if(showParticles) {
      pS.Render(&framebuffer);
  }

  
  framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed){
  if(showParticles) {
      pS.Update(seconds_elapsed);
      framebuffer.Fill(Color::BLACK);

  } 
}

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

  case 'f': // Toggle fill
  case 'F':
    isFilled = !isFilled;
    std::cout << "Fill: " << (isFilled ? "ON" : "OFF") << std::endl;
    break;

  case 'c': // Toggle color mode (border vs fill)
  case 'C':
    colorMode = (colorMode == BORDER) ? FILL : BORDER;
    std::cout << "Color Mode: " << (colorMode == BORDER ? "BORDER" : "FILL")
              << std::endl;
    break;
  }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
    // check click on buttons
    if (lineButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::LINE;
      isDrawing = false; // Reset state
      std::cout << "Line tool activated" << std::endl;
      return;
    }

    if (rectangleButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::RECTANGLEB;
      isDrawing = false; // Reset state
      std::cout << "Rectangle tool activated" << std::endl;
      return;
    }

    if (triangleButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::TRIANGLE;
      triangleClickCount = 0; // Reset triangle state
      std::cout << "Triangle tool activated" << std::endl;
      return;
    }

    // Color button clicks - sets border or fill color based on current mode
    if (whiteColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::WHITE;
        std::cout << "Border Color: WHITE" << std::endl;
      } else {
        fillColor = Color::WHITE;
        std::cout << "Fill Color: WHITE" << std::endl;
      }
      return;
    }
    if (blackColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::BLACK;
        std::cout << "Border Color: BLACK" << std::endl;
      } else {
        fillColor = Color::BLACK;
        std::cout << "Fill Color: BLACK" << std::endl;
      }
      return;
    }
    if (redColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::RED;
        std::cout << "Border Color: RED" << std::endl;
      } else {
        fillColor = Color::RED;
        std::cout << "Fill Color: RED" << std::endl;
      }
      return;
    }
    if (greenColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::GREEN;
        std::cout << "Border Color: GREEN" << std::endl;
      } else {
        fillColor = Color::GREEN;
        std::cout << "Fill Color: GREEN" << std::endl;
      }
      return;
    }
    if (blueColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::BLUE;
        std::cout << "Border Color: BLUE" << std::endl;
      } else {
        fillColor = Color::BLUE;
        std::cout << "Fill Color: BLUE" << std::endl;
      }
      return;
    }
    if (yellowColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::YELLOW;
        std::cout << "Border Color: YELLOW" << std::endl;
      } else {
        fillColor = Color::YELLOW;
        std::cout << "Fill Color: YELLOW" << std::endl;
      }
      return;
    }
    if (pinkColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::PURPLE;
        std::cout << "Border Color: PURPLE" << std::endl;
      } else {
        fillColor = Color::PURPLE;
        std::cout << "Fill Color: PURPLE" << std::endl;
      }
      return;
    }
    if (cyanColorButton.IsMouseInside(mouse_position)) {
      if (colorMode == BORDER) {
        currentColor = Color::CYAN;
        std::cout << "Border Color: CYAN" << std::endl;
      } else {
        fillColor = Color::CYAN;
        std::cout << "Fill Color: CYAN" << std::endl;
      }
      return;
    }
    if (clearButton.IsMouseInside(mouse_position)) {
      framebuffer.Fill(Color::BLACK);
      std::cout << "Canvas cleared" << std::endl;
      return; // Note: This doesn't change ActiveTool
    }
    if (saveButton.IsMouseInside(mouse_position)) {
      if (framebuffer.SaveTGA("drawing.tga")) {
        std::cout << "Drawing saved to drawing.tga" << std::endl;
      } else {
        std::cout << "Failed to save drawing" << std::endl;
      }
      return;
    }

    if (loadButton.IsMouseInside(mouse_position)) {
      if (framebuffer.LoadTGA("drawing.tga")) {
        std::cout << "Drawing loaded from drawing.tga" << std::endl;
      } else {
        std::cout << "Failed to load drawing" << std::endl;
      }
      return;
    }

    if (pencilButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::PENCIL;
      isDrawing = false;
      std::cout << "Pencil tool activated" << std::endl;
      return;
    }

    if (eraserButton.IsMouseInside(mouse_position)) {
      ActiveTool = ButtonType::ERASER;
      isDrawing = false;
      std::cout << "Eraser tool activated" << std::endl;
      return;
    }

    // If clicked outside the toolbar
    if (mouse_position.y > 50) {
      if (ActiveTool == ButtonType::LINE) {
        if (!isDrawing) {
          drawStartPoint = mouse_position;
          isDrawing = true;
          std::cout << "Line - Initial point" << std::endl;
        } else {
          framebuffer.DrawLineDDA(drawStartPoint.x, drawStartPoint.y,
                                  mouse_position.x, mouse_position.y,
                                  currentColor);
          isDrawing = false;
          std::cout << "Line drawn" << std::endl;
        }
      } else if (ActiveTool == ButtonType::RECTANGLEB) {
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

          framebuffer.DrawRect(x, y, w, h, currentColor, borderWidth, isFilled,
                               fillColor);
          isDrawing = false;
          std::cout << "Rectángulo dibujado" << std::endl;
        }
      } else if (ActiveTool == ButtonType::TRIANGLE) {
        // Triangle needs 3 clicks
        if (triangleClickCount == 0) {
          // First click: store first point
          trianglePoint1 = mouse_position;
          triangleClickCount = 1;
          std::cout << "Triangle - Point 1" << std::endl;
        } else if (triangleClickCount == 1) {
          // Second click: store second point
          trianglePoint2 = mouse_position;
          triangleClickCount = 2;
          std::cout << "Triangle - Point 2" << std::endl;
        } else if (triangleClickCount == 2) {
          // Third click: draw the triangle
          Vector2 trianglePoint3 = mouse_position;
          framebuffer.DrawTriangle(trianglePoint1, trianglePoint2,
                                   trianglePoint3, currentColor, isFilled,
                                   fillColor);
          triangleClickCount = 0; // Reset for next triangle
          std::cout << "Triangle drawn" << std::endl;
        }
      } else if (ActiveTool == ButtonType::PENCIL) {
        lastPencilPosition = mouse_position;
        isDrawing = true;
        std::cout << "Pencil - Started drawing" << std::endl;
      }
    }
  }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
    if (ActiveTool == ButtonType::PENCIL && isDrawing) {
      isDrawing = false;
      std::cout << "Pencil - Stopped drawing" << std::endl;
    }
  }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event) {
  if (isDrawing && ActiveTool == ButtonType::PENCIL) {
    framebuffer.DrawLineDDA(lastPencilPosition.x, lastPencilPosition.y,
                            mouse_position.x, mouse_position.y, currentColor);
    lastPencilPosition = mouse_position; // Update for next segment
  }
}

void Application::OnWheel(SDL_MouseWheelEvent event) {
  float dy = event.preciseY;

  // ...
}

void Application::OnFileChanged(const char *filename) {
  Shader::ReloadSingleShader(filename);
}