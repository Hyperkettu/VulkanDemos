### Vulkan Fox Renderer

## Installation
- Install Vulkan SDK
- Include Include directory from Vulkan SDK folder 
- Include Library Directory from Vulkan SDK folder

- Add to Linker -> Input: SDL2.lib SDLmain.lib vulkan-1.lib
- Copy SDL2.dll to Renderer folder

## Precompiled header
 - include as first pch.h
 - Create pch option in pch.cpp

 ## Setup validation layers
 - vk_layer_settings.txt in Config folder of Vulkan SDK tells how to setup validation layers


 ## To Learn
 
    Push constants
    Instanced rendering
    Dynamic uniforms
    Separate images and sampler descriptors
    Pipeline cache
    Multi-threaded command buffer generation
    Multiple subpasses
    Compute shaders


    ### Compute Shaders
    
    Shared memory
    Asynchronous compute
    Atomic operations
    Subgroups 