/********************************************************************************
/Name:		TGA.cpp																*
/Header:	tga.h																*
/Purpose:	Load Compressed and Uncompressed TGA files							*
/Functions:	LoadTGA(Texture * texture, char * filename)							*
/			LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)	*
/			LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)*	
/*******************************************************************************/
#include "tga.h"


/********************************************************************************
/name :		LoadTGA(Texture * texture, char * filename)							*
/function:  Open and test the file to make sure it is a valid TGA file			*	
/parems:	texture, pointer to a Texture structure								*
/			filename, string pointing to file to open							*
/********************************************************************************/

bool LoadTGA(Texture * texture, char * filename)				
{
	FILE * fTGA;												
	fTGA = fopen(filename, "rb");								

	if(fTGA == NULL)											
	{
		MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK);	
		return false;														
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)					
	{
		MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);		
		if(fTGA != NULL)													
		{
			fclose(fTGA);													
		}
		return false;														
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				
	{																		
		LoadUncompressedTGA(texture, filename, fTGA);						
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		
	{																		
		LoadCompressedTGA(texture, filename, fTGA);							
	}
	else																	
	{
		MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);	
		fclose(fTGA);
		return false;																
	}
	return true;															
}

bool LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)	
{																			
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					
	{										
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		
		if(fTGA != NULL)													
		{
			fclose(fTGA);													
		}
		return false;														
	}	

	texture->width  = tga.header[1] * 256 + tga.header[0];					
	texture->height = tga.header[3] * 256 + tga.header[2];					
	texture->bpp	= tga.header[4];										
	tga.Width		= texture->width;										
	tga.Height		= texture->height;										
	tga.Bpp			= texture->bpp;											

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	
		if(fTGA != NULL)													
		{
			fclose(fTGA);													
		}
		return false;														
	}

	if(texture->bpp == 24)													
		texture->type	= GL_RGB;											
	else																	
		texture->type	= GL_RGBA;											

	tga.bytesPerPixel	= (tga.Bpp / 8);									
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					

	if(texture->imageData == NULL)											
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	
		fclose(fTGA);														
		return false;														
	}

	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	
	{
		MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		
		if(texture->imageData != NULL)										
		{
			free(texture->imageData);										
		}
		fclose(fTGA);														
		return false;														
	}

	
	for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}

	fclose(fTGA);															
	return true;															
}

bool LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)		
{ 
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					
	{
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		
		if(fTGA != NULL)													
		{
			fclose(fTGA);													
		}
		return false;														
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					
	texture->height = tga.header[3] * 256 + tga.header[2];					
	texture->bpp	= tga.header[4];										
	tga.Width		= texture->width;										
	tga.Height		= texture->height;										
	tga.Bpp			= texture->bpp;											

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	
		if(fTGA != NULL)													
		{
			fclose(fTGA);													
		}
		return false;														
	}

	if(texture->bpp == 24)													
		texture->type	= GL_RGB;											
	else																	
		texture->type	= GL_RGBA;											

	tga.bytesPerPixel	= (tga.Bpp / 8);									
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					

	if(texture->imageData == NULL)											
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	
		fclose(fTGA);														
		return false;														
	}

	GLuint pixelcount	= tga.Height * tga.Width;							
	GLuint currentpixel	= 0;												
	GLuint currentbyte	= 0;												
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			

	do
	{
		GLubyte chunkheader = 0;											

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);	
			if(fTGA != NULL)												
			{
				fclose(fTGA);												
			}
			if(texture->imageData != NULL)									
			{
				free(texture->imageData);									
			}
			return false;													
		}

		if(chunkheader < 128)												
		{																	
			chunkheader++;													
			for(short counter = 0; counter < chunkheader; counter++)		
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) 
				{
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		

					if(fTGA != NULL)													
					{
						fclose(fTGA);													
					}

					if(colorbuffer != NULL)												
					{
						free(colorbuffer);												
					}

					if(texture->imageData != NULL)										
					{
						free(texture->imageData);										
					}

					return false;														
				}
																						
				texture->imageData[currentbyte		] = colorbuffer[2];				    
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				
				}

				currentbyte += tga.bytesPerPixel;										
				currentpixel++;															

				if(currentpixel > pixelcount)											
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			

					if(fTGA != NULL)													
					{
						fclose(fTGA);													
					}	

					if(colorbuffer != NULL)												
					{
						free(colorbuffer);												
					}

					if(texture->imageData != NULL)										
					{
						free(texture->imageData);										
					}

					return false;														
				}
			}
		}
		else																			
		{
			chunkheader -= 127;															
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		
			{	
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			

				if(fTGA != NULL)														
				{
					fclose(fTGA);														
				}

				if(colorbuffer != NULL)													
				{
					free(colorbuffer);													
				}

				if(texture->imageData != NULL)											
				{
					free(texture->imageData);											
				}

				return false;															
			}

			for(short counter = 0; counter < chunkheader; counter++)					
			{																			
				texture->imageData[currentbyte		] = colorbuffer[2];					
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				
				}

				currentbyte += tga.bytesPerPixel;										
				currentpixel++;															

				if(currentpixel > pixelcount)											
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			

					if(fTGA != NULL)													
					{
						fclose(fTGA);													
					}	

					if(colorbuffer != NULL)												
					{
						free(colorbuffer);												
					}

					if(texture->imageData != NULL)										
					{
						free(texture->imageData);										
					}

					return false;														
				}
			}
		}
	}

	while(currentpixel < pixelcount);													
	fclose(fTGA);																		
	return true;																		
}
