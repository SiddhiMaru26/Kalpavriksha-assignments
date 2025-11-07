#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define blockSize 512
#define numberOfBlocks 1024
#define maxNameLength 50
#define lineBuffer 4096

typedef struct FreeBlock
{
    int index;
    struct FreeBlock* previous;
    struct FreeBlock* next;
} FreeBlock;

typedef struct FileNode
{
    char name[maxNameLength + 1];
    int isDirectory;
    struct FileNode* parent;
    struct FileNode* nextSibling;
    struct FileNode* prevSibling;
    struct FileNode* firstChild;
    int blockCount;
    int* blockIndices;
    int sizeBytes;
} FileNode;

unsigned char virtualDisk[numberOfBlocks][blockSize];
FreeBlock* freeListHead = NULL;
FreeBlock* freeListTail = NULL;
int freeBlockCount = 0;
FileNode* rootDirectory = NULL;
FileNode* currentWorkingDirectory = NULL;

FreeBlock* CreateFreeBlockNode(int index)
{
    FreeBlock* node = (FreeBlock*)malloc(sizeof(FreeBlock));
    if (!node)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    node->index = index;
    node->previous = NULL;
    node->next = NULL;
    return node;
}

void InitializeFreeBlocks(void)
{
    freeListHead = freeListTail = NULL;
    freeBlockCount = 0;
    for (int blockIndex = 0; blockIndex < numberOfBlocks; ++blockIndex)
    {
        FreeBlock* node = CreateFreeBlockNode(blockIndex);
        if (!node)
        {
           return;
        } 
        if (!freeListHead)
        {
            freeListHead = freeListTail = node;
        }
        else
        {
            freeListTail->next = node;
            node->previous = freeListTail;
            freeListTail = node;
        }
        freeBlockCount++;
    }
}

int AllocateBlocks(int requiredBlocks, int* allocatedBlockIndices)
{
    if (requiredBlocks <= 0 || freeBlockCount < requiredBlocks) return 0;
    for (int index = 0; index < requiredBlocks; index++)
    {
        FreeBlock* head = freeListHead;
        if (!head)
        { 
            return index;
        }
        allocatedBlockIndices[index] = head->index;
        freeListHead = head->next;
        if (freeListHead) freeListHead->previous = NULL;
        else freeListTail = NULL;
        free(head);
        freeBlockCount--;
    }
    return requiredBlocks;
}

void ReturnBlocks(int* allocatedBlockIndices, int count)
{
    if (!allocatedBlockIndices || count <= 0) return;
    for (int index = 0; index< count; index++)
    {
        FreeBlock* node = CreateFreeBlockNode(allocatedBlockIndices[index]);
        if (!node)
        {
             return;
        }
        if (!freeListTail)
        {
            freeListHead = freeListTail = node;
        }
        else
        {
            freeListTail->next = node;
            node->previous = freeListTail;
            freeListTail = node;
        }
        freeBlockCount++;
    }
}

FileNode* CreateFileNode(const char* name, int isDir)
{
    FileNode* node = (FileNode*)malloc(sizeof(FileNode));
    if (!node)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }
    strncpy(node->name, name, maxNameLength);
    node->name[maxNameLength] = '\0';
    node->isDirectory = isDir;
    node->parent = node->nextSibling = node->prevSibling = node->firstChild = NULL;
    node->blockCount = 0;
    node->blockIndices = NULL;
    node->sizeBytes = 0;
    return node;
}

int InsertChild(FileNode* directory, FileNode* child)
{
    if (!directory || !directory->isDirectory || !child) return 0;
    child->parent = directory;
    if (!directory->firstChild)
    {
        directory->firstChild = child;
        child->nextSibling = child->prevSibling = child;
    }
    else
    {
        FileNode* head = directory->firstChild;
        FileNode* tail = head->prevSibling;
        tail->nextSibling = child;
        child->prevSibling = tail;
        child->nextSibling = head;
        head->prevSibling = child;
    }
    return 1;
}

FileNode* FindChild(FileNode* directory, const char* name)
{
    if (!directory || !directory->isDirectory || !directory->firstChild || !name) 
    {
        return NULL;
    }
    FileNode*temporary = directory->firstChild;
    do
    {
        if (strcmp(temporary->name, name) == 0) 
        {
            return temporary;
        }
        temporary = temporary->nextSibling;
    } while (temporary != directory->firstChild);
    return NULL;
}

int RemoveChild(FileNode* child)
{
    if (!child || !child->parent) 
    {
        return 0;
    }
    FileNode* parent = child->parent;
    if (child->nextSibling == child && child->prevSibling == child)
    {
        parent->firstChild = NULL;
    }
    else
    {
        child->prevSibling->nextSibling = child->nextSibling;
        child->nextSibling->prevSibling = child->prevSibling;
        if (parent->firstChild == child) parent->firstChild = child->nextSibling;
    }
    child->nextSibling = child->prevSibling = NULL;
    child->parent = NULL;
    return 1;
}

void ReleaseFileNode(FileNode* node)
{
    if (!node) return;
    if (node->isDirectory)
    {
        while (node->firstChild)
        {
            FileNode* child = node->firstChild;
            RemoveChild(child);
            ReleaseFileNode(child);
        }
        free(node);
        return;
    }
    if (node->blockCount && node->blockIndices)
    {
        ReturnBlocks(node->blockIndices, node->blockCount);
        free(node->blockIndices);
    }
    free(node);
}

void InitializeRoot(void)
{
    rootDirectory = CreateFileNode("/", 1);
    if (rootDirectory)
    {
         rootDirectory->parent = rootDirectory;
    }
    currentWorkingDirectory = rootDirectory;
}

void InitializeDisk(void)
{
    for (int blockIndex = 0; blockIndex < numberOfBlocks; ++blockIndex)
    {
        for (int index = 0; index < blockSize; index++) 
        {
            virtualDisk[blockIndex][index] = 0;
        }
    }
}

void CommandMkdir(const char* directoryName)
{
    if (!directoryName || strlen(directoryName) == 0)
    {
        printf("Usage: mkdir <directoryName>\n");
        return;
    }
    
    for (int index = 0; directoryName[index]; index++)
    {
        if (isspace((unsigned char)directoryName[index]))
        {
            printf("mkdir: A positional parameter cannot be found that accepts argument '%s'.\n", directoryName);
            return;
        }
    }

    if (FindChild(currentWorkingDirectory, directoryName))
    {
        printf("mkdir: '%s' already exists\n", directoryName);
        return;
    }
    FileNode* directory = CreateFileNode(directoryName, 1);
    InsertChild(currentWorkingDirectory, directory);
    printf("Directory '%s' created successfully.\n", directoryName);
}

void CommandCreate(const char* fileName)
{
    if (!fileName || strlen(fileName) == 0)
    {
        printf("Usage: create <fileName>\n");
        return;
    }

    for (int index = 0; fileName[index]; index++)
    {
        if (isspace((unsigned char)fileName[index]))
        {
            printf("create: A positional parameter cannot be found that accepts argument '%s'.\n", fileName);
            return;
        }
    }

    if (FindChild(currentWorkingDirectory, fileName))
    {
        printf("create: '%s' already exists\n", fileName);
        return;
    }
    FileNode* file = CreateFileNode(fileName, 0);
    InsertChild(currentWorkingDirectory, file);
    printf("File '%s' created successfully.\n", fileName);
}

void CommandWrite(const char* fileName, const char* content)
{
    if (!fileName || strlen(fileName) == 0)
    {
        printf("Usage: write <fileName> <content>\n");
        return;
    }
    FileNode* file = FindChild(currentWorkingDirectory, fileName);
    if (!file)
    {
        printf("write: '%s' not found\n", fileName);
        return;
    }
    if (file->isDirectory)
    {
        printf("write: '%s' is a directory\n", fileName);
        return;
    }

    const char* ptr = content ? content : "";
    int contentLength = (int)strlen(ptr);
    if (file->blockCount && file->blockIndices)
    {
        ReturnBlocks(file->blockIndices, file->blockCount);
        free(file->blockIndices);
        file->blockIndices = NULL;
        file->blockCount = 0;
        file->sizeBytes = 0;
    }

    int requiredBlocks = (contentLength + blockSize - 1) / blockSize;
    if (requiredBlocks == 0)
    {
        printf("Data written successfully (size=0 bytes).\n");
        return;
    }

    int* allocatedBlockIndices = (int*)malloc(sizeof(int) * requiredBlocks);
    if (!allocatedBlockIndices)
    {
        printf("Memory allocation failed\n");
        return;
    }

    int allocated = AllocateBlocks(requiredBlocks, allocatedBlockIndices);

    if (allocated < requiredBlocks)
    {
        if (allocated > 0)
        {
             ReturnBlocks(allocatedBlockIndices, allocated);
        }
        free(allocatedBlockIndices);
        printf("write: disk full or insufficient blocks. Write failed.\n");
        return;
    }

    int remaining = contentLength;
    const unsigned char* src = (const unsigned char*)ptr;
    for (int index = 0; index< requiredBlocks; index++)
    {
        int writeBytes = remaining >= blockSize ? blockSize : remaining;
        if (writeBytes > 0) 
        {
            memcpy(virtualDisk[allocatedBlockIndices[index]], src, writeBytes);
        }
        if (writeBytes < blockSize)
        {
            for (int indices = writeBytes; indices< blockSize; indices++)
            {
               virtualDisk[allocatedBlockIndices[index]][indices] = 0;
            }
        }
        src += writeBytes;
        remaining -= writeBytes;
    }

    file->blockIndices = allocatedBlockIndices;
    file->blockCount = requiredBlocks;
    file->sizeBytes = contentLength;
    printf("Data written successfully (size=%d bytes).\n", contentLength);
}

void CommandRead(const char* fileName)
{
    if (!fileName || strlen(fileName) == 0)
    {
        printf("Usage: read <fileName>\n");
        return;
    }
    FileNode* file = FindChild(currentWorkingDirectory, fileName);
    if (!file)
    {
        printf("read: '%s' not found\n", fileName);
        return;
    }
    if (file->isDirectory)
    {
        printf("read: '%s' is a directory\n", fileName);
        return;
    }
    if (!file->blockCount || !file->blockIndices) 
    { 
        printf("\n"); return; 
    }

    int bytesLeft = file->sizeBytes;
    for (int index = 0; index < file->blockCount && bytesLeft > 0; index++)
    {
        int toPrint = bytesLeft >= blockSize ? blockSize : bytesLeft;
        fwrite(virtualDisk[file->blockIndices[index]], 1, toPrint, stdout);
        bytesLeft -= toPrint;
    }
    printf("\n");
}

void CommandDelete(const char* fileName)
{
    if (!fileName || strlen(fileName) == 0)
    {
        printf("Usage: delete <fileName>\n");
        return;
    }
    FileNode* file = FindChild(currentWorkingDirectory, fileName);
    if (!file)
    {
        printf("delete: '%s' not found\n", fileName);
        return;
    }
    if (file->isDirectory)
    {
        printf("delete: '%s' is a directory. Use rmdir\n", fileName);
        return;
    }
    RemoveChild(file);
    if (file->blockCount && file->blockIndices) { ReturnBlocks(file->blockIndices, file->blockCount); free(file->blockIndices); }
    free(file);
    printf("File deleted successfully.\n");
}

void CommandRmdir(const char* directoryName)
{
    if (!directoryName || strlen(directoryName) == 0)
    {
        printf("Usage: rmdir <directoryName>\n");
        return;
    }
    FileNode* directory = FindChild(currentWorkingDirectory, directoryName);
    if (!directory)
    {
        printf("rmdir: '%s' not found\n", directoryName);
        return;
    }
    if (!directory->isDirectory)
    {
        printf("rmdir: '%s' is not a directory\n", directoryName);
        return;
    }
    if (directory->firstChild)
    {
        printf("rmdir: '%s' is not empty\n", directoryName);
        return;
    }
    RemoveChild(directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

void CommandLs(void)
{
    if (!currentWorkingDirectory->firstChild) 
    {
         printf("(empty)\n"); return; 
    }
    FileNode* it = currentWorkingDirectory->firstChild;
    do
    {
        if (it->isDirectory)
        {
             printf("%s/\n", it->name);
        }
        else 
        {
            printf("%s\n", it->name);
        }
        it = it->nextSibling;
    } while (it != currentWorkingDirectory->firstChild);
}

void PrintWorkingDirectory(void)
{
    if (currentWorkingDirectory == rootDirectory) 
    { 
        printf("/\n"); return; 
    }
    const int maxDepth = 1024;
    const char* parts[maxDepth];
    int depth = 0;
    FileNode* node = currentWorkingDirectory;
    while (node != rootDirectory && depth < maxDepth)
    {
        parts[depth++] = node->name;
        node = node->parent;
    }
    printf("/");
    for (int index = depth - 1; index >= 0; index--)
    {
        printf("%s", parts[index]);
        if (index > 0) printf("/");
    }
    printf("\n");
}

void CommandCd(const char* argument)
{
    if (!argument || strlen(argument) == 0)
    {
        printf("Usage: cd <path>\n");
        return;
    }
    if (strcmp(argument, "/") == 0)
    {
        currentWorkingDirectory = rootDirectory;
        printf("Moved to /\n");
        return;
    }
    if (strcmp(argument, "..") == 0)
    {
        if (currentWorkingDirectory == rootDirectory) 
        { 
            printf("Already at root\n");
             return;
        }
        currentWorkingDirectory = currentWorkingDirectory->parent;
        printf("Moved to "); 
        PrintWorkingDirectory(); 
        return;
    }
    FileNode* directory = FindChild(currentWorkingDirectory, argument);
    if (!directory)
    {
        printf("cd: '%s' not found\n", argument);
        return;
    }
    if (!directory->isDirectory)
    {
        printf("cd: '%s' is not a directory\n", argument);
        return;
    }
    currentWorkingDirectory = directory;
    printf("Moved to "); PrintWorkingDirectory();
}

void CommandDf(void)
{
    int used = numberOfBlocks - freeBlockCount;
    double percent = numberOfBlocks == 0 ? 0.0 : (100.0 * used / numberOfBlocks);
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\nDisk Usage: %.2f%%\n",
           numberOfBlocks, used, freeBlockCount, percent);
}

void ShutdownAll(void)
{
    if (rootDirectory)
    {
        while (rootDirectory->firstChild)
        {
            FileNode* child = rootDirectory->firstChild;
            RemoveChild(child);
            ReleaseFileNode(child);
        }
        free(rootDirectory);
        rootDirectory = currentWorkingDirectory = NULL;
    }
    FreeBlock* fb = freeListHead;
    while (fb)
    {
        FreeBlock* next = fb->next;
        free(fb);
        fb = next;
    }
    freeListHead = freeListTail = NULL;
    freeBlockCount = 0;
}

void PrintPrompt(void)
{
    if (currentWorkingDirectory == rootDirectory) 
    {
        printf("/ > ");
    }
    else 
    {
        printf("%s > ", currentWorkingDirectory->name);
    }
}

int main(void)
{
    InitializeFreeBlocks();
    InitializeRoot();
    InitializeDisk();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char buffer[lineBuffer];
    while (1)
    {
        PrintPrompt();
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("\n");
            ShutdownAll();
            printf("Memory released. Exiting program...\n");
            return 0;
        }

        char* newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';

        char* line = buffer;
        while (*line && isspace((unsigned char)*line)) line++;
        if (*line == '\0') continue;

        char command[64] = {0};
        char argument[lineBuffer] = {0};

        int pos = 0;
        while (*line && !isspace((unsigned char)*line) && pos < 63)
        {
            command[pos++] = *line++;
        }
        command[pos] = '\0';

        while (*line && isspace((unsigned char)*line))
        {
             line++;
        }
        if (*line)
        {
            size_t argLen = strlen(line);
            if (argLen >= sizeof(argument)) 
            {
                argLen = sizeof(argument) - 1;
            }
            strncpy(argument, line, argLen);
            argument[argLen] = '\0';
        }

        if (strcmp(command, "mkdir") == 0)
        { 
            CommandMkdir(argument);
        }
        else if (strcmp(command, "create") == 0) 
        {
            CommandCreate(argument);
        }
        else if (strcmp(command, "write") == 0)
        {
            char* spacePos = strchr(argument, ' ');
            if (!spacePos) 
            { 
                printf("Usage: write <fileName> <content>\n"); 
                continue; 
            }
            *spacePos = '\0';
            CommandWrite(argument, spacePos + 1);
        }
        else if (strcmp(command, "read") == 0)
        { 
            CommandRead(argument);
        }
        else if (strcmp(command, "delete") == 0) 
        {
            CommandDelete(argument);
        }
        else if (strcmp(command, "rmdir") == 0)\
        {
            CommandRmdir(argument);
        }
        else if (strcmp(command, "ls") == 0) 
        {
            CommandLs();
        }
        else if (strcmp(command, "cd") == 0) 
        {
            CommandCd(argument);
        }
        else if (strcmp(command, "pwd") == 0) 
        {
            PrintWorkingDirectory();
        }
        else if (strcmp(command, "df") == 0) 
        {
            CommandDf();
        }
        else if (strcmp(command, "exit") == 0)
        {
            ShutdownAll();
            printf("Memory released. Exiting program...\n");
            return 0;
        }
        else
        {
            printf("Unknown command: '%s'\n", command);
        }
    }
    return 0;
}
