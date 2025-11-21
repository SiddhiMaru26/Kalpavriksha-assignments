#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_MAP_SIZE 2003
#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000

typedef struct CacheNode
{
    int key; 
    char *data;
    struct CacheNode *prev;
    struct CacheNode *next;
} CacheNode;

typedef struct HashNode
{
    int key;
    CacheNode *cacheNode;
    struct HashNode *next;
} HashNode;

typedef struct LRUCache
{
    int capacity;
    int currentSize;
    CacheNode *head;
    CacheNode *tail;
    HashNode *buckets[HASH_MAP_SIZE];
} LRUCache;

int isValidInteger(char *text)
{
    if (text == NULL)
    {
        return 0;
    }
    int length = strlen(text);
    if (length == 0)
    {
        return 0;
    }
    for (int index = 0; index < length; ++index)
    {
        char ch = text[index];
        if (ch < '0' || ch > '9')
        {
            return 0;
        }
    }
    return 1;
}

bool isValidKey(int key)
{
    return key >= 0;
}

bool isValidValue(char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return false;
    }

    int length = strlen(value);
    for (int index = 0; index < length; ++index)
    {
        char charcter = value[index];
        if ((charcter >= 'A' && charcter <= 'Z') || (charcter >= 'a' && charcter <= 'z'))
        {
            return true;
        }
    }
    return false;
}

char * stringDuplicate(char *source)
{
    if (source == NULL)
    {
        return NULL;
    }
    int length = strlen(source);
    char * copy = (char *) malloc(length + 1);
    if (copy == NULL)
    {
        return NULL;
    }
    strcpy(copy, source);
    return copy;
}

int computeHashIndex(int key)
{
    unsigned int absoluteKey = (key < 0) ? (unsigned int)(-key) : (unsigned int)key;
    return (int)(absoluteKey % HASH_MAP_SIZE);
}

CacheNode * mapGetNode(LRUCache *cache, int key)
{
    int index = computeHashIndex(key);
    HashNode * current = cache->buckets[index];
    while (current != NULL)
    {
        if (current->key == key)
        {
            return current->cacheNode;
        }
        current = current->next;
    }
    return NULL;
}

bool mapPutNode(LRUCache *cache, int key, CacheNode *node)
{
    int index = computeHashIndex(key);
    HashNode * newHashNode = (HashNode *) malloc(sizeof(HashNode));
    if (newHashNode == NULL)
    {
        return false;
    }
    newHashNode->key = key;
    newHashNode->cacheNode = node;
    newHashNode->next = cache->buckets[index];
    cache->buckets[index] = newHashNode;
    return true;
}

bool mapRemoveKey(LRUCache *cache, int key)
{
    int index = computeHashIndex(key);
    HashNode * current = cache->buckets[index];
    HashNode * previous = NULL;
    while (current != NULL)
    {
        if (current->key == key)
        {
            if (previous == NULL)
            {
                cache->buckets[index] = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

void insertNodeAtHead(LRUCache *cache, CacheNode *node)
{
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head != NULL)
    {
        cache->head->prev = node;
    }
    cache->head = node;
    if (cache->tail == NULL)
    {
        cache->tail = node;
    }
}

void removeNodeFromList(LRUCache *cache, CacheNode *node)
{
    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else
    {
        cache->head = node->next;
    }
    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else
    {
        cache->tail = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
}

void moveNodeToHead(LRUCache *cache, CacheNode *node)
{
    if (cache->head == node)
    {
        return;
    }
    removeNodeFromList(cache, node);
    insertNodeAtHead(cache, node);
}

CacheNode * removeTailNode(LRUCache *cache)
{
    CacheNode * nodeToRemove = cache->tail;
    if (nodeToRemove == NULL)
    {
        return NULL;
    }
    removeNodeFromList(cache, nodeToRemove);
    return nodeToRemove;
}

LRUCache * createCache(int capacity)
{
    if (capacity < MIN_CAPACITY)
    {
        return NULL;
    }
    LRUCache * cache = (LRUCache *) malloc(sizeof(LRUCache));
    if (cache == NULL)
    {
        return NULL;
    }
    cache->capacity = capacity;
    cache->currentSize = 0;
    cache->head = NULL;
    cache->tail = NULL;
    for (int index = 0; index < HASH_MAP_SIZE; ++index)
    {
        cache->buckets[index] = NULL;
    }
    return cache;
}

char * getValueFromCache(LRUCache *cache, int key)
{
    if (cache == NULL)
    {
        return NULL;
    }
    if (!isValidKey(key))
    {
        return NULL;
    }
    CacheNode * foundNode = mapGetNode(cache, key);
    if (foundNode == NULL)
    {
        return NULL;
    }
    moveNodeToHead(cache, foundNode);
    return foundNode->data;
}

void putValueInCache(LRUCache *cache, int key, char *value)
{
    if (cache == NULL)
    {
        return;
    }
    if (!isValidKey(key) || !isValidValue(value))
    {
        return;
    }
    CacheNode * existingNode = mapGetNode(cache, key);
    if (existingNode != NULL)
    {
        char * newData = stringDuplicate(value);
        if (newData == NULL)
        {
            return;
        }
        free(existingNode->data);
        existingNode->data = newData;
        moveNodeToHead(cache, existingNode);
        return;
    }
    CacheNode * newNode = (CacheNode *) malloc(sizeof(CacheNode));
    if (newNode == NULL)
    {
        return;
    }
    newNode->key = key;
    newNode->data = stringDuplicate(value);
    if (newNode->data == NULL)
    {
        free(newNode);
        return;
    }
    newNode->prev = NULL;
    newNode->next = NULL;
    if (!mapPutNode(cache, key, newNode))
    {
        free(newNode->data);
        free(newNode);
        return;
    }
    insertNodeAtHead(cache, newNode);
    cache->currentSize += 1;
    if (cache->currentSize > cache->capacity)
    {
        CacheNode * nodeToRemove = removeTailNode(cache);
        if (nodeToRemove != NULL)
        {
            mapRemoveKey(cache, nodeToRemove->key);
            free(nodeToRemove->data);
            free(nodeToRemove);
            cache->currentSize -= 1;
        }
    }
}

void freeCache(LRUCache *cache)
{
    if (cache == NULL)
    {
        return;
    }
    CacheNode * currentNode = cache->head;
    while (currentNode != NULL)
    {
        CacheNode * nextNode = currentNode->next;
        free(currentNode->data);
        free(currentNode);
        currentNode = nextNode;
    }
    for (int index = 0; index < HASH_MAP_SIZE; ++index)
    {
        HashNode * currentHash = cache->buckets[index];
        while (currentHash != NULL)
        {
            HashNode * nextHash = currentHash->next;
            free(currentHash);
            currentHash = nextHash;
        }
    }
    free(cache);
}

void trimNewlineAndSpaces(char * buffer)
{
    int length = strlen(buffer);
    while (length > 0)
    {
        char lastChar = buffer[length - 1];
        if (lastChar == '\n' || lastChar == '\r' || lastChar == ' ' || lastChar == '\t')
        {
            buffer[length - 1] = '\0';
            length -= 1;
        }
        else
        {
            break;
        }
    }
}

char * duplicateLiteral(char *text)
{
    if (text == NULL)
    {
        return NULL;
    }
    char * copy = stringDuplicate(text);
    return copy;
}

int main(void)
{
    LRUCache * cache = NULL;
    char inputLine[1024];
    char ** getResults = NULL;
    int resultsCount = 0;
    int resultsCapacity = 0;
    while (true)
    {
        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL)
        {
            break;
        }
        trimNewlineAndSpaces(inputLine);
        if (inputLine[0] == '\0')
        {
            continue;
        }
        char * commandToken = strtok(inputLine, " \t");
        if (commandToken == NULL)
        {
            continue;
        }
        if (strcmp(commandToken, "createCache") == 0)
        {
            char * sizeToken = strtok(NULL, " \t");
            if (sizeToken == NULL)
            {
                printf("createCache requires a positive integer capacity\n");
                continue;
            }
            if (!isValidInteger(sizeToken))
            {
                printf("capacity must be an integer\n");
                continue;
            }
            int capacity = atoi(sizeToken);
            if (capacity < MIN_CAPACITY || capacity>MAX_CAPACITY)
            {
                printf("capacity must be between 1 to 1000\n");
                continue;
            }
            if (cache != NULL)
            {
                freeCache(cache);
                cache = NULL;
            }
            cache = createCache(capacity);
            if (cache == NULL)
            {
                printf("could not create cache (memory problem)\n");
            }
            continue;
        }
        if (strcmp(commandToken, "put") == 0)
        {
            char * keyToken = strtok(NULL, " \t");
            char * dataToken = strtok(NULL, "\n");
            if (keyToken == NULL || dataToken == NULL)
            {
                printf("put requires a key and a value\n");
                continue;
            }
            while (*dataToken == ' ' || *dataToken == '\t')
            {
                dataToken++;
            }
            if (!isValidInteger(keyToken))
            {
                printf("key must be an integer\n");
                continue;
            }
            int keyValue = atoi(keyToken);

            if (!isValidValue(dataToken))
            {
                printf("value must be a string\n");
                continue;
            }

            if (cache == NULL)
            {
                printf("cache not initialized; use createCache <size>\n");
                continue;
            }
            putValueInCache(cache, keyValue, dataToken);
            continue;
        }
        if (strcmp(commandToken, "get") == 0)
        {
            char * keyToken = strtok(NULL, " \t");
            if (keyToken == NULL)
            {
                printf("get requires a key\n");
                continue;
            }
            if (!isValidInteger(keyToken))
            {
                printf("key must be an integer\n");
                continue;
            }
            int keyValue = atoi(keyToken);
            char * foundData = NULL;
            if (cache == NULL)
            {
                foundData = NULL;
            }
            else
            {
                foundData = getValueFromCache(cache, keyValue);
            }
            char * resultText = NULL;
            if (foundData == NULL)
            {
                resultText = duplicateLiteral("NULL");
            }
            else
            {
                resultText = duplicateLiteral(foundData);
            }
            if (resultText == NULL)
            {
                printf("memory allocation failed while recording get result\n");
                continue;
            }
            if (resultsCount + 1 > resultsCapacity)
            {
                int newCapacity = (resultsCapacity == 0) ? 8 : resultsCapacity * 2;
                char ** newArray = (char **) malloc(sizeof(char *) * newCapacity);
                if (newArray == NULL)
                {
                    free(resultText);
                    printf("ERROR: memory allocation failed\n");
                    continue;
                }
                for (int copyIndex = 0; copyIndex < resultsCount; ++copyIndex)
                {
                    newArray[copyIndex] = getResults[copyIndex];
                }
                free(getResults);
                getResults = newArray;
                resultsCapacity = newCapacity;
            }
            getResults[resultsCount++] = resultText;
            continue;
        }
        if (strcmp(commandToken, "exit") == 0)
        {
            for (int index = 0; index < resultsCount; ++index)
            {
                printf("%s\n", getResults[index]);
            }
            for (int index = 0; index < resultsCount; ++index)
            {
                free(getResults[index]);
            }
            free(getResults);
            getResults = NULL;
            resultsCount = 0;
            resultsCapacity = 0;
            break;
        }
        printf("unknown command\n");
    }
    freeCache(cache);
    return 0;
}
