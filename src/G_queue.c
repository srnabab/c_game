#include "G_queue.h"

static bool defaultAddTailFunc(G_Queue * queue, void * data)
{
    if (G_QueueIsFull(*queue)) return false;

    SDL_LockMutex(queue->mutex);

    if (queue->tail != -1)
    {
        queue->tail = (queue->tail + 1) % queue->capacity;
    }
    else if (queue->tail == -1) queue->tail = queue->head = 0;
    memcpy((char*)queue->data + (queue->tail * queue->dataSize), data, queue->dataSize);
    SDL_UnlockMutex(queue->mutex);

    return true;
}
static bool defaultAddHeadFunc(G_Queue * queue, void * data)
{
    if (G_QueueIsFull(*queue)) return false;

    SDL_LockMutex(queue->mutex);

    if (queue->head == -1)
    {
        queue->head = queue->tail = queue->capacity - 1;
    }
    else if (queue->head == 0)
    {
        queue->head = queue->capacity - 1;
    }
    else
    {
        queue->head--;
    }
    memcpy((char*)queue->data + (queue->head * queue->dataSize), data, queue->dataSize);
    SDL_UnlockMutex(queue->mutex);

    return true;
}
static bool defaultGetHeadFunc(G_Queue * queue, void * data)
{
    if (G_QueueIsEmpty(*queue)) return false;

    SDL_LockMutex(queue->mutex);

    memcpy(data, (char*)queue->data + (queue->head * queue->dataSize), queue->dataSize);

    queue->head = (queue->head + 1) % queue->capacity;

    if (queue->head == (queue->tail + 1) % queue->capacity)
    {
        queue->head = queue->tail = -1;
    }

    SDL_UnlockMutex(queue->mutex);

    return true;
}
static bool defaultGetTailFunc(G_Queue * queue, void * data)
{
    if (G_QueueIsEmpty(*queue)) return false;

    SDL_LockMutex(queue->mutex);

    memcpy(data, (char*)queue->data + (queue->tail * queue->dataSize), queue->dataSize);

    if (queue->tail == 0)
    {
        queue->tail = queue->capacity - 1;
    }
    else
    {
        queue->tail--;
    }

    if (queue->head == (queue->tail + 1) % queue->capacity)
    {
        queue->head = queue->tail = -1;
    }

    SDL_UnlockMutex(queue->mutex);

    return true;
}
bool initQueue(G_Queue * queue, size_t dataSize, int32_t capacity, QueueOp addTail, QueueOp getHead, QueueOp addHead, QueueOp getTail)
{
    queue->data = SDL_malloc(dataSize * capacity);
    if (queue->data == NULL) return false;

    queue->capacity = capacity;
    queue->head = -1;
    queue->tail = -1;
    queue->dataSize = dataSize;
    
    queue->mutex = SDL_CreateMutex();
    if (addTail == NULL) queue->addTail = defaultAddTailFunc;
    else queue->addTail = addTail;
    if (addHead == NULL) queue->addHead = defaultAddHeadFunc;
    else queue->addHead = addHead;
    if (getTail == NULL) queue->getTail = defaultGetTailFunc;
    else queue->getTail = getTail;
    if (getHead == NULL) queue->getHead = defaultGetHeadFunc;
    else queue->getHead = getHead;

    return true;
}
bool G_QueueIsEmpty(G_Queue queue)
{
    SDL_LockMutex(queue.mutex);
    if (queue.tail == -1) 
    {
        SDL_UnlockMutex(queue.mutex);
        return true;
    }

    SDL_UnlockMutex(queue.mutex);

    return false;
} 
bool G_QueueIsFull(G_Queue queue)
{
    SDL_LockMutex(queue.mutex);
    if ((queue.tail + 1) % queue.capacity == queue.head)
    {
        SDL_UnlockMutex(queue.mutex);
        return true;
    }

    SDL_UnlockMutex(queue.mutex);

    return false;
}
bool G_QueueResize(G_Queue * queue, int newCapacity)
{
    SDL_LockMutex(queue->mutex);
    if (queue->head != 0)
    {
        if (queue->tail > queue->head) 
        {
            size_t length = 0;
            length = queue->tail - queue->head;
            memmove(queue->data, (char*)queue->data + (queue->head * queue->dataSize), (length + 1) * queue->dataSize);

            queue->head = 0;
            queue->tail = length;
        }
        else if (queue->tail < queue->head)
        {
            size_t length0, length1;
            length0 = length1 = 0;

            length0 = queue->tail;
            length1 = queue->capacity - queue->head - 1;

            void * temp;
            if (length0 >= length1)
            {
                temp = SDL_malloc((length1 + 1) * queue->dataSize);
                if (temp == NULL)
                {
                    SDL_UnlockMutex(queue->mutex);
                    return false;
                }

                memcpy(temp, (char*)queue->data + (queue->head * queue->dataSize), (length1 + 1) * queue->dataSize);

                memmove((char*)queue->data + ((length1 + 1) * queue->dataSize), queue->data, (length0 + 1) * queue->dataSize);
                memcpy(queue->data, temp, (length1 + 1) * queue->dataSize);

                SDL_free(temp);

                queue->head = 0;
                queue->tail = length0 + length1 + 1;
            }
            else
            {
                temp = SDL_malloc((length0 + 1) * queue->dataSize);
                if (temp == NULL) 
                {
                    SDL_UnlockMutex(queue->mutex);
                    return false;
                }

                memcpy(temp, queue->data, (length0 + 1) * queue->dataSize);

                memcpy(queue->data, (char*)queue->data + (queue->head * queue->dataSize), (length1 + 1) * queue->dataSize);
                memcpy((char*)queue->data + ((length1 + 1)* queue->dataSize), temp, (length0 + 1) * queue->dataSize);

                SDL_free(temp);

                queue->head = 0;
                queue->tail = length0 + length1 + 1;
            }
        }
        else
        {
            memcpy(queue->data, (char*)queue->data + (queue->head * queue->dataSize), queue->dataSize);

            queue->head = queue->tail = 0;
        }
    }

    void * tempPtr = queue->data;
    queue->data = SDL_realloc(queue->data, newCapacity * queue->dataSize);
    if (queue->data == NULL) 
    {
        queue->data = tempPtr;

        SDL_UnlockMutex(queue->mutex);
        return false;
    }

    queue->capacity = newCapacity;

    SDL_UnlockMutex(queue->mutex);

    return true;
}
void G_deInitQueue(G_Queue * queue)
{
    SDL_free(queue->data);
    queue->capacity = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->dataSize = 0;

    SDL_DestroyMutex(queue->mutex);

    queue->addTail = NULL;
    queue->getTail = NULL;
    queue->getHead = NULL;
    queue->addHead = NULL;
}