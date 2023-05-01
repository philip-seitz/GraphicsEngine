#pragma once

#include <GL/glew.h>
// alle Sachen, um die Fehler zu handeln usw.
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void PrintErrorMsg(const int& errorcode);

bool GLLogCall(const char* function, const char* file, int line);

void GLClearError();