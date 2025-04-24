#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_judge.h"

#include "G_file/G_file.h"

extern VK_ALL allInOne;

SDL_PixelFormat getSDL_PixelFormat(Uint8 channel)
{
    switch (channel)
    {
        case 1:
        return SDL_PIXELFORMAT_INDEX8;

        case 3:
        return SDL_PIXELFORMAT_RGB24;

        case 4:
        return SDL_PIXELFORMAT_RGBA32;
        
        default:
        return SDL_PIXELFORMAT_UNKNOWN;
    }
}
VkFormat getVulkanFormat(Uint8 channel, FormatQualifier flags)
{
    switch (channel)
    {
        case 1:
        return (VkFormat)((Uint32)VK_FORMAT_R8_UNORM + (Uint32)flags);

        case 3:
        return (VkFormat)((Uint32)VK_FORMAT_R8G8B8_UNORM + (Uint32)flags);

        case 4:
        return (VkFormat)((Uint32)VK_FORMAT_R8G8B8A8_UNORM + (Uint32)flags);

        default:
        return VK_FORMAT_UNDEFINED;
    }
}
void createTextureImageFromFile(PathType type, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem)
{
    Uint32 width, height;
    Uint8 channel;
    void * pixels = readPNG(type, &width, &height, &channel);
    VkDeviceSize imageSize = width * height * channel;
    //printf("imagesize: %u\n", imageSize);

    createTextureImageFromMem(pixels, width, height, imageSize, format, pTextureImage, pTextureImageMem);
}
void createTextureImageFromMem(void * pixels, Uint32 width, Uint32 height, VkDeviceSize imageSize, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(&stagingBuffer, &stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, NULL, 0, 0);

    void * data;
    vkMapMemory(allInOne.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(allInOne.device, stagingBufferMemory);

    createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pTextureImage, pTextureImageMem);

    transitionImageLayout(NULL, *pTextureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, 1);
    copyBufferToImage(NULL, pTextureImage, width, height, &stagingBuffer);

    transitionImageLayout(NULL, *pTextureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

    vkDestroyBuffer(allInOne.device, stagingBuffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(allInOne.device, stagingBufferMemory, allInOne.pAllocationCallbacks);
}
unsigned char * readPNG(PathType type, Uint32 * pWidth, Uint32 * pHeight, Uint8 * pChannel)
{
    SDL_IOStream * io = SDL_IOFromFile(getPath(type), "rb");
    if (io == NULL) return NULL;
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
VkResult copyBufferToImage(VkCommandBuffer commandBuffer, VkImage * pImage, Uint32 width, Uint32 height, VkBuffer * pBuffer)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer singleTimeCommandBuffer = NULL;
    if (commandBuffer == NULL)
    {
        result |= beginSingleTimeCommands(allInOne.graphicCommandPool, &singleTimeCommandBuffer);
    }
    else
    {
        beginCommandBuffer(commandBuffer);
    }

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

    if (commandBuffer == NULL)
    {
        vkCmdCopyBufferToImage(singleTimeCommandBuffer, *pBuffer, *pImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        result |= endSingleTimeCommands(allInOne.graphicCommandPool, getGraphic2dQueue(), &singleTimeCommandBuffer);
    }
    else
    {
        vkCmdCopyBufferToImage(commandBuffer, *pBuffer, *pImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        vkEndCommandBuffer(commandBuffer);
    }

    return result;
}
void createTextureImageView(VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView)
{
    resultVulkan(createImageView(*pTextureImage, format, aspectFlags, pTextureImageView), 0);
}
// png_bytep readPNG(PathType type, Uint32 * pWidth, Uint32 * pHeight, Uint8 * pChannel)
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