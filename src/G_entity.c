#include "G_entity.h"
#include "cglm/cglm.h"

#define SQRT05 0.70710678118654752440084436210485f

void initEntity(G_Entity * entity, float x, float y, float speed)
{
    entity->position.x = x;
    entity->position.y = y;
    entity->speed = speed;
    for (int i = 0; i < 4; i++)
    {
        entity->direction[i] = false;
    }
}
void EntityMove(G_Entity * entity, float deltaTime)
{
    float distance = entity->speed * deltaTime;
    if (entity->direction[UP] && entity->direction[RIGHT])
    {
        distance *= SQRT05;
        entity->position.x += distance;
        entity->position.y += distance;
    }
    else if (entity->direction[UP] && entity->direction[LEFT])
    {
        distance *= SQRT05;
        entity->position.x -= distance;
        entity->position.y += distance;
    }
    else if (entity->direction[DOWN] && entity->direction[RIGHT])
    {
        distance *= SQRT05;
        entity->position.x += distance;
        entity->position.y -= distance;
    }
    else if (entity->direction[DOWN] && entity->direction[LEFT])
    {
        distance *= SQRT05;
        entity->position.x -= distance;
        entity->position.y -= distance;
    }
    else if (entity->direction[RIGHT])
    {
        entity->position.x += distance;
    }
    else if (entity->direction[LEFT])
    {
        entity->position.x -= distance;
    }
    else if (entity->direction[DOWN])
    {
        entity->position.y -= distance;
    }
    else if (entity->direction[UP])
    {
        entity->position.y += distance;
    }
}