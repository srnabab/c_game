#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_file/G_file.h"

extern VK_ALL allInOne;

void createTextureImage(VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, PathType type, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem)
{
    FuncCode code = createTextureImageF;

    uint32_t width, height;
    width = height = 0;
    uint8_t channel = 0;
    void * pixels = readPNG(type, &width, &height, &channel);
    VkDeviceSize imageSize = width * height * channel;
    //printf("imagesize: %u\n", imageSize);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    resultVulkan(createBuffer(&stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 1, pixels);

    void * data;
    resultVulkan(vkMapMemory(*allInOne.pDevice, stagingBufferMemory, 0, imageSize, 0, &data), code, 1, pixels);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(*allInOne.pDevice, stagingBufferMemory);

    resultVulkan(createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pTextureImage, pTextureImageMem), code, 1, pixels);

    resultVulkan(transitionImageLayout(pCommandPool, pGraphicQueue, pTextureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL), code, 1, pixels);
    resultVulkan(copyBufferToImage(pCommandPool, pGraphicQueue, pTextureImage, width, height, &stagingBuffer), code, 1, pixels);

    resultVulkan(transitionImageLayout(pCommandPool, pGraphicQueue, pTextureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), code, 1, pixels);
    
    SDL_free(pixels);

    vkDestroyBuffer(*allInOne.pDevice, stagingBuffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(*allInOne.pDevice, stagingBufferMemory, allInOne.pAllocationCallbacks);
}
unsigned char * readPNG(PathType type, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel)
{
    SDL_IOStream * io = SDL_IOFromFile(getPath(type), "rb");
    SDL_Surface * png = IMG_LoadPNG_IO(io);
    SDL_CloseIO(io);

    *pWidth = png->w;
    *pHeight = png->h;
    
    if (png->format == SDL_PIXELFORMAT_INDEX8) *pChannel = 1;
    else if ((png->format == SDL_PIXELFORMAT_RGB24)) *pChannel = 3;
    else if ((png->format == SDL_PIXELFORMAT_RGBA32) || (png->format == SDL_PIXELFORMAT_ARGB32)) *pChannel = 4;

    unsigned char * pixels = (unsigned char*)SDL_malloc((*pWidth) * (*pHeight) * (*pChannel) * sizeof(unsigned char));
    memcpy(pixels, png->pixels, (*pWidth) * (*pHeight) * (*pChannel) * sizeof(unsigned char));
    SDL_DestroySurface(png);

    return pixels;
}
VkResult copyBufferToImage(VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, uint32_t width, uint32_t height, VkBuffer * pBuffer)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer commandBuffer = NULL;
    result |= beginSingleTimeCommands(pCommandPool, &commandBuffer);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, *pBuffer, *pImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    
    result |= endSingleTimeCommands(pCommandPool, pGraphcisQueue, &commandBuffer);

    return result;
}
void createTextureImageView(VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView)
{
    FuncCode code = createTextureImageViewF;
    
    resultVulkan(createImageView(pTextureImage, format, aspectFlags, pTextureImageView), code, 0);
}
void createTextureSampler(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkSampler * pTextureSampler)
{
    FuncCode code = createTextureSamplerF;

    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(*pPhysicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = NULL;
    samplerInfo.flags = 0;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    resultVulkan(vkCreateSampler(*pDevice, &samplerInfo, allInOne.pAllocationCallbacks, pTextureSampler), code, 0);
}
// png_bytep readPNG(PathType type, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel)
// {
//     SDL_IOStream * fp;
//     if ((fp = SDL_IOFromFile(getPath(type), "rb+")) == NULL)
//     {
//         fprintf(stderr, "open file %s failed\n", getPath(type));
//         exit(0);
//     }

//     png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//     if (!png) {
//         fprintf(stderr, "png_create_read_struct failed\n");
//         SDL_CloseIO(fp);
//         exit(EXIT_FAILURE);
//     }

//     png_infop info = png_create_info_struct(png);
//     if (!info) {
//         fprintf(stderr, "png_create_info_struct failed\n");
//         png_destroy_read_struct(&png, NULL, NULL);
//         SDL_CloseIO(fp);
//         exit(EXIT_FAILURE);
//     }

//     if (setjmp(png_jmpbuf(png))) {
//         fprintf(stderr, "Error during png reading\n");
//         png_destroy_read_struct(&png, &info, NULL);
//         SDL_CloseIO(fp);
//         exit(EXIT_FAILURE);
//     }

//     *pChannel = 4;

//     png_init_io(png, fp);

//     // Read the PNG file information
//     png_read_info(png, info);

//     // Get information about the image
//     *pWidth = png_get_image_width(png, info);
//     *pHeight = png_get_image_height(png, info);
//     png_byte bit_depth = png_get_bit_depth(png, info);
//     png_byte color_type = png_get_color_type(png, info);

//     // Handle different color formats
//     if (color_type == PNG_COLOR_TYPE_PALETTE) {
//         png_set_palette_to_rgb(png);  // Convert palette images to RGB
//     }
//     if (color_type == PNG_COLOR_TYPE_RGB)
//     {
//         *pChannel = 3;
//     }
//     if (color_type == PNG_COLOR_TYPE_GRAY)
//     {
//         *pChannel = 1;
//     }
//     if (png_get_valid(png, info, PNG_INFO_tRNS)) {
//         png_set_tRNS_to_alpha(png);  // Convert transparency to alpha
//     }
//     if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
//     {
//         *pChannel = 2;
//     }
//     if (bit_depth == 16) {
//         png_set_strip_16(png);  // Strip 16-bit to 8-bit
//     }

//     png_set_interlace_handling(png);  // Handle interlacing if necessary
//     png_read_update_info(png, info);

//     // Allocate memory for the image data
//     png_size_t rowbytes = png_get_rowbytes(png, info);

//     png_bytep image_data = (png_bytep)SDL_malloc(rowbytes * *pHeight);
//     // 检查内存是否分配成功
//     if (!image_data) {
//         png_destroy_read_struct(&png, &info, NULL);
//         SDL_CloseIO(fp);
//         exit(0);
//     }

//     // 创建行指针数组，指向image_data中的各行
//     png_bytep *row_pointers = (png_bytep *)SDL_malloc(sizeof(png_bytep) * *pHeight);
//     for (png_uint_32 i = 0; i < *pHeight; i++) {
//         row_pointers[i] = image_data + i * rowbytes;
//     }

//     // 读取PNG文件中的图像数据
//     png_read_image(png, row_pointers);

//     // Pixel data is now fully decoded and filtered, ready for use
//     logMessage("Width: %u, Height: %u\n", *pWidth, *pHeight);
//     //printf("Bit Depth: %u, Color Type: %u\n", bit_depth, color_type);

//     //exit(0);
//     /*FILE * ffp = fopen("test.txt", "w");

//     // Access pixel data
//     for (png_uint_32 y = 0; y < *pHeight; y++) {
//         png_bytep row = row_pointers[y];
//         for (png_uint_32 x = 0; x < *pWidth; x++) {
//             png_bytep px = &(row[x * *pChannel]); // Assuming RGBA
//             //if (y == 2000)
//                 fprintf(ffp, "Pixel[%u,%u]: (%u, %u, %u", x, y, px[0], px[1], px[2]);
//                 if (*pChannel == 4)
//                     fprintf(ffp, ", %u)\n", px[3]);
//                 else
//                     fprintf(ffp, ")\n");
//         }
//     }*/

//     // Process image data here (e.g., display it or modify it)
//     SDL_free(row_pointers);
//     png_destroy_read_struct(&png, &info, NULL);
//     SDL_CloseIO(fp);

//     return image_data;
// }