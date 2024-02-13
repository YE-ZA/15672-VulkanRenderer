#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm> // for std::clamp
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint> // for uint32_t
#include <limits>  // for std::numeric_limits
#include <array>
#include <optional>
#include <set>

#include "CullingHelper.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities; // min/max number of images in swap chain, min/max width and height of images
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 normal;
};

class VulkanHelper
{
public:
    void initVulkan(GLFWwindow *window);
    void initScene(std::vector<std::string> &vertexData, size_t uboSize, std::vector<uint32_t> &in_counts, std::vector<uint32_t> &in_strides, std::vector<uint32_t> &in_posOffsets, std::vector<uint32_t> &in_normalOffsets, std::vector<uint32_t> &in_colorOffsets, std::vector<std::string> &in_posFormats, std::vector<std::string> &in_normalFormats, std::vector<std::string> &in_colorFormats, std::vector<uint32_t> &in_instanceCounts);
    void drawFrame(GLFWwindow *window, const std::vector<glm::mat4> &uniformData, glm::mat4 view, glm::mat4 proj, bool debug);
    void cleanup();

    VkDevice getDevice();
    void setCullingFrustum(float right, float top, float near, float far);

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    std::vector<uint32_t> counts;
    std::vector<uint32_t> strides;
    std::vector<uint32_t> posOffsets;
    std::vector<uint32_t> normalOffsets;
    std::vector<uint32_t> colorOffsets;
    std::vector<std::string> posFormats;
    std::vector<std::string> normalFormats;
    std::vector<std::string> colorFormats;
    std::vector<uint32_t> instanceCounts;

    VkVertexInputBindingDescription2EXT vertexBindingDescriptions{
        .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor = 1};
    VkVertexInputAttributeDescription2EXT vertexAttributeDescriptions[3]{
        {.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
         .location = 0,
         .binding = 0,
         .format = VK_FORMAT_R32G32B32_SFLOAT,
         .offset = offsetof(Vertex, pos)},
        {.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
         .location = 1,
         .binding = 0,
         .format = VK_FORMAT_R32G32B32_SFLOAT,
         .offset = offsetof(Vertex, normal)},
        {.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
         .location = 2,
         .binding = 0,
         .format = VK_FORMAT_R8G8B8A8_UNORM,
         .offset = offsetof(Vertex, color)}};

    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceMemory> vertexBufferMemorys;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void *> uniformBuffersMapped;
    std::vector<AABB> aabbs;
    std::vector<glm::mat4> aabbTransforms;
    CullingFrustum frustum;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    void cleanupSwapChain();
    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

    void createSurface(GLFWwindow *window);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain(GLFWwindow *window);
    void recreateSwapChain(GLFWwindow *window);
    void createImageViews();

    void createRenderPass();
    void createDepthResources();
    void createFramebuffers();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void updateVertexDescriptions(uint32_t stride, uint32_t posOffset, uint32_t normalOffset, uint32_t colorOffset, std::string posFormat, std::string normalFormat, std::string colorFormat);
    void updateUniformBuffer(uint32_t currentImage, const std::vector<glm::mat4> &uniformData, glm::mat4 view, glm::mat4 proj, bool debug);

    void createVertexBuffer(const char *meshData, size_t size);
    void createUniformBuffers(size_t size);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createDescriptorPool();
    void createDescriptorSets(size_t size);

    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);
    static std::vector<char> readFile(const std::string &filename);

    PFN_vkCmdSetVertexInputEXT pfnVkCmdSetVertexInputEXT = NULL;
};
