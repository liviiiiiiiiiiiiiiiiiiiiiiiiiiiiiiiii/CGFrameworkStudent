#include "particlesystem.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

ParticleSystem::ParticleSystem() {
    std::srand(static_cast<unsigned int>(time(NULL)));
}

void ParticleSystem::Init(){
    std::cout << "Initializing particle system with " << MAX_PARTICLES << " particles" << std::endl;
    
    srand(static_cast<unsigned int>(time(NULL)));
    
    // Initialize all particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        p.position.x = static_cast<float>(rand() % 800 );  
        p.position.y = static_cast<float>(rand() % 600);
        
        float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
        p.velocity.x = cos(angle);
        p.velocity.y = sin(angle);
        
        float speed = 10.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 90.0f;
        p.velocity.x *= speed;
        p.velocity.y *= speed;
        
        p.color = Color(
            rand() % 256,
            rand() % 256,
            rand() % 256
        );
        
        p.acceleration = 50.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
        
        p.ttl = 1.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f;
        
        p.inactive = false;
    }
}

void ParticleSystem::Render(Image* framebuffer) {
    if (!framebuffer) return;
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        if (p.inactive) continue;
        
        int x = static_cast<int>(p.position.x);
        int y = static_cast<int>(p.position.y);
        
        if (x >= 0 && x < static_cast<int>(framebuffer->width) && 
            y >= 0 && y < static_cast<int>(framebuffer->height)) {
            framebuffer->SetPixel(x, y, p.color);

        }
    }
}

void ParticleSystem::Update(float dt) {
          
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = particles[i];
        
        if (p.inactive) continue;
        
        // time to live
        p.ttl -= dt;
        
        // Reset expired particle
        if (p.ttl <= 0) {
            p.inactive = true;
            
            p.position.x = static_cast<float>(rand() % 800);
            p.position.y = static_cast<float>(rand() % 600);
            
            float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
            p.velocity.x = cos(angle);
            p.velocity.y = sin(angle);
            float speed = 10.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 90.0f;
            p.velocity.x *= speed;
            p.velocity.y *= speed;
            
            p.color = Color(
                rand() % 256,
                rand() % 256,
                rand() % 256
            );
            
            // TTL
            p.ttl = 1.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f;
            p.inactive = false;
            
            continue;
        }
        
        p.velocity.y += p.acceleration * dt;
        
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        
        const float screenWidth = 800.0f;
        const float screenHeight = 600.0f;
        
        if (p.position.x < 0) {
            p.position.x = 0;
            p.velocity.x = -p.velocity.x * 0.8f; // Dampen velocity
        }
        else if (p.position.x >= screenWidth) {
            p.position.x = screenWidth - 1;
            p.velocity.x = -p.velocity.x * 0.8f;
        }
        
        if (p.position.y < 0) {
            p.position.y = 0;
            p.velocity.y = -p.velocity.y * 0.8f;
        }
        else if (p.position.y >= screenHeight) {
            p.position.y = screenHeight - 1;
            p.velocity.y = -p.velocity.y * 0.8f;
        }
    }
}