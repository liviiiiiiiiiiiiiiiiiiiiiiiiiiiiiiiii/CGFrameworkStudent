#pragma once

#include "framework.h"
#include "image.h"
#include <vector>

class ParticleSystem {
public:
    static const int MAX_PARTICLES = 500;
    
    struct Particle {
        Vector2 position;
        Vector2 velocity;
        Color color;
        float acceleration;
        float ttl;
        bool inactive;
    };

    Particle particles[MAX_PARTICLES];

public:
    ParticleSystem();
    void Init(int windowWidth, int windowHeight);
    void Render(Image* framebuffer);
    void Update(float dt, int windowWidth, int windowHeight);
};