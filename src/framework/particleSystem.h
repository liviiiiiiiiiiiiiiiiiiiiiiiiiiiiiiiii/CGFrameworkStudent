#pragma once

#include "framework.h"
#include "image.h"
#include <vector>

class ParticleSystem {
public:
    static const int MAX_PARTICLES = 500;
    
    struct Particle {
        Vector2 position;
        Vector2 velocity; // Normalized speed and direction of the particle
        Color color;
        float acceleration;
        float ttl; // Time left until the particle expires
        bool inactive; // Particle is not used/expired, so it can be recreated
        
    };

    Particle particles[MAX_PARTICLES];

public:
    ParticleSystem();
    ParticleSystem(Vector2 position, Vector2 velocity, Color color, float acceleration, float ttl, bool inactive);
    void Init();
    void Render(Image* framebuffer);
    void Update(float dt);
};