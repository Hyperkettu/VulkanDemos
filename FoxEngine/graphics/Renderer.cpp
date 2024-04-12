#include "pch.h"
#include "Renderer.h"

namespace Fox {
	
	namespace Vulkan {

		Renderer* Renderer::rendererInstance = nullptr;
	
		Renderer::~Renderer() {
		
		}

        void Renderer::LoadModel() {
            model = std::make_shared<Fox::Vulkan::Model>();
            model->Load(MODEL_PATH);
        }

        void Renderer::CreateSDL2Surface(SDL_Window* window) {
            if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
                throw std::runtime_error("Failed to create window surface.");
            }
        }

        void Renderer::Initialize(const Fox::Vulkan::RendererConfig& config) {
            this->config = config;
            SDL_Window* window = static_cast<SDL_Window*>(config.windowHandle);
            CreateVulkanInstance();
            SetupDebugMessenger();
            CreateSDL2Surface(window);
            Initialize();
        }

        void Renderer::Initialize() {
            swapchain = std::make_unique<Fox::Vulkan::Swapchain>();
            descriptorManager = std::make_unique<Fox::Vulkan::DescriptorSetManager>(MAX_FRAMES_IN_FLIGHT);
            graphicsPipelineState = std::make_unique<Fox::Vulkan::GraphicsPipelineStateManager>();
            renderPassManager = std::make_unique<Fox::Vulkan::RenderPassManager>();

            PickPhysicalDevice();
            CreateLogicalDevice();
            swapchain->Create();
            renderPassManager->CreateRenderPass();
            descriptorManager->CreateDescriptorSetLayouts();
            graphicsPipelineState->CreateGraphicsPipelines();
            CreateCommandPool();
            swapchain->CreateColorResources();
            swapchain->CreateDepthResources();
            swapchain->CreateFrameBuffers(renderPassManager->GetRenderPass());

            textureManager = std::make_unique<Fox::Vulkan::TextureManager>();
            textureManager->CreateTextures(mipLevels);

            samplerManager = std::make_unique<Fox::Vulkan::SamplerManager>(mipLevels);
            samplerManager->CreateSamplers();

            LoadModel();

            constantBuffers = std::make_unique<Fox::Vulkan::ConstantBuffers>();
            constantBuffers->CreateUniformBuffers(MAX_FRAMES_IN_FLIGHT);

            descriptorManager->CreateDescriptorPools();
            descriptorManager->CreateDescriptorSets();
            CreateCommandBuffers();
            
            synchronization = std::make_unique<Fox::Vulkan::Synchronization>(MAX_FRAMES_IN_FLIGHT);
        }

        void Renderer::Destroy() {
            model->~Model();
            model = nullptr;

            constantBuffers = nullptr;
            swapchain->Cleanup();

            textureManager = nullptr;
            samplerManager = nullptr;
            descriptorManager = nullptr;
            synchronization = nullptr;

            vkDestroyCommandPool(device, commandPool, nullptr);

            graphicsPipelineState = nullptr;
            renderPassManager = nullptr;

            vkDestroyDevice(device, nullptr);

            if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);

            SDL_DestroyWindow(static_cast<SDL_Window*>(config.windowHandle));
            config.windowHandle = nullptr;

            SDL_Quit();
        }

        void Renderer::Render() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            synchronization->WaitForFence(device, currentFrame);
            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(device, swapchain->swapChain, UINT64_MAX,
                synchronization->GetSempahore(Fox::Vulkan::Synchronization::SemaphoreType::IMAGE_AVAILABLE, currentFrame), VK_NULL_HANDLE, &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                swapchain->Recreate(renderPassManager->GetRenderPass());
                return;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("Failed to acquire swap chain image!");
            }

            synchronization->ResetFence(device, currentFrame);

            vkResetCommandBuffer(commandBuffers[currentFrame], 0);
            RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

            constantBuffers->SyncPerFrame(currentFrame);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = { 
                synchronization->GetSempahore(Fox::Vulkan::Synchronization::SemaphoreType::IMAGE_AVAILABLE, currentFrame)
            };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

            VkSemaphore signalSemaphores[] = {
                synchronization->GetSempahore(Fox::Vulkan::Synchronization::SemaphoreType::RENDER_FINISHED, currentFrame)
            };
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, synchronization->GetInFlightFence(currentFrame)) != VK_SUCCESS) {
                throw std::runtime_error("Failed to submit draw command buffer!");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = { swapchain->swapChain };
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr; // Optional

            result = vkQueuePresentKHR(presentQueue, &presentInfo);

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized) {
                frameBufferResized = false;
                swapchain->Recreate(renderPassManager->GetRenderPass());
            } else if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image!");
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
            
        
        }

        void Renderer::RenderBegin(VkCommandBuffer commandBuffer) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer!");
            }
        }

        void Renderer::RenderEnd(VkCommandBuffer commandBuffer) {
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer!");
            }
        }

        void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
           
            RenderBegin(commandBuffer);
            RenderPassBegin(commandBuffer, { 0.0f, 0.0f, 0.0f, 1.0f }, 1.0f, 0, renderPassManager->GetRenderPass(), swapchain->GetFramebuffer(imageIndex), {0, 0}, swapchain->GetExtent());

            SetGraphicsPipeline(commandBuffer, graphicsPipelineState->GetCurrentPipelineState());
            SetViewport(commandBuffer, 0.0f, 0.0f, static_cast<float>(swapchain->GetExtent().width), static_cast<float>(swapchain->GetExtent().height), 0.0f, 1.0f);
            SetScissor(commandBuffer, { 0, 0 }, swapchain->GetExtent());

            VkDeviceSize offsets[] = { 0 };
            std::vector<VkBuffer> vertexBuffers;
            vertexBuffers.push_back(model->GetVertexBuffer());
            SetVertexBuffers(commandBuffer, vertexBuffers, 0, offsets);

            SetIndexBuffer(commandBuffer, model->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
            SetDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, 1, descriptorManager->GetAddressOfDescriptorSet(currentFrame));
            DrawIndexed(commandBuffer, static_cast<uint32_t>(model->GetIndexCount()), 1, 0, 0, 0);

            RenderPassEnd(commandBuffer);
            RenderEnd(commandBuffer);
        }

        void Renderer::RenderPassBegin(VkCommandBuffer commandBuffer, VkClearColorValue clearColor, float clearDepth, uint32_t clearStencil, VkRenderPass renderPass, VkFramebuffer framebuffer, VkOffset2D renderAreaOffset, VkExtent2D renderArea) {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffer;
            renderPassInfo.renderArea.offset = renderAreaOffset;
            renderPassInfo.renderArea.extent = renderArea;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { clearColor };
            clearValues[1].depthStencil = { clearDepth, clearStencil };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }

        void Renderer::RenderPassEnd(VkCommandBuffer commandBuffer) {
            vkCmdEndRenderPass(commandBuffer);

        }

        void Renderer::SetGraphicsPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        }

        void Renderer::SetViewport(VkCommandBuffer commandBuffer, float topleftX, float topleftY, float width, float height, float minDepth, float maxDepth) {
            VkViewport viewport{};
            viewport.x = topleftX;
            viewport.y = topleftY;
            viewport.width = width;
            viewport.height = height;
            viewport.minDepth = minDepth;
            viewport.maxDepth = maxDepth;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        }

        void Renderer::SetScissor(VkCommandBuffer commandBuffer, VkOffset2D offset, VkExtent2D extent) {
            VkRect2D scissor{};
            scissor.offset = offset;
            scissor.extent = extent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        }

        void Renderer::SetIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer indexBuffer, VkDeviceSize offset, VkIndexType type) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, type);
        }

        void Renderer::SetVertexBuffers(VkCommandBuffer commandBuffer, std::vector<VkBuffer>& vertexBuffers, uint32_t firstBinding, const VkDeviceSize* offsets) {
            vkCmdBindVertexBuffers(commandBuffer, firstBinding, vertexBuffers.size(), vertexBuffers.data(), offsets);
        }

        void Renderer::DrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
            vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void Renderer::SetDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* descriptorSets) {
            vkCmdBindDescriptorSets(commandBuffer, bindPoint, graphicsPipelineState->GetCurrentPipelineStateLayout(), firstSet, descriptorSetCount,
                descriptorSets, 0, nullptr);
        }

        bool Renderer::HasStencilComponent(VkFormat format) {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

        VkFormat Renderer::FindDepthFormat() {
            return FindSupportedFormat(
                { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
        }

        VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
            for (VkFormat format : candidates) {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

                if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                    return format;
                } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                    return format;
                }
            }

            throw std::runtime_error("Failed to find supported format!");
        }

        void Renderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
            VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            // barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (HasStencilComponent(format)) {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            } else {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            } else {
                throw std::invalid_argument("Unsupported layout transition!");
            }

            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            EndSingleTimeCommands(commandBuffer);
        }

        VkShaderModule createShaderModule(const std::vector<char>& code) {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create shader module!");
            }
            return shaderModule;
        }

        void Renderer::CreateCommandPool() {
            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

            if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create command pool!");
            }
        }

        void Renderer::CreateCommandBuffers() {

            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate command buffers!");
            }
        }

        



        bool Renderer::IsDeviceSuitable(VkPhysicalDevice device) {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            QueueFamilyIndices indices = FindQueueFamilies(device);

            bool extensionsSupported = CheckDeviceExtensionSupport(device);

            bool swapChainAdequate = false;
            if (extensionsSupported) {
                SwapChainSupportDetails swapChainSupport = swapchain->QuerySupport(device, surface);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                deviceFeatures.geometryShader && indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
        }

        void Renderer::CreateLogicalDevice() {
            QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

            float queuePriority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies) {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};
            deviceFeatures.samplerAnisotropy = VK_TRUE;
            deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

            createInfo.pQueueCreateInfos = queueCreateInfos.data();
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

            createInfo.pEnabledFeatures = &deviceFeatures;

            createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();

            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();
            } else {
                createInfo.enabledLayerCount = 0;
            }

            if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
                throw std::runtime_error("failed to create logical device!");
            }

            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
            vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &presentQueue);
        }

        void Renderer::PickPhysicalDevice() {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            if (deviceCount == 0) {
                throw std::runtime_error("Failed to find GPUs with Vulkan support!");
            }
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const VkPhysicalDevice& device : devices) {
                if (IsDeviceSuitable(device)) {
                    physicalDevice = device;
                    config.msaaSamples = GetMaxUsableSampleCount();
                    break;
                }
            }

            if (physicalDevice == VK_NULL_HANDLE) {
                throw std::runtime_error("failed to find a suitable GPU!");
            }

        }

        bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice device) {
            QueueFamilyIndices indices;

            std::optional<uint32_t> graphicsFamily;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int i = 0;
            for (const auto& queueFamily : queueFamilies) {

                if (indices.isComplete()) {
                    break;
                }

                VkBool32 presentSupport = false;

                if (surface) {
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
                }

                if (presentSupport) {
                    indices.presentFamily = i;
                }

                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indices.graphicsFamily = i;
                }
                i++;
            }

            return indices;
        }

        void Renderer::CreateVulkanInstance() {
            VkApplicationInfo applicationInfo = {};
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pApplicationName = "Fox Engine";
            applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.pEngineName = "No Engine";
            applicationInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &applicationInfo;

            auto extensions = GetRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledLayerCount = 0;

            if (enableValidationLayers && !CheckValidationLayerSupport()) {
                throw std::runtime_error("Validation layers requested, but not available!");
            }

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                PopulateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create instance!");
            }
        }

        bool Renderer::CheckValidationLayerSupport() {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }

            return true;

        }

        std::vector<const char*> Renderer::GetRequiredExtensions() {
            SDL_Window* window  = static_cast<SDL_Window*>(config.windowHandle);
            uint32_t numSDLExtensions = 0u;
            SDL_bool success = SDL_Vulkan_GetInstanceExtensions(window, &numSDLExtensions, nullptr);

            if (!success) return std::vector<const char*>();

            std::vector<const char*> extensionNames(numSDLExtensions);
            success = SDL_Vulkan_GetInstanceExtensions(window, &numSDLExtensions, extensionNames.data());

            if (!success) return std::vector<const char*>();

            std::vector<const char*> finalExtensions(extensionNames.size());
            std::copy(extensionNames.begin(), extensionNames.end(), finalExtensions.begin());

            if (enableValidationLayers) {
                finalExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return finalExtensions;
        }

        void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo) {
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = Renderer::DebugCallback;
            createInfo.pUserData = nullptr; // Optional
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

            if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
            }

            return VK_FALSE;
        }

        VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void Renderer::SetupDebugMessenger() {
            if (!enableValidationLayers) return;

            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            PopulateDebugMessengerCreateInfo(createInfo);

            if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("Failed to set up debug messenger!");
            }
        }

        void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }

		void Renderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
			VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			EndSingleTimeCommands(commandBuffer);
		}

        void Renderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
            VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = {
                width,
                height,
                1
            };

            vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
            );

            EndSingleTimeCommands(commandBuffer);
        }

        VkCommandBuffer Renderer::BeginSingleTimeCommands() {

            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = commandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            return commandBuffer;
        }

        void Renderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {

            VkDevice device = Fox::Vulkan::Renderer::GetDevice();

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }

        VkSampleCountFlagBits Renderer::GetMaxUsableSampleCount() {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
            if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
            if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
            if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
            if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
            if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
            if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

            return VK_SAMPLE_COUNT_1_BIT;
        }

	}
}