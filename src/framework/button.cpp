#include "button.h"

Button::Button() {
  Image *image = NULL;
  Vector2 position = Vector2(0, 0);
  ButtonType type = LINE;
}

Button::Button(Image *img, float x, float y, ButtonType t) {
  Image *image = img;
  Vector2 position = Vector2(x, y);
  ButtonType type = t;
}

bool Button::IsMouseInside(const Vector2 &mousePosition) {
  if (image == NULL)
    return false;

  return (mousePosition.x >= position.x &&
          mousePosition.x <= position.x + image->width &&
          mousePosition.y >= position.y &&
          mousePosition.y <= position.y + image->height);
}

void Button::Draw(Image &framebuffer) {
  if (&framebuffer == NULL)
    return;
  framebuffer.DrawImage(framebuffer, position.x, position.y);
}