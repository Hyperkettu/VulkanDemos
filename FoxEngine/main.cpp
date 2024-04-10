#include "pch.h"
#include "graphics/Renderer.h"

class HelloVideo {
public:
    void Run() {
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:

    void InitVulkan() {
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        Fox::Vulkan::RendererConfig config;
        config.windowHandle = static_cast<void*>(window);

        renderer = std::make_unique<Fox::Vulkan::Renderer>();
        renderer->Initialize(config);       
    }





    void MainLoop() {

        VkDevice device = Fox::Vulkan::Renderer::GetDevice();

        SDL_Event event;
        bool running = true;
        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                } else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event = SDL_WINDOWEVENT_RESIZED) {
                        renderer->ResizeWindow(event.window.data1, event.window.data2);
                    }
                    
                }
            }

            renderer->Render();
        }
        vkDeviceWaitIdle(device);
    }

    void Cleanup() {
        renderer->Destroy();
    }

protected:
    std::unique_ptr<Fox::Vulkan::Renderer> renderer;
    SDL_Window* window;


};

int main(int argc, char** args) {
    HelloVideo app;

    try {
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
};





