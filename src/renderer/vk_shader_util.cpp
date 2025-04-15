#include "logger.h"

void vk_compile_shader(char *shaderPath, char *sprivPath)
{
    ASSERT(shaderPath, "No shader path supplied");
    ASSERT(sprivPath, "No spriv path supplied");

    char command[320] = {};
    sprintf(command, "lib\\glslc.exe %s -o %s", shaderPath, sprivPath);

    int result = system(command);

    ASSERT(!result, "Failed to compile shader: %s", shaderPath);
}