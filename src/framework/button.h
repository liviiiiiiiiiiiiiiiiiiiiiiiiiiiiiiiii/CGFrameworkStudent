#pragma once

#include "framework.h"
#include "image.h"

enum ButtonType {
    LINE,
    RECTANGLEB,
    TRIANGLE,
    COLORS,
    PENCIL,
    ERASER,
    ClearImage,
    LoadImageBtn,
    SaveImage   
};

class Button{
public:
    Image* image;
    Vector2 position;
    ButtonType type;

    Button();
    Button(Image* img, float x, float y, ButtonType t);

    bool IsMouseInside(const Vector2& mousePosition);
    void Draw(Image &framebuffer);

};