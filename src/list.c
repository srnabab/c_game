#include "list.h"

void initList(List list, bool (*compareFn)(void *, void *))
{
    list.head = NULL;
    list.compareFn = compareFn;
}
bool insertNodeEnd(List list, void * data)
{
    Node * node = (Node *)SDL_malloc(sizeof(Node));
    if (node == NULL) return false;
    node->data = data;
    node->next = NULL;

    toEnd(list)->next = node;
    // list->size++;
    return true;
}
Node * searchPreNode(List list, void * data)
{
    Node * node = list.head;
    Node * preNode = NULL;
    while (node != NULL)
    {
        if (list.compareFn(node->data, data))
        {
            return preNode;
        }
        preNode = node;
        node = node->next;
    }
    return NULL;
}
bool deleteNode(List list, void * data)
{
    Node * preNode = searchPreNode(list, data);
    if (preNode != NULL)
    {
        Node * node = preNode->next;
        preNode->next = node->next;
        SDL_free(node);
        return true;
    }

    return false;
}
Node * toEnd(List list)
{
    Node * node = list.head;
    Node * preNode = NULL;
    while (node != NULL)
    {
        node = node->next;
        preNode = node;
    }
    return preNode;
}