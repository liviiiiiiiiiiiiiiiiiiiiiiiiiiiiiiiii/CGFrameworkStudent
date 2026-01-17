#include "button.h"

Button::Button() {
    image = NULL;
    position = Vector2(0, 0);
    type = Line;
}

Button::Button(Image* img, float x, float y, ButtonType t) {
    image = img;
    position = Vector2(x, y);
    type = t;
}


bool Button::IsMouseInside(const Vector2& mousePosition) {
    if (image == NULL) return false;
    
    return (mousePosition.x >= position.x && 
            mousePosition.x <= position.x + image->width &&
            mousePosition.y >= position.y && 
            mousePosition.y <= position.y + image->height);
}