#ifndef __TGA_H__
#define __TGA_H__

#pragma comment(lib, "Opengl32.lib")					

#include <windows.h>									
#include <stdio.h>										
#include <gl\gl.h>										

typedef	struct
{
	GLubyte	* imageData;									
	GLuint	bpp;											
	GLuint	width;											
	GLuint	height;											
	GLuint	texID;											
	GLuint	type;											
} Texture;

typedef struct
{
	GLubyte Header[12];									
} TGAHeader;


typedef struct
{
	GLubyte		header[6];								
	GLuint		bytesPerPixel;							
	GLuint		imageSize;								
	GLuint		temp;									
	GLuint		type;	
	GLuint		Height;									
	GLuint		Width;									
	GLuint		Bpp;									
} TGA;


TGAHeader tgaheader;									
TGA tga;												



GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	
bool LoadUncompressedTGA(Texture *, char *, FILE *);	
bool LoadCompressedTGA(Texture *, char *, FILE *);		
bool LoadTGA(Texture * texture, char * filename);				
#endif






