#include "particlesystem.h"
#include "application.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

ParticleSystem::ParticleSystem() {
    std::srand(std::time(0)); 
}

void ParticleSystem::Init(int windowWidth, int windowHeight) {
    std::cout << "Initializing particle system with " << MAX_PARTICLES << " particles\n";
    
    // Initialize all particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        p.position.x = (rand() % windowWidth);  
        p.position.y = (rand() % windowHeight);
        
        float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
        float speed = 10.0f + (rand() / (float)RAND_MAX) * 90.0f;
        p.velocity.x = cos(angle) * speed;
        p.velocity.y = sin(angle) * speed;
        
        p.color = Color(0,0, 255);
        
        p.acceleration = 50.0f +( rand() / (float)RAND_MAX ) * 100.0f;
        
        p.ttl = 1.0f + (float)( rand()/ (float)RAND_MAX ) * 4.0f;
        
        p.inactive = false;
    }
}

void ParticleSystem::Render(Image* framebuffer) {
    if (!framebuffer) return;
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        if (p.inactive) continue;
        
        int x = (int)(p.position.x);
        int y = (int)(p.position.y);
        
        if (x >= 0 && x < (framebuffer->width) && y >= 0 && y < (framebuffer->height)) {
            framebuffer->SetPixel(x, y, p.color);

        }
    }
}

void ParticleSystem::Update(float dt, int width, int height) {
          
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        if (p.inactive) continue;
        
        // time to live
        p.ttl -= dt;
        
        // Reset expired particle
        if (p.ttl <= 0) {
            p.inactive = true;
            
            p.position.x = (float)(rand() % width);
            p.position.y = (float)(rand() % height);
            
            float angle = (float)(rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
            float speed = 10.0f + (float)(rand() / (float)RAND_MAX) * 90.0f;
            p.velocity.x = cos(angle) * speed;
            p.velocity.y = sin(angle)* speed;
            
            //Particles start blue and become more 'white' over time
            Color current = p.color;
            int red = current.r + 50;   
            int green = current.g + 50; 
            
            if (red > 255) red = 255;
            if (green > 255) green = 255;
            
            p.color = Color(red, green, current.b);
            
            // TTL
            p.ttl = 1.0f + (float)(rand()/ (float)RAND_MAX) * 4.0f;
            p.inactive = false;
            
            continue;
        }
        
        p.velocity.y += p.acceleration * dt;
        
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        
        
        if (p.position.x < 0) {
            p.position.x = 0;
            p.velocity.x = -p.velocity.x * 0.8f; 
        }
        else if (p.position.x >= width) {
            p.position.x = width - 1;
            p.velocity.x = -p.velocity.x * 0.8f;
        }
        
        if (p.position.y < 0) {
            p.position.y = 0;
            p.velocity.y = -p.velocity.y * 0.8f;
        }
        else if (p.position.y >= height) {
            p.position.y = height - 1;
            p.velocity.y = -p.velocity.y * 0.8f;
        }
    }
}