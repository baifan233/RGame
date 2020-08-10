#pragma once

#include<Windows.h>
#include"GL/glew.h"

#include<stdio.h>
#include<vector>


#pragma comment(lib,"opengl32.lib")
#ifdef _M_IX86
#pragma comment(lib,"lib32/glew32.lib")
#pragma comment(lib,"lib32/glew32s.lib")
#endif

#ifdef _M_X64
#pragma comment(lib,"lib64/glew32.lib")
#pragma comment(lib,"lib64/glew32s.lib")
#endif
