#include "pch.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include <STB_Image/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB_Image/stb_image_write.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/tiny_obj_loader.h>

static const uint64_t render_width = 800;
static const uint64_t render_height = 600;
static const uint32_t workgroup_width = 16;
static const uint32_t workgroup_height = 8;

using CheckResultCallback = std::function<bool(VkResult, const char*, int32_t, const char*)>;
CheckResultCallback g_checkResultCallback;

#define LOG(format, ...) \
    do { \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), format, ##__VA_ARGS__); \
        std::cout << buffer << std::endl; \
    } while (0)

const char* getResultString(VkResult result)
{
    const char* resultString = "unknown";

#define STR(a)                                                                                                         \
  case a:                                                                                                              \
    resultString = #a;                                                                                                 \
    break;

    switch (result)
    {
        STR(VK_SUCCESS);
        STR(VK_NOT_READY);
        STR(VK_TIMEOUT);
        STR(VK_EVENT_SET);
        STR(VK_EVENT_RESET);
        STR(VK_INCOMPLETE);
        STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR(VK_ERROR_INITIALIZATION_FAILED);
        STR(VK_ERROR_DEVICE_LOST);
        STR(VK_ERROR_MEMORY_MAP_FAILED);
        STR(VK_ERROR_LAYER_NOT_PRESENT);
        STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR(VK_ERROR_FEATURE_NOT_PRESENT);
        STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR(VK_ERROR_TOO_MANY_OBJECTS);
        STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR(VK_ERROR_FRAGMENTED_POOL);
        STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR(VK_ERROR_SURFACE_LOST_KHR);
        STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(VK_SUBOPTIMAL_KHR);
        STR(VK_ERROR_OUT_OF_DATE_KHR);
        STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(VK_ERROR_VALIDATION_FAILED_EXT);
        STR(VK_ERROR_INVALID_SHADER_NV);
        STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR(VK_ERROR_FRAGMENTATION_EXT);
        STR(VK_ERROR_NOT_PERMITTED_EXT);
        STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
    }
#undef STR
    return resultString;
}

bool checkResult(VkResult result, const char* message = nullptr) {
    if (g_checkResultCallback)
        return g_checkResultCallback(result, nullptr, -1, message);

    if (result == VK_SUCCESS)
    {
        return false;
    }

    if (result < 0)
    {
        if (message)
        {
            LOG("VkResult %d - %s - %s\n", result, getResultString(result), message);
        } else
        {
            LOG("VkResult %d - %s\n", result, getResultString(result));
        }
        assert(!"Critical Vulkan Error");
        return true;
    }

    return false;
}

bool checkResult(VkResult result, const char* file, int32_t line) {
    if (g_checkResultCallback)
        return g_checkResultCallback(result, file, line, nullptr);

    if (result == VK_SUCCESS)
    {
        return false;
    }

    if (result < 0)
    {
        LOG("%s(%d): Vulkan Error : %s\n", file, line, getResultString(result));
        assert(!"Critical Vulkan Error");

        return true;
    }

    return false;
}


#ifndef VK_CHECK
#define VK_CHECK(result) checkResult(result, __FILE__, __LINE__)
#endif

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct PhysicalDeviceInfo
{
    VkPhysicalDeviceMemoryProperties     memoryProperties{};
    std::vector<VkQueueFamilyProperties> queueProperties;

    VkPhysicalDeviceFeatures         features10{};
    VkPhysicalDeviceVulkan11Features features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
    VkPhysicalDeviceVulkan12Features features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    VkPhysicalDeviceVulkan13Features features13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };

    VkPhysicalDeviceProperties         properties10{};
    VkPhysicalDeviceVulkan11Properties properties11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES };
    VkPhysicalDeviceVulkan12Properties properties12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES };
    VkPhysicalDeviceVulkan13Properties properties13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES };
};

struct Features11Old
{
    VkPhysicalDeviceMultiviewFeatures    multiview{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
    VkPhysicalDevice16BitStorageFeatures t16BitStorage{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
    VkPhysicalDeviceSamplerYcbcrConversionFeatures samplerYcbcrConversion{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES };
    VkPhysicalDeviceProtectedMemoryFeatures protectedMemory{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES };
    VkPhysicalDeviceShaderDrawParameterFeatures drawParameters{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES };
    VkPhysicalDeviceVariablePointerFeatures variablePointers{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };

    Features11Old()
    {
        multiview.pNext = &t16BitStorage;
        t16BitStorage.pNext = &samplerYcbcrConversion;
        samplerYcbcrConversion.pNext = &protectedMemory;
        protectedMemory.pNext = &drawParameters;
        drawParameters.pNext = &variablePointers;
        variablePointers.pNext = nullptr;
    }

    void read(const VkPhysicalDeviceVulkan11Features& features11)
    {
        multiview.multiview = features11.multiview;
        multiview.multiviewGeometryShader = features11.multiviewGeometryShader;
        multiview.multiviewTessellationShader = features11.multiviewTessellationShader;
        t16BitStorage.storageBuffer16BitAccess = features11.storageBuffer16BitAccess;
        t16BitStorage.storageInputOutput16 = features11.storageInputOutput16;
        t16BitStorage.storagePushConstant16 = features11.storagePushConstant16;
        t16BitStorage.uniformAndStorageBuffer16BitAccess = features11.uniformAndStorageBuffer16BitAccess;
        samplerYcbcrConversion.samplerYcbcrConversion = features11.samplerYcbcrConversion;
        protectedMemory.protectedMemory = features11.protectedMemory;
        drawParameters.shaderDrawParameters = features11.shaderDrawParameters;
        variablePointers.variablePointers = features11.variablePointers;
        variablePointers.variablePointersStorageBuffer = features11.variablePointersStorageBuffer;
    }

    void write(VkPhysicalDeviceVulkan11Features& features11)
    {
        features11.multiview = multiview.multiview;
        features11.multiviewGeometryShader = multiview.multiviewGeometryShader;
        features11.multiviewTessellationShader = multiview.multiviewTessellationShader;
        features11.storageBuffer16BitAccess = t16BitStorage.storageBuffer16BitAccess;
        features11.storageInputOutput16 = t16BitStorage.storageInputOutput16;
        features11.storagePushConstant16 = t16BitStorage.storagePushConstant16;
        features11.uniformAndStorageBuffer16BitAccess = t16BitStorage.uniformAndStorageBuffer16BitAccess;
        features11.samplerYcbcrConversion = samplerYcbcrConversion.samplerYcbcrConversion;
        features11.protectedMemory = protectedMemory.protectedMemory;
        features11.shaderDrawParameters = drawParameters.shaderDrawParameters;
        features11.variablePointers = variablePointers.variablePointers;
        features11.variablePointersStorageBuffer = variablePointers.variablePointersStorageBuffer;
    }
};

struct Properties11Old
{
    VkPhysicalDeviceMaintenance3Properties maintenance3{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceIDProperties           deviceID{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES };
    VkPhysicalDeviceMultiviewProperties    multiview{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES };
    VkPhysicalDeviceProtectedMemoryProperties protectedMemory{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES };
    VkPhysicalDevicePointClippingProperties pointClipping{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES };
    VkPhysicalDeviceSubgroupProperties      subgroup{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES };

    Properties11Old()
    {
        maintenance3.pNext = &deviceID;
        deviceID.pNext = &multiview;
        multiview.pNext = &protectedMemory;
        protectedMemory.pNext = &pointClipping;
        pointClipping.pNext = &subgroup;
        subgroup.pNext = nullptr;
    }

    void write(VkPhysicalDeviceVulkan11Properties& properties11)
    {
        memcpy(properties11.deviceLUID, deviceID.deviceLUID, sizeof(properties11.deviceLUID));
        memcpy(properties11.deviceUUID, deviceID.deviceUUID, sizeof(properties11.deviceUUID));
        memcpy(properties11.driverUUID, deviceID.driverUUID, sizeof(properties11.driverUUID));
        properties11.deviceLUIDValid = deviceID.deviceLUIDValid;
        properties11.deviceNodeMask = deviceID.deviceNodeMask;
        properties11.subgroupSize = subgroup.subgroupSize;
        properties11.subgroupSupportedStages = subgroup.supportedStages;
        properties11.subgroupSupportedOperations = subgroup.supportedOperations;
        properties11.subgroupQuadOperationsInAllStages = subgroup.quadOperationsInAllStages;
        properties11.pointClippingBehavior = pointClipping.pointClippingBehavior;
        properties11.maxMultiviewViewCount = multiview.maxMultiviewViewCount;
        properties11.maxMultiviewInstanceIndex = multiview.maxMultiviewInstanceIndex;
        properties11.protectedNoFault = protectedMemory.protectedNoFault;
        properties11.maxPerSetDescriptors = maintenance3.maxPerSetDescriptors;
        properties11.maxMemoryAllocationSize = maintenance3.maxMemoryAllocationSize;
    }
};

struct Queue
{
    VkQueue  queue = VK_NULL_HANDLE;
    uint32_t familyIndex = ~0U;
    uint32_t queueIndex = ~0U;
    float    priority = 1.0f;

    operator VkQueue() const { return queue; }
    operator uint32_t() const { return familyIndex; }
};

struct QueueScore
{
    uint32_t score = 0;  // the lower the score, the more 'specialized' it is
    uint32_t familyIndex = ~0u;
    uint32_t queueIndex = ~0u;
    float priority = 1.0f;
};

struct QueueSetup
{
    VkQueueFlags requiredFlags = 0;
    uint32_t     count = 0;
    float        priority = 1.0f;
};
using QueueArray = std::vector<QueueSetup>;

struct Entry
{
    Entry(const char* entryName, bool isOptional = false, void* pointerFeatureStruct = nullptr, uint32_t checkVersion = 0)
        : name(entryName)
        , optional(isOptional)
        , pFeatureStruct(pointerFeatureStruct)
        , version(checkVersion)
    {
    }

    std::string name;
    bool        optional{ false };
    void* pFeatureStruct{ nullptr };
    uint32_t    version{ 0 };
};


class SimplePathTracerApplication {
public:
    void run() {

        apiMajor = 1;
        apiMinor = 2;

        deviceExtensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
        deviceExtensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        deviceExtensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
        deviceExtensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

        VkPhysicalDeviceAccelerationStructureFeaturesKHR asFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
        deviceExtensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, false, &asFeatures);

        VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR };
        deviceExtensions.emplace_back(VK_KHR_RAY_QUERY_EXTENSION_NAME, false, &rayQueryFeatures);

      //  deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        deviceExtensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

        bool initialized = initVulkan();
        if (!initialized) {
            return;
        }

        VkDeviceSize bufferSizeBytes = render_width * render_height * 3 * sizeof(float);
        VkBufferCreateInfo bufferCreateInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                            .size = bufferSizeBytes,
                                            .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT };

        buffer = createDedicatedBuffer(device, bufferCreateInfo,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkCommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = graphicsComputeTransferQueue.familyIndex;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = 0;
        
        VkCommandPool commandPool;
        VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

        // Shader loading and pipeline creation
        VkShaderModule rayTraceModule = createShaderModule(readFile("shaders/raytrace.comp.spv"));

        // Describes the entrypoint and the stage to use for this shader module in the pipeline
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                              .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                                                              .module = rayTraceModule,
                                                              .pName = "main" };

        // For the moment, create an empty pipeline layout. You can ignore this code
        // for now; we'll replace it in the next chapter.
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  //
                                                            .setLayoutCount = 0,                             //
                                                            .pushConstantRangeCount = 0 };
        VkPipelineLayout           pipelineLayout;
        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

        // Create the compute pipeline
        VkComputePipelineCreateInfo pipelineCreateInfo{ .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,  //
                                                       .stage = shaderStageCreateInfo,                           //
                                                       .layout = pipelineLayout };
        // Don't modify flags, basePipelineHandle, or basePipelineIndex
        VkPipeline computePipeline;
        VK_CHECK(vkCreateComputePipelines(device,                 // Device
            VK_NULL_HANDLE,          // Pipeline cache (uses default)
            1, &pipelineCreateInfo,  // Compute pipeline create info
            nullptr,                 // Allocator (uses default)
            &computePipeline));      // Output

        // Allocate a command buffer
        VkCommandBufferAllocateInfo commandBufferAllocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                 .commandPool = commandPool,
                                                 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                 .commandBufferCount = 1 };
        VkCommandBuffer             commandBuffer;
        VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        // Begin recording
        VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                           .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

        // Run the compute shader with one workgroup for now
        vkCmdDispatch(commandBuffer, 1, 1, 1);

        VkMemoryBarrier memoryBarrier{ .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
                                .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,  // Make transfer writes
                                .dstAccessMask = VK_ACCESS_HOST_READ_BIT };      // Readable by the CPU
        vkCmdPipelineBarrier(commandBuffer,                                               // The command buffer
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,                          // From the transfer stage
            VK_PIPELINE_STAGE_HOST_BIT,                              // To the CPU
            0,                                                       // No special flags
            1, &memoryBarrier,                                       // An array of memory barriers
            0, nullptr, 0, nullptr);                                 // No other barriers

        // End recording
        VK_CHECK(vkEndCommandBuffer(commandBuffer));

        // Submit the command buffer
        VkSubmitInfo submitInfo{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,  
                                .commandBufferCount = 1,                              
                                .pCommandBuffers = &commandBuffer };
        VK_CHECK(vkQueueSubmit(graphicsComputeTransferQueue.queue, 1, &submitInfo, VK_NULL_HANDLE));

        // Wait for the GPU to finish
        VK_CHECK(vkQueueWaitIdle(graphicsComputeTransferQueue.queue));

        void* data = nullptr;
        VkResult result = vkMapMemory(device, bufferMemory, 0, bufferSizeBytes, 0, &data);
        assert(result == VK_SUCCESS);

        float* fltData = reinterpret_cast<float*>(data);
        LOG("First three elements: %f, %f, %f\n", fltData[0], fltData[1], fltData[2]);       
        stbi_write_hdr("output.hdr", render_width, render_height, 3, fltData);
        vkUnmapMemory(device, bufferMemory);

        vkDestroyPipeline(device, computePipeline, nullptr);
        vkDestroyShaderModule(device, rayTraceModule, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);  // Will be removed in the next chapter

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        vkDestroyCommandPool(device, commandPool, nullptr);

        mainLoop();
        cleanup();
    }

private:

    static std::vector<char> readFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule createShaderModule(const std::vector<char>& code) {
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

    bool initVulkan() {

        requestedQueues.push_back({ defaultQueueGraphicsComputerTransfer, 1, defaultPriorityGraphicsComputeTransfer });
        requestedQueues.push_back({ defaultQueueTransfer, 1, defaultPriorityTransfer });
        requestedQueues.push_back({ defaultQueueCompute, 1, defaultPriorityCompute });

#ifndef NDEBUG
        instanceExtensions.push_back({ VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true });
        if (enableValidationLayers)
            instanceLayers.push_back({ "VK_LAYER_KHRONOS_validation", true });
#endif
    
        if (!createInstance())
            return false;

        // Find all compatible devices
        auto compatibleDevices = getCompatibleDevices();
        if (compatibleDevices.empty())
        {
            assert(!"No compatible device found");
            return false;
        }

        // Use a compatible device
        return initDevice(compatibleDevices[compatibleDeviceIndex]);
    }

    bool createInstance() {
        VkApplicationInfo applicationInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        applicationInfo.pApplicationName = "Simple Path Tracer";
        applicationInfo.pEngineName = "Simple Path Tracer";
        applicationInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

        apiMajor = 1;
        apiMinor = 1;

        if (verboseUsed)
        {
            uint32_t version;
            VkResult result = vkEnumerateInstanceVersion(&version);
            VK_CHECK(result);
            LOG("_______________\n");
            LOG("Vulkan Version:\n");
            LOG(" - available:  %d.%d.%d\n", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
            LOG(" - requesting: %d.%d.%d\n", apiMajor, apiMinor, 0);
        }

        auto layerProperties = getInstanceLayers();

        if (fillFilteredNameArray(usedInstanceLayers, layerProperties, instanceLayers) != VK_SUCCESS)
        {
            return false;
        }
        if (verboseAvailable)
        {
            LOG("___________________________\n");
            LOG("Available Instance Layers :\n");
            for (const VkLayerProperties& it : layerProperties)
            {
                LOG("%s (v. %d.%d.%d %x) : %s\n", it.layerName, VK_VERSION_MAJOR(it.specVersion),
                    VK_VERSION_MINOR(it.specVersion), VK_VERSION_PATCH(it.specVersion), it.implementationVersion, it.description);
            }
        }

        // Get all extensions
        auto extensionProperties = getInstanceExtensions();

        std::vector<void*> featureStructs;
        if (fillFilteredNameArray(usedInstanceExtensions, extensionProperties, instanceExtensions, featureStructs) != VK_SUCCESS)
        {
            return false;
        }

        if (verboseAvailable)
        {
            LOG("\n");
            LOG("Available Instance Extensions :\n");
            for (const VkExtensionProperties& it : extensionProperties)
            {
                LOG("%s (v. %d)\n", it.extensionName, it.specVersion);
            }
        }

        if (verboseUsed)
        {
            LOG("______________________\n");
            LOG("Used Instance Layers :\n");
            for (const auto& it : usedInstanceLayers)
            {
                LOG("%s\n", it.c_str());
            }
            LOG("\n");
            LOG("Used Instance Extensions :\n");
            for (const auto& it : usedInstanceExtensions)
            {
                LOG("%s\n", it.c_str());
            }
        }

        std::vector<const char*> usedLayers;
        std::vector<const char*> usedExtensions;
        for (const auto& it : usedInstanceExtensions)
        {
            usedExtensions.push_back(it.c_str());
        }
        for (const auto& it : usedInstanceLayers)
        {
            usedLayers.push_back(it.c_str());
        }

        VkInstanceCreateInfo instanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(usedExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = usedExtensions.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(usedLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = usedLayers.data();
        instanceCreateInfo.pNext = instanceCreateInfoExt;

        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

        for (const auto& it : usedExtensions)
        {
            if (strcmp(it, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
            {
                initDebugUtils();
                break;
            }
        }

        return true;
    }

    inline VkBuffer createDedicatedBuffer(VkDevice device, VkBufferCreateInfo info, VkMemoryPropertyFlags properties)
    {
        VkBuffer bufferTemp = VK_NULL_HANDLE;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(device, &info, nullptr, &bufferTemp);
        assert(result == VK_SUCCESS);



        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, bufferTemp, &memRequirements);

        VkMemoryDedicatedAllocateInfo dedicatedInfo{ VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO };
        VkMemoryAllocateInfo memAllocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        VkMemoryAllocateFlagsInfo     flagsInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO };
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

        dedicatedInfo.pNext = memAllocInfo.pNext;
        memAllocInfo.pNext = &dedicatedInfo;

        dedicatedInfo.buffer = bufferTemp;
        dedicatedInfo.image = nullptr;

        flagsInfo.pNext = memAllocInfo.pNext;
        memAllocInfo.pNext = &flagsInfo;

        memAllocInfo.allocationSize = memRequirements.size;
        memAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &memAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, bufferTemp, bufferMemory, 0);

        return bufferTemp;
    }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }


    void initDebugUtils()
    {
        // Debug reporting system
        // Setup our pointers to the VK_EXT_debug_utils commands
        createDebugUtilsMessengerEXT =
            (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        destroyDebugUtilsMessengerEXT =
            (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        // Create a Debug Utils Messenger that will trigger our callback for any warning
        // or error.
        if (createDebugUtilsMessengerEXT != nullptr)
        {
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
            debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT       // For debug printf
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT  // GPU info, bug
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;   // Invalid usage
            debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT            // Other
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT       // Violation of spec
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;     // Non-optimal use
            debugMessengerCreateInfo.pfnUserCallback = debugMessengerCallback;
            debugMessengerCreateInfo.pUserData = this;
            VK_CHECK(createDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger));
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT        messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData) {

        SimplePathTracerApplication* ctx = reinterpret_cast<SimplePathTracerApplication*>(userData);

        if (ctx->debugIgnoreMessages.find(callbackData->messageIdNumber) != ctx->debugIgnoreMessages.end())
            return VK_FALSE;

        // Check for severity: default ERROR and WARNING
        if ((ctx->debugSeverity & messageSeverity) != messageSeverity)
            return VK_FALSE;

     ///   int level = LOGLEVEL_INFO;
        // repeating nvprintfLevel to help with breakpoints : so we can selectively break right after the print
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            LOG("VERBOSE: %s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            LOG("INFO: %s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LOG("WARNING: %s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            LOG("ERROR: %s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        {
            LOG("GENERAL: %s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else
        {
            LOG("%s \n --> %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        }

       // Don't bail out, but keep going.
       return VK_FALSE;
        
    }

    VkResult fillFilteredNameArray(std::vector<std::string>& used,
        const std::vector<VkExtensionProperties>& properties,
        const std::vector<Entry>& requested,
        std::vector<void*>& featureStructs)
    {
        for (const auto& itr : requested)
        {
            bool found = false;
            for (const auto& property : properties)
            {
                if (strcmp(itr.name.c_str(), property.extensionName) == 0 && (itr.version == 0 || itr.version == property.specVersion))
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                used.push_back(itr.name);
                if (itr.pFeatureStruct)
                {
                    featureStructs.push_back(itr.pFeatureStruct);
                }
            } else if (!itr.optional)
            {
                LOG("VK_ERROR_EXTENSION_NOT_PRESENT: %s - %d\n", itr.name.c_str(), itr.version);
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        return VK_SUCCESS;
    }

    bool initDevice(uint32_t deviceIndex) {
        assert(instance != nullptr);

        VkPhysicalDeviceGroupProperties physicalGroup;
        if (useDeviceGroups)
        {
            auto groups = getPhysicalDeviceGroups();
            assert(deviceIndex < static_cast<uint32_t>(groups.size()));
            physicalGroup = groups[deviceIndex];
            physicalDevice = physicalGroup.physicalDevices[0];
        } else
        {
            auto physicalDevices = getPhysicalDevices();
            assert(deviceIndex < static_cast<uint32_t>(physicalDevices.size()));
            physicalDevice = physicalDevices[deviceIndex];
        }

        if (verboseCompatibleDevices)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevice, &props);
            LOG("Using Device:\n");
            printPhysicalDeviceProperties(props);
        }

        initPhysicalInfo(physicalInfo, physicalDevice, apiMajor, apiMinor);

        // setup Vulkan queues
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<float> priorities;
        std::vector<QueueScore> queueScoresTemp;

        uint32_t maxQueueCount = 0;

        for (auto& it : physicalInfo.queueProperties) {
            maxQueueCount = std::max(maxQueueCount, it.queueCount);
        }

        priorities.resize(physicalInfo.queueProperties.size() * maxQueueCount);

        // init list with all maximum queue counts
        initQueueList(queueScoresTemp, nullptr, nullptr, 0);

        // queues needed per family
        std::vector<uint32_t> queueFamilyCounts(physicalInfo.queueProperties.size(), 0);

        for (auto& it : requestedQueues) {
            for (uint32_t i = 0; i < it.count; i++) {
                QueueScore queue = removeQueueListItem(queueScoresTemp, it.requiredFlags, 1.0f);
                if (!queue.score)
                {
                    // there were not enough queues left supporting the required flags
                    LOG("could not setup requested queue configuration\n");
                    return false;
                }

                priorities[queue.familyIndex * maxQueueCount + queueFamilyCounts[queue.familyIndex]] = it.priority;
                queueFamilyCounts[queue.familyIndex]++;
            }
        }

        // init requested families with family count
        for (uint32_t i = 0; i < physicalInfo.queueProperties.size(); ++i) {
            if (queueFamilyCounts[i]) {
                VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                queueInfo.queueFamilyIndex = i;
                queueInfo.queueCount = queueFamilyCounts[i];
                queueInfo.pQueuePriorities = priorities.data() + (i * maxQueueCount);
                
                queueCreateInfos.push_back(queueInfo);
            }
        }
        // setup available queues, now with actual requested counts and available priorities
        initQueueList(availableQueues, queueFamilyCounts.data(), priorities.data(), maxQueueCount);

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

        // version features and device extensions
        VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        Features11Old features11old;
        std::vector<void*> featureStructs;

        features2.features = physicalInfo.features10;
        features11old.read(physicalInfo.features11);

        if (apiMajor == 1 && apiMinor == 1) {
            features2.pNext = &features11old.multiview;

        }

        if (apiMajor == 1 && apiMinor >= 2) {
            features2.pNext = &physicalInfo.features11;
            physicalInfo.features11.pNext = &physicalInfo.features12;
            physicalInfo.features12.pNext = nullptr;
        }

        if (apiMajor == 1 && apiMinor >= 3) {
            physicalInfo.features12.pNext = &physicalInfo.features13;
            physicalInfo.features13.pNext = nullptr;
        }

        auto extensionProperties = getDeviceExtensions(physicalDevice);

        if (verboseAvailable) {
            LOG("_____________________________\n");
            LOG("Available Device Extensions :\n");
            for (const VkExtensionProperties& it : extensionProperties)
            {
                LOG("%s (v. %d)\n", it.extensionName, it.specVersion);
            }
        }

        std::vector<Entry> deviceExtensionsTemp = deviceExtensions;

        if (fillFilteredNameArray(usedDeviceExtensions, extensionProperties, deviceExtensionsTemp, featureStructs) != VK_SUCCESS) {
            cleanup();
            return false;
        }

        if (verboseUsed) {
            LOG("________________________\n");
            LOG("Used Device Extensions :\n");
            for (const auto& it : usedDeviceExtensions)
            {
                LOG("%s\n", it.c_str());
            }
            LOG("\n");
        }

        struct ExtensionHeader {
            VkStructureType sType;
            void* pNext;
        };

        if (!featureStructs.empty()) {
            // build up chain of all used extension features
            for (size_t i = 0; i < featureStructs.size(); i++)
            {
                auto* header = reinterpret_cast<ExtensionHeader*>(featureStructs[i]);
                header->pNext = i < featureStructs.size() - 1 ? featureStructs[i + 1] : nullptr;
            }

            ExtensionHeader* lastCoreFeature = (ExtensionHeader*)&features2;
            while (lastCoreFeature->pNext != nullptr)
            {
                lastCoreFeature = (ExtensionHeader*)lastCoreFeature->pNext;
            }
            lastCoreFeature->pNext = featureStructs[0];

            // query support
            vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);
        }
        // affects performance
        if (disableRobustBufferAccess)
        {
            features2.features.robustBufferAccess = VK_FALSE;
        }

        std::vector<const char*> usedDeviceExtensionsTemp;
        for (auto& it : usedDeviceExtensions) {
            usedDeviceExtensionsTemp.push_back(it.c_str());
        }

        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(usedDeviceExtensionsTemp.size());
        deviceCreateInfo.ppEnabledExtensionNames = usedDeviceExtensionsTemp.data();

        // Vulkan >= 1.1 uses pNext to enable features, and not pEnabledFeatures
        deviceCreateInfo.pEnabledFeatures = nullptr;
        deviceCreateInfo.pNext = &features2;

        // device group information
        VkDeviceGroupDeviceCreateInfo deviceGroupCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO };
        if (useDeviceGroups)
        {
            // add ourselves to the chain
            deviceGroupCreateInfo.pNext = deviceCreateInfo.pNext;
            deviceGroupCreateInfo.physicalDeviceCount = uint32_t(physicalGroup.physicalDeviceCount);
            deviceGroupCreateInfo.pPhysicalDevices = physicalGroup.physicalDevices;
            deviceCreateInfo.pNext = &deviceGroupCreateInfo;
        }
            
        VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

        if (result != VK_SUCCESS) {
            cleanup();
            return false;
        }

        graphicsComputeTransferQueue = createQueue(defaultQueueGraphicsComputerTransfer, "queueGraphicsComputeTransfer", defaultPriorityGraphicsComputeTransfer);
        computeQueue = createQueue(defaultQueueCompute, "queueCompute", defaultPriorityCompute);
        transferQueue = createQueue(defaultQueueTransfer, "queueTransfer", defaultPriorityTransfer);

        return true;
    }

    Queue createQueue(VkQueueFlags requiredFlags, const std::string& debugName, float priority)
    {
        if (!requiredFlags || availableQueues.empty())
        {
            return Queue();
        }

        QueueScore score = removeQueueListItem(availableQueues, requiredFlags, priority);
        if (!score.score)
        {
            return Queue();
        }

     
        Queue queue;
        queue.familyIndex = score.familyIndex;
        queue.queueIndex = score.queueIndex;
        queue.priority = score.priority;
        vkGetDeviceQueue(device, queue.familyIndex, queue.queueIndex, &queue.queue);

        return queue;
    }

    void initQueueList(std::vector<QueueScore>& list, const uint32_t* maxFamilyCounts, const float* priorities, uint32_t maxQueueCount) const 
    {
        for (uint32_t qF = 0; qF < physicalInfo.queueProperties.size(); qF++) {
            const auto& queueFamily = physicalInfo.queueProperties[qF];
            QueueScore score{ 0, qF, 0, 1.0f };

            for (uint32_t i = 0; i < 32; i++) {
                if (queueFamily.queueFlags & (1 << i)) {
                    score.score++;
                }
            }
            for (uint32_t qI = 0; qI < (maxFamilyCounts ? maxFamilyCounts[qF] : queueFamily.queueCount); ++qI) {
                score.queueIndex = qI;

                if (priorities) {
                    score.priority = priorities[qF * maxQueueCount + qI];
                }

                list.emplace_back(score);
            }
        }
        // Sort the queues for specialization, highest specialization has lowest score
        std::sort(list.begin(), list.end(), [](const QueueScore& lhs, const QueueScore& rhs) {
            if (lhs.score < rhs.score)
                return true;
            if (lhs.score > rhs.score)
                return false;
            if (lhs.priority > rhs.priority)
                return true;
            if (lhs.priority < rhs.priority)
                return false;
            return lhs.queueIndex < rhs.queueIndex;
        });
    }

    QueueScore removeQueueListItem(std::vector<QueueScore>& list, VkQueueFlags needFlags, float priority) 
    {
        for (uint32_t q = 0; q < list.size(); ++q) {
            const QueueScore& score = list[q];
            auto& family = physicalInfo.queueProperties[score.familyIndex];
            if ((family.queueFlags & needFlags) == needFlags && score.priority == priority) {
                QueueScore queue = score;
                queue.familyIndex = score.familyIndex;
                queue.queueIndex = score.queueIndex;
                list.erase(list.begin() + q);
                return queue;
            }
        }
        return {};
    }

    std::string getVersionString(uint32_t version)
    {
        char str[64];
        sprintf_s(str, "%d.%d.%d", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
        return std::string(str);
    }

    std::string getVendorName(uint32_t vendorID)
    {
        switch (vendorID)
        {
        case 0x1002:
            return ("AMD");
        case 0x1010:
            return ("ImgTec");
        case 0x10DE:
            return ("NVIDIA");
        case 0x13B5:
            return ("ARM");
        case 0x5143:
            return ("Qualcomm");
        case 0x8086:
            return ("INTEL");
        }
        return ("Unknown Vendor");
    }

    std::string getDeviceType(uint32_t deviceType)
    {
        switch (deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return ("Other");
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return ("Integrated GPU");
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return ("Discrete GPU");
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return ("Virtual GPU");
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return ("CPU");
        }
        return ("Unknown");
    }

    void printPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties)
    {
        LOG(" - Device Name    : %s\n", properties.deviceName);
        LOG(" - Vendor         : %s\n", getVendorName(properties.vendorID).c_str());
        LOG(" - Driver Version : %s\n", getVersionString(properties.driverVersion).c_str());
        LOG(" - API Version    : %s\n", getVersionString(properties.apiVersion).c_str());
        LOG(" - Device Type    : %s\n", getDeviceType(properties.deviceType).c_str());
    }


    VkResult fillFilteredNameArray(std::vector<std::string>& used,
        const std::vector<VkLayerProperties>& properties,
        const std::vector<Entry>& requested)
    {
        for (const auto& itr : requested)
        {
            bool found = false;
            for (const auto& property : properties)
            {
                if (strcmp(itr.name.c_str(), property.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                used.push_back(itr.name);
            } else if (itr.optional == false)
            {
                LOG("Requiered layer not found: %s\n", itr.name.c_str());
                return VK_ERROR_LAYER_NOT_PRESENT;
            }
        }

        return VK_SUCCESS;
    }

    std::vector<VkLayerProperties> getInstanceLayers()
    {
        uint32_t count;
        std::vector<VkLayerProperties> layerProperties;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));
        layerProperties.resize(count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&count, layerProperties.data()));
        layerProperties.resize(std::min(layerProperties.size(), size_t(count)));
        return layerProperties;
    }

    std::vector<VkExtensionProperties> getInstanceExtensions()
    {
        uint32_t count;
        std::vector<VkExtensionProperties> extensionProperties;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
        extensionProperties.resize(count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, extensionProperties.data()));
        extensionProperties.resize(std::min(extensionProperties.size(), size_t(count)));
        return extensionProperties;
    }

    std::vector<VkPhysicalDevice> getPhysicalDevices() {
        uint32_t nbElems;
        std::vector<VkPhysicalDevice> physicalDevices;
        VK_CHECK(vkEnumeratePhysicalDevices(instance, &nbElems, nullptr));
        physicalDevices.resize(nbElems);
        VK_CHECK(vkEnumeratePhysicalDevices(instance, &nbElems, physicalDevices.data()));
        return physicalDevices;
    }

    void initPhysicalInfo(PhysicalDeviceInfo& info, VkPhysicalDevice physicalDevice, uint32_t versionMajor, uint32_t versionMinor)
    {
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &info.memoryProperties);
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
        info.queueProperties.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, info.queueProperties.data());

        // for queries and device creation
        VkPhysicalDeviceFeatures2   features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        Properties11Old             properties11old;
        Features11Old               features11old;

        if (versionMajor == 1 && versionMinor == 1)
        {
            features2.pNext = &features11old.multiview;
            properties2.pNext = &properties11old.maintenance3;
        } else if (versionMajor == 1 && versionMinor >= 2)
        {
            features2.pNext = &info.features11;
            info.features11.pNext = &info.features12;
            info.features12.pNext = nullptr;

            info.properties12.driverID = VK_DRIVER_ID_NVIDIA_PROPRIETARY;
            info.properties12.supportedDepthResolveModes = VK_RESOLVE_MODE_MAX_BIT;
            info.properties12.supportedStencilResolveModes = VK_RESOLVE_MODE_MAX_BIT;

            properties2.pNext = &info.properties11;
            info.properties11.pNext = &info.properties12;
            info.properties12.pNext = nullptr;
        }

        if (versionMajor == 1 && versionMinor >= 3)
        {
            info.features12.pNext = &info.features13;
            info.features13.pNext = nullptr;
            info.properties12.pNext = &info.properties13;
            info.properties13.pNext = nullptr;
        }

        vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);
        vkGetPhysicalDeviceProperties2(physicalDevice, &properties2);

        info.properties10 = properties2.properties;
        info.features10 = features2.features;

        if (versionMajor == 1 && versionMinor == 1)
        {
            properties11old.write(info.properties11);
            features11old.write(info.features11);
        }
    }

    std::vector<VkExtensionProperties> getDeviceExtensions(VkPhysicalDevice physicalDevice)
    {
        uint32_t count;
        std::vector<VkExtensionProperties> extensionProperties;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr));
        extensionProperties.resize(count);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensionProperties.data()));
        extensionProperties.resize(std::min(extensionProperties.size(), size_t(count)));
        return extensionProperties;
    }

    std::vector<VkPhysicalDeviceGroupProperties> getPhysicalDeviceGroups()
    {
        uint32_t deviceGroupCount;
        std::vector<VkPhysicalDeviceGroupProperties> groups;
        VK_CHECK(vkEnumeratePhysicalDeviceGroups(instance, &deviceGroupCount, nullptr));
        groups.resize(deviceGroupCount);
        VK_CHECK(vkEnumeratePhysicalDeviceGroups(instance, &deviceGroupCount, groups.data()));
        return groups;
    }

    std::vector<std::string> checkEntryArray(const std::vector<VkExtensionProperties>& properties,
        const std::vector<Entry>& requested)
    {
        std::vector<std::string> missingExtensions;
        for (const auto& requestedExtension : requested)
        {
            bool found = false;
            if (requestedExtension.optional)
                continue;  // Looking only for non-optional extensions
            for (const auto& property : properties)
            {
                if (strcmp(requestedExtension.name.c_str(), property.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                missingExtensions.push_back(requestedExtension.name);
        }

        return missingExtensions;
    }

    std::vector<uint32_t> getCompatibleDevices()
    {
        assert(instance != nullptr);

        std::vector<std::pair<bool, uint32_t>> compatibleDevices;
        std::vector<uint32_t> sortedDevices;
        std::vector<VkPhysicalDeviceGroupProperties> groups;
        std::vector<VkPhysicalDevice> physicalDevices;

        uint32_t nbElems;
        if (useDeviceGroups)
        {
            groups = getPhysicalDeviceGroups();
            nbElems = static_cast<uint32_t>(groups.size());
        } else
        {
            physicalDevices = getPhysicalDevices();
            nbElems = static_cast<uint32_t>(physicalDevices.size());
        }

        if (verboseCompatibleDevices)
        {
            LOG("____________________\n");
            LOG("Devices : %d\n", nbElems);
        }

        uint32_t compatible = 0;
        for (uint32_t elemId = 0; elemId < nbElems; elemId++)
        {
            VkPhysicalDevice physicalDevice = useDeviceGroups ? groups[elemId].physicalDevices[0] : physicalDevices[elemId];

            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevice, &props);

            bool discreteGpu = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

            if (verboseCompatibleDevices)
                LOG("%d: %s\n", elemId, props.deviceName);

            const std::vector<VkExtensionProperties> extensionProperties = getDeviceExtensions(physicalDevice);
            const std::vector<std::string> missingExtensions = checkEntryArray(extensionProperties, deviceExtensions);

            if (missingExtensions.empty())
            {
                if (verboseCompatibleDevices)
                    LOG("  - Compatible \n");
                compatibleDevices.emplace_back(discreteGpu, elemId);
                compatible++;
            } else
            {
                if (verboseCompatibleDevices)
                {
                    LOG("  - Missing extensions: ");
                    for (auto& e : missingExtensions)
                        LOG("%s ", e.c_str());
                    LOG("\n");
                }
            }
        }

        if (verboseCompatibleDevices)
        {
            LOG("Compatible physical devices found : ");
            if (compatible > 0)
            {
                LOG("%d\n", compatible);
            } else
            {
                LOG("OMG... NONE !!\n");
            }
        }

        // Sorting by discrete GPU
        std::sort(compatibleDevices.begin(), compatibleDevices.end(), [](auto a, auto b) { return a.first > b.first; });
        sortedDevices.reserve(compatibleDevices.size());
        for (const auto& d : compatibleDevices)
            sortedDevices.push_back(d.second);

        return sortedDevices;
    }

    void mainLoop() {
    
    }

    void cleanup() {

        if (buffer) {
            vkDestroyBuffer(device, buffer, nullptr);
        }

        if (bufferMemory) {
            vkFreeMemory(device, bufferMemory, nullptr);
        }


        if (device)
        {
            VK_CHECK(vkDeviceWaitIdle(device));

            vkDestroyDevice(device, nullptr);
            device = VK_NULL_HANDLE;
        }
        if (destroyDebugUtilsMessengerEXT)
        {
            // Destroy the Debug Utils Messenger
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        if (instance)
        {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }

        usedInstanceExtensions.clear();
        usedInstanceLayers.clear();
        usedDeviceExtensions.clear();

        createDebugUtilsMessengerEXT = nullptr;
        destroyDebugUtilsMessengerEXT = nullptr;
        debugMessenger = nullptr;
    }

protected:

    bool verboseCompatibleDevices = true;
    bool verboseUsed = true;
    bool verboseAvailable =
#ifndef NDEBUG
        true;
#else
        false;
#endif

    bool useDeviceGroups = false;
    uint32_t compatibleDeviceIndex = 0;

    uint32_t apiMajor;
    uint32_t apiMinor;

    std::vector<QueueSetup> requestedQueues;
    std::vector<QueueScore> availableQueues;

    Queue graphicsComputeTransferQueue;
    Queue computeQueue;
    Queue transferQueue;

    VkQueueFlags defaultQueueGraphicsComputerTransfer = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    VkQueueFlags defaultQueueTransfer = VK_QUEUE_TRANSFER_BIT;
    VkQueueFlags defaultQueueCompute = VK_QUEUE_COMPUTE_BIT;
    float        defaultPriorityGraphicsComputeTransfer = 1.0f;
    float        defaultPriorityTransfer = 1.0f;
    float        defaultPriorityCompute = 1.0f;

    std::vector<Entry> instanceExtensions;
    std::vector<Entry> instanceLayers;
    std::vector<Entry> deviceExtensions;

    std::vector<std::string> usedInstanceLayers;
    std::vector<std::string> usedInstanceExtensions;
    std::vector<std::string> usedDeviceExtensions;

    VkInstance instance = VK_NULL_HANDLE;
    void* instanceCreateInfoExt = nullptr;

    VkPhysicalDevice physicalDevice;
    PhysicalDeviceInfo physicalInfo;
    VkDevice device;

    PFN_vkCreateDebugUtilsMessengerEXT  createDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT = nullptr;
    VkDebugUtilsMessengerEXT            debugMessenger = nullptr;

    // buffers
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    bool disableRobustBufferAccess = true;

    public:

    std::set<int32_t> debugIgnoreMessages;
    uint32_t debugSeverity{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT };

};

int main(int argc, char** args) {
    SimplePathTracerApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
};



