#include "application.h"
#include "entity.h"
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

  // Initialize drawing state
  this->ActiveTool = ButtonType::LINE;
  this->isDrawing = false;
  this->borderWidth = 2;             // Default border width
  this->triangleClickCount = 0;      // No triangle points yet
  this->currentColor = Color::WHITE; // Default drawing color
  this->fillColor = Color::WHITE;    // Default fill color
  this->isFilled = false;            // Shapes not filled by default

  this->lastPencilPosition = Vector2(0, 0);

  // Particle system
  this->showParticles = false;
  this->pS.Init(width, height);
  this->lab_mode = 2; // Start with one entity mode
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

  // Init Camera
  camera = new Camera();
  camera->LookAt(Vector3(0.f, 0.f, 1.5f), Vector3(0.f, 0.f, 0.f), Vector3::UP);
  camera->SetPerspective(45.f, window_width / (float)window_height, 0.01f,
                         100.f); // Degrees for gluPerspective

  shared_mesh = new Mesh();
  shared_mesh->LoadOBJ("meshes/lee.obj");

  entities.clear();

  for (int i = 0; i < 3; ++i) {
    Entity *e = new Entity();
    e->SetMesh(shared_mesh);

    e->base_position = Vector3(-0.6f + i * 0.6f, 0.f, 0.f);
    e->rot_axis = Vector3::UP;
    e->rotation_speed = 0.8f + 0.4f * i;
    e->scale = 0.6f + 0.2f * i;
    e->phase = i * 1.0f;

    e->Update(0.0f);
    entities.push_back(e);
  }
  entity = new Entity();
  entity->SetMesh(shared_mesh);
  Matrix44 model;
  model.SetIdentity();
  entity->SetModelMatrix(model);
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

void Application::Render(void) {
  framebuffer.SetPixel(0, 0, Color::GREEN);
  // Clear the framebuffer first (black or dark gray)
  framebuffer.Fill(Color(40, 40, 40));

  // Render Entity
  if (lab_mode == 1) {
    entity->Render(&framebuffer, camera, Color::WHITE);
  }

  // Draw UI on top
  // InitUI();

  if (showParticles) {
    pS.Render(&framebuffer);
  }

  else if (lab_mode == 2) {
    if (entities.size() > 0)
      entities[0]->Render(&framebuffer, camera, Color::BLUE);
    if (entities.size() > 1)
      entities[1]->Render(&framebuffer, camera, Color::WHITE);
    if (entities.size() > 2)
      entities[2]->Render(&framebuffer, camera, Color::RED);
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
      /*El objetivo de este update es actualizar el vector eye para que en el update matrix se calcule la nueva ViewMatrix con los valores
      nueos de donde estamos posicionando la camara con el mouse*/
      // Rotate around UP

      //Basically creating the orthonormal camera frame: Zc,Xc
      Vector3 vector_eye_center =
          camera->eye - camera->center; // distance from camera to the center
      Matrix44 R;
      R.MakeRotationMatrix(-mouse_delta.x * 0.01f,
                           Vector3::UP); // Rotation matrix to rotate the
                                         // around the vertical axis
      vector_eye_center = R * vector_eye_center;
      camera->eye = camera->center + vector_eye_center;

      // Rotate around Right
      Vector3 front = (camera->center - camera->eye).Normalize(); //Zc
      Vector3 right = front.Cross(camera->up).Normalize(); //Xc
      R.MakeRotationMatrix(-mouse_delta.y * 0.01f, right); //get the angle that we are gonna rotate
      //get the new eye vector so that it is updated and can be used by UpdateViewMatrix()
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

    if (current_prop == PROP_NEAR) {
      std::cout << "HOOOOLAA" << std::endl;
      camera->near_plane += 0.1f;
      camera->UpdateViewMatrix();
    } else if (current_prop == PROP_FAR)
      camera->far_plane += 0.1f;
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

    camera->SetPerspective(camera->fov, window_width / (float)window_height,
                           camera->near_plane, camera->far_plane);
    break;
  }

  case '-': {
    if (!camera)
      break;

    if (current_prop == PROP_NEAR)
      camera->near_plane -= 0.1f;
    else if (current_prop == PROP_FAR)
      camera->far_plane -= 0.1f;
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

    camera->SetPerspective(camera->fov, window_width / (float)window_height,
                           camera->near_plane, camera->far_plane);
    break;
  }
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

    // Color button clicks - sets both border and fill color
    if (whiteColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::WHITE;
      fillColor = Color::WHITE;
      std::cout << "Color: WHITE" << std::endl;
      return;
    }
    if (blackColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::BLACK;
      fillColor = Color::BLACK;
      std::cout << "Color: BLACK" << std::endl;
      return;
    }
    if (redColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::RED;
      fillColor = Color::RED;
      std::cout << "Color: RED" << std::endl;
      return;
    }
    if (greenColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::GREEN;
      fillColor = Color::GREEN;
      std::cout << "Color: GREEN" << std::endl;
      return;
    }
    if (blueColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::BLUE;
      fillColor = Color::BLUE;
      std::cout << "Color: BLUE" << std::endl;
      return;
    }
    if (yellowColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::YELLOW;
      fillColor = Color::YELLOW;
      std::cout << "Color: YELLOW" << std::endl;
      return;
    }
    if (pinkColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::PURPLE;
      fillColor = Color::PURPLE;
      std::cout << "Color: PURPLE" << std::endl;
      return;
    }
    if (cyanColorButton.IsMouseInside(mouse_position)) {
      currentColor = Color::CYAN;
      fillColor = Color::CYAN;
      std::cout << "Color: CYAN" << std::endl;
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
      if (framebuffer.LoadTGA("drawing.tga", true)) {
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
                                   trianglePoint3, currentColor, borderWidth,
                                   isFilled, fillColor);
          triangleClickCount = 0; // Reset for next triangle
          std::cout << "Triangle drawn" << std::endl;
        }
      } else if (ActiveTool == ButtonType::PENCIL) {
        lastPencilPosition = mouse_position;
        isDrawing = true;
        std::cout << "Pencil - Started drawing" << std::endl;
      } else if (ActiveTool == ButtonType::ERASER) {
        lastPencilPosition = mouse_position;
        isDrawing = true;
        std::cout << "Eraser - Started erasing" << std::endl;
      }
    }
  }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
  if (event.button == SDL_BUTTON_LEFT) {
    if ((ActiveTool == ButtonType::PENCIL ||
         ActiveTool == ButtonType::ERASER) &&
        isDrawing) {
      isDrawing = false;
      if (ActiveTool == ButtonType::PENCIL) {
        std::cout << "Pencil - Stopped drawing" << std::endl;
      } else {
        std::cout << "Eraser - Stopped erasing" << std::endl;
      }
    }
  }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event) {
  if (isDrawing && ActiveTool == ButtonType::PENCIL) {
    framebuffer.DrawLineDDA(lastPencilPosition.x, lastPencilPosition.y,
                            mouse_position.x, mouse_position.y, currentColor);
    lastPencilPosition = mouse_position; // Update for next segment
  } else if (isDrawing && ActiveTool == ButtonType::ERASER) {
    // Eraser draws with black color to erase
    framebuffer.DrawLineDDA(lastPencilPosition.x, lastPencilPosition.y,
                            mouse_position.x, mouse_position.y, Color::BLACK);
    lastPencilPosition = mouse_position; // Update for next segment
  }
}

void Application::OnWheel(SDL_MouseWheelEvent event) {
  if(!camera) return;
  float dy = event.preciseY;
  Vector3 v = camera->eye - camera->center;
  float dist = v.Length();
  if(dist<0.0001f) dist = 0.0001f;
  Vector3 dir = v/ dist;
  float zoom = 1.0f -dy*0.1f;
  if(zoom< 0.1f) zoom = 0.1f;
  dist*=zoom;

  //ADD MIN
  float min = 0.05f;
  float max = 50.0f;
  if(dist < min) dist = min;
  if (dist >max) dist = max;


  Vector3 new_eye = camera->center +dir*dist;
  camera->LookAt(new_eye,camera->center, Vector3::UP);
  // ...
}

void Application::OnFileChanged(const char *filename) {
  Shader::ReloadSingleShader(filename);
}