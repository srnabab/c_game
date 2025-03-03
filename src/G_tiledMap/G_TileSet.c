#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

// #include <stdio.h>

// .tsd format
// 4byte tile width
// 4byte tile height
// 4byte tile property <count>
// sequently 4byte index <count>byte properties
// 4byte crc32 for index and properties

//.tsdI format
// 4byte row
// 4byte col
// sequently 4byte index
// 4byte crc32 for indeices

#define PROPERTY_COUNT 1

struct _NMI
{
    Uint32 n, m, i, total;
};
typedef struct _NMI NMI;

static unsigned char * set;
static Uint32 ** setOffset;

static unsigned char * image;
static Uint32 ** imageOffset;


static int calculateFunc(void * arg)
{
    Uint32 i = ((NMI*)arg)->i;
    Uint32 m = ((NMI*)arg)->m;
    Uint32 n = ((NMI*)arg)->n;
    Uint32 total = ((NMI*)arg)->total;
    // SDL_Log("inner n: %u\n", n);
    for (Uint32 j = 0;j < total;j++)
    {
        // SDL_Log("image: %u, set: %u\n", image[imageOffset[i][n] + j], set[setOffset[m][n] + j]);
        if (image[imageOffset[i][n] + j] != set[setOffset[m][n] + j])
        {
            // SDL_Log("return 1\n");

            return 1;
        }
    }

    return 0;
}
static char * printByProIndex(Uint32 index)
{
    if (index == 0) return "show";

    return "";
}
static unsigned char * readPNG(char * path, Uint32 * pWidth, Uint32 * pHeight, Uint8 * pChannel)
{
    SDL_IOStream * io = SDL_IOFromFile(path, "rb");
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
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        SDL_Log("Usage: %s [mode](0:tile set, 1:tile map index generate with tile set) \n0:[image.png] [width] [height]\n1:[set.png] [set.tsd] [image.png]\n", argv[0]);
        return 1;
    }
    Uint32 i, j, k, m, n, p;

    if (SDL_atoi(argv[1]) == 1)
    {
        SDL_IOStream * tsd = SDL_IOFromFile(argv[3], "rb");
        Uint32 split_width, split_height;
        SDL_ReadIO(tsd, &split_width, sizeof(Uint32));
        SDL_ReadIO(tsd, &split_height, sizeof(Uint32));
        SDL_CloseIO(tsd);

        Uint32 setWidth, setHeight;
        Uint8 setChannel;
        set = readPNG(argv[2], &setWidth, &setHeight, &setChannel);
        Uint32 setRow, setCol;
        setRow = setHeight / split_height;
        setCol = setWidth / split_width;
        // SDL_Log("set row: %u, set col: %u, total: %u\n", setRow, setCol, setRow * setCol);
        setOffset = (Uint32**)SDL_malloc(setRow * setCol * sizeof(Uint32*));
        for (i = 0;i < setRow;i++)
        for (j = 0;j < setCol;j++)
        {
            setOffset[i * setCol + j] = (Uint32*)SDL_malloc(split_height * sizeof(Uint32));
            for (k = 0;k < split_height;k++)
            {
                setOffset[i * setCol + j][k] = setWidth * setChannel * k + j * split_width * setChannel + i * setWidth * split_height * setChannel;
                // SDL_Log("set offset: %u\n", setOffset[i * setCol + j][k]);
            }
        }

        Uint32 imageWidth, imageHeight;
        Uint8 imageChannel;
        image = readPNG(argv[4], &imageWidth, &imageHeight, &imageChannel);
        Uint32 imageRow, imageCol;
        imageRow = imageHeight / split_height;
        imageCol = imageWidth / split_width;
        // SDL_Log("image row: %u, image col: %u, total: %u\n", imageRow, imageCol, imageRow * imageCol);
        Uint32 * index = (Uint32*)SDL_malloc(imageRow * imageCol * sizeof(Uint32));
        imageOffset = (Uint32**)SDL_malloc(imageRow * imageCol * sizeof(Uint32*));
        for (i = 0;i < imageRow;i++)
        for (j = 0;j < imageCol;j++)
        {
            // SDL_Log("image offset index: %u\n", i * imageCol + j);
            imageOffset[i * imageCol + j] = (Uint32*)SDL_malloc(split_height * sizeof(Uint32));
            for (k = 0;k < split_height;k++)
            {
                imageOffset[i * imageCol + j][k] = imageWidth * imageChannel * k + j * split_width * imageChannel + i * imageWidth * split_height * imageChannel;
                // SDL_Log("image offset: %u\n", imageOffset[i * imageCol + j][k]);
            }
        }

        int result, result2;
        NMI * prama = (NMI*)SDL_malloc(split_height * sizeof(NMI));
        SDL_Thread ** threads = (SDL_Thread**)SDL_malloc(split_height * sizeof(SDL_Thread*));
        for (i = 0;i < imageRow * imageCol;i++)
        {
            // SDL_Log("\ni: %d\n", i);
            for (m = 0;m < setRow * setCol;m++)
            {
                // SDL_Log("\nm: %u\n", m);
                result2 = result = 0;
                for (n = 0;n < split_height;n++)
                {
                    prama[n].n = n;
                    prama[n].m = m;
                    prama[n].i = i;
                    prama[n].total = split_height * imageChannel;
                    threads[n] = SDL_CreateThread(calculateFunc, "calculate", prama + n);
                    // SDL_Log("thread%u\n", n);
                }
                for (n = 0;n < split_height;n++)
                {
                    SDL_WaitThread(threads[n], &result);
                    if (result) result2 = result;
                }
                if (!result2)
                {
                    // SDL_Log("m: %u\n", m);
                    break;
                }
            }
            if (m != setRow * setCol) index[i] = m;
            else index[i] = -1;

            // SDL_Log("index: %d\n", index[i]);

            SDL_free(imageOffset[i]);
        }

        SDL_free(prama);
        SDL_free(threads);

        SDL_free(imageOffset);
        for (m = 0;m < setRow * setCol;m++) SDL_free(setOffset[m]);
        SDL_free(setOffset);

        char path[255];
        SDL_strlcpy(path, argv[0], strlen(argv[0]));
        char * slash = SDL_strrchr(path, '\\');
        memcpy(slash, argv[4] + 1, SDL_strlen(argv[4] + 1));
        char * dot = SDL_strrchr(path, '.');
        *dot = '\0';
        memcpy(dot, ".tsdI", SDL_strlen(".tsdI") + 1);
        // SDL_Log(path);
        SDL_IOStream * indexFile = SDL_IOFromFile(path, "wb+");

        Uint32 crc32 = SDL_crc32(0, index, sizeof(Uint32) * imageRow * imageCol);
        SDL_WriteIO(indexFile, &imageRow, sizeof(Uint32));
        SDL_WriteIO(indexFile, &imageCol, sizeof(Uint32));
        SDL_WriteIO(indexFile, index, sizeof(Uint32) * imageRow * imageCol);
        SDL_WriteIO(indexFile, &crc32, sizeof(Uint32));
        SDL_CloseIO(indexFile);

        SDL_free(index);
    }
    else if (SDL_atoi(argv[1]) == 0)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return 1;
        }

        SDL_Window *window;
        SDL_Renderer *renderer;
        bool res = SDL_CreateWindowAndRenderer("tile pic", 800, 700, SDL_WINDOW_ALWAYS_ON_TOP, &window, &renderer);
        if (window == NULL || renderer == NULL)
        {
            SDL_Log("window created failed: %s", SDL_GetError());
            SDL_Log("renderer created failed: %s", SDL_GetError());
            SDL_Quit();
            return 1;
        }

        SDL_IOStream *stream = SDL_IOFromFile(argv[2], "rb");
        if (stream == NULL)
        {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        SDL_Surface *imageSurface = IMG_LoadPNG_IO(stream);
        if (imageSurface == NULL)
        {
            SDL_CloseIO(stream);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        SDL_CloseIO(stream);

        SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
        if (imageTexture == NULL)
        {
            SDL_Log("Texture created failed: ", SDL_GetError());
            SDL_DestroySurface(imageSurface);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        SDL_DestroySurface(imageSurface);

        SDL_SetTextureScaleMode(imageTexture, SDL_SCALEMODE_NEAREST);

        Uint32 width = imageTexture->w;
        Uint32 height = imageTexture->h;

        Uint32 split_width = SDL_atoi(argv[3]);
        Uint32 split_height = SDL_atoi(argv[4]);

        Uint32 col = width / split_width;
        Uint32 row = height / split_height;

        char path[255];
        SDL_strlcpy(path, argv[0], strlen(argv[0]));
        char * slash = SDL_strrchr(path, '\\');
        memcpy(slash, argv[2] + 1, SDL_strlen(argv[2] + 1));
        char * dot = SDL_strrchr(path, '.');
        *dot = '\0';
        memcpy(dot, ".tsd", SDL_strlen(".tsd") + 1);
        SDL_Log(path);
        
        unsigned char * data = (unsigned char*)SDL_malloc((sizeof(Uint32) + sizeof(bool) * PROPERTY_COUNT) * row * col);

        bool ** properties = (bool**)SDL_malloc(row * col * sizeof(bool*));
        for (i = 0;i < row * col;i++)
        {
            properties[i] = (bool*)SDL_calloc(PROPERTY_COUNT, sizeof(bool));
        }

        SDL_IOStream * file = SDL_IOFromFile(path, "rb+");
        if (file == NULL)
        {
            file = SDL_IOFromFile(path, "wb+");
        }
        else
        {
            SDL_SeekIO(file, 3 * sizeof(Uint32), SDL_IO_SEEK_SET);
            SDL_ReadIO(file, data, (sizeof(Uint32) + sizeof(bool) * 1) * row * col);
            Uint32 crc = SDL_crc32(0, data, (sizeof(Uint32) + sizeof(bool) * 1) * row * col);
            Uint32 crc_check;
            SDL_ReadIO(file, &crc_check, sizeof(Uint32));
            SDL_Log("crc: %u, file: %u", crc, crc_check);
            if (crc != crc_check) return 1;
            for (i = 0;i < row * col;i++)
            {
                for (j = 0;j < PROPERTY_COUNT;j++)
                {
                    properties[i][j] = data[(sizeof(Uint32) + sizeof(bool) * PROPERTY_COUNT) * i + sizeof(Uint32) + sizeof(bool) * j];
                }
            }
        }
        if (file == NULL)
        {
            SDL_Log("file created failed: ", SDL_GetError());
            SDL_DestroySurface(imageSurface);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        SDL_Event event;
        bool quit = false;
        SDL_Event preEvent = {0};
        SDL_StartTextInput(window);

        for (i = 0;i < row;i++)
        {
            for (j = 0;j < col;j++)
            {
                SDL_FRect srcRect;
                srcRect.x = j * split_width;
                srcRect.y = i * split_height;
                srcRect.w = split_width;
                srcRect.h = split_height;

                SDL_FRect dstRect;
                dstRect.x = 100;
                dstRect.y = 0;
                dstRect.w = 592;
                dstRect.h = 592;

                SDL_SetRenderDrawColor(renderer, 154, 120, 223, 255);
                SDL_RenderClear(renderer);

                SDL_RenderTexture(renderer, imageTexture, &srcRect, &dstRect);

                SDL_RenderPresent(renderer);
                SDL_Log("\nindex: %u", i * col + j);
                SDL_Log("show: 0 or 1");
                Uint32 proIndex = 0;
                while (!quit)
                {
                    while (SDL_PollEvent(&event))
                    {
                        if (event.key.key == SDLK_RIGHT && !event.key.repeat && event.type == SDL_EVENT_KEY_DOWN)
                        {
                            quit = true;
                        }
                        if (event.key.key == SDLK_DOWN && !event.key.repeat && event.type == SDL_EVENT_KEY_DOWN)
                        {
                            if (proIndex < PROPERTY_COUNT) SDL_Log("%s: %u", printByProIndex(proIndex), properties[i * col + j][proIndex]);
                            proIndex++;
                        }
                        if (event.type == SDL_EVENT_TEXT_INPUT && proIndex < PROPERTY_COUNT)
                        {
                            properties[i * col + j][proIndex] = SDL_atoi(event.text.text) % 2;
                            SDL_Log("%s: %u", printByProIndex(proIndex), properties[i * col + j][proIndex]);
                            proIndex++;
                        }
                    }
                    // SDL_Log("pre event type: %d", preEvent.key);
                    // SDL_Log("event type: %d", event.key);
                }
                quit = false;
            }
        }

        SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
        SDL_WriteIO(file, &split_width, sizeof(Uint32));
        SDL_WriteIO(file, &split_height, sizeof(Uint32));
        Uint32 propertyCount = PROPERTY_COUNT;
        SDL_WriteIO(file, &propertyCount, sizeof(Uint32));
        for (i = 0;i < row * col;i++)
        {
            SDL_WriteIO(file, &i, sizeof(Uint32));
            SDL_WriteIO(file, properties[i], sizeof(bool) * PROPERTY_COUNT);
            SDL_free(properties[i]);
        }
        SDL_SeekIO(file, 2 * sizeof(Uint32), SDL_IO_SEEK_SET);
        SDL_ReadIO(file, data, (sizeof(Uint32) + sizeof(bool) * PROPERTY_COUNT) * row * col);

        Uint32 crc32 = SDL_crc32(0, data, (sizeof(Uint32) + sizeof(bool) * PROPERTY_COUNT) * row * col);
        SDL_WriteIO(file, &crc32, sizeof(Uint32));

        SDL_free(properties);

        SDL_CloseIO(file);

        SDL_DestroyTexture(imageTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    return 0;
}