#ifndef _MAIN_H
#define _MAIN_H





#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										
#include <gl\glu.h>										
#include "CVector3.h"



#define SCREEN_WIDTH 1680								
#define SCREEN_HEIGHT 1050								
#define SCREEN_DEPTH 16									


extern bool  g_bFullScreen;								
extern HWND  g_hWnd;									
extern RECT  g_rRect;									
extern HDC   g_hDC;										
extern HGLRC g_hRC;										
extern HINSTANCE g_hInstance;							
extern float g_DT;										


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);


LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


WPARAM MainLoop();


void ChangeToFullScreen();


HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);


bool bSetupPixelFormat(HDC hdc);


void SizeOpenGLScreen(int width, int height);


void InitializeOpenGL(int width, int height);


void Init(HWND hWnd);


void RenderScene();


void DeInit();

#endif 
















