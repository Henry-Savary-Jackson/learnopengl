# 3D OpenGL rendering Engine

A prototype for a rendering engine using OpenGL

## NB This project is unfinished.
Many of the goals listed below were overly ambitious for the time I had available. I might continue working on this if time allows it.

# Goals
## Rendering
- [x]  Implement architecture for Batch rendering+GPU instancing for scenes with static and dynamic objects
with different textures, shaders and meshes as well as multiple lights

- [x] add skyboxes

- Implement a way to add post-processing effects 
    - [x] Blur
    - [x] Bloom
    - [x] Edge detection
    - Stencils
- Implement basic trasparency
- Implement deffered lighting for multiple lights (might conflict with trasnparency)



## Physics
- Implement efficient collisions detection for different hitboxes
    - AABB
    - Sphere 
    - Cube
- Implement basic rigid body physics for objects
    - Gravity
    - Momentum
    - Applying forces 
    - Torque
    - Friction

## UI
- Implement rendering text (maybe use imgui instead of from scratch)
- Implement interacting and spawning objects
- Implement loading new objects from files
- Implement screen recording directly
