#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"										
#include "Camera.h"
#include "SimplexNoise.h"

#include "CTerrain.h"

CCamera g_Camera;										

bool  g_bFullScreen = TRUE;								
HWND  g_hWnd;											
RECT  g_rRect;											
HDC   g_hDC;											
HGLRC g_hRC;											
HINSTANCE g_hInstance;									
float g_DT = 0.0f;										

bool is_focus;

CTerrain* terrain_object = NULL;


void Init(HWND hWnd)
{
	g_hWnd = hWnd;										
	GetClientRect(g_hWnd, &g_rRect);					
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	

	g_Camera.PositionCamera(1.5, 6.5f, -8.f,   0, 1.5f, 0,   0, 1, 0);
}



bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; 
	float deltaTime = currentTime - lastTime; 
	float desiredFPS = 1.0f / desiredFrameRate; 

	elapsedTime += deltaTime; 
	lastTime = currentTime; 

	
	if( elapsedTime > desiredFPS )
	{
		g_DT = desiredFPS; 
		elapsedTime -= desiredFPS; 

		return true;
	}

	
	return false;
}

CVector3 GetOGLPos(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	return CVector3(posX, posY, posZ);
}

CVector3 current_point;


WPARAM MainLoop()
{
	MSG msg;

	while(1)											
	{													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					
				break;
			TranslateMessage(&msg);						
			DispatchMessage(&msg);						
		}
		else											
		{
			if(AnimateNextFrame(4000))					
			{
				if(is_focus)
					g_Camera.Update();						

				RenderScene();							
			}
			else
			{
				Sleep(1);								
			}
		} 
	}

	DeInit();											

	return(msg.wParam);									
}


#define GIRD_SIZE 150
float world[GIRD_SIZE][GIRD_SIZE];
float normals[GIRD_SIZE][GIRD_SIZE][3];

struct vec3_t {
	float x, y, z;
};

#include "random.h"

GLfloat vertices[] = { 1,1,1,  -1,1,1,  -1,-1,1,  1,-1,1,        
					  1,1,1,  1,-1,1,  1,-1,-1,  1,1,-1,        
					  1,1,1,  1,1,-1,  -1,1,-1,  -1,1,1,        
					  -1,1,1,  -1,1,-1,  -1,-1,-1,  -1,-1,1,    
					  -1,-1,-1,  1,-1,-1,  1,-1,1,  -1,-1,1,    
					  1,-1,-1,  -1,-1,-1,  -1,1,-1,  1,1,-1 };   

float cube[] = {

	0.f, 0.f, 0.f,
	0.f, 0.f, 1.f,
	0.f, 1.f, 1.f,
	0.f, 1.f, 0.f,

	1.f, 0.f, 0.f,
	1.f, 0.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 0.f, 

	0.f, 0.f, 1.f,
	0.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 0.f, 1.f,

	0.f, 0.f, 0.f, 
	0.f, 1.f, 0.f,
	1.f, 1.f, 0.f,
	1.f, 0.f, 0.f,

	0.f, 0.f, 0.f,
	0.f, 0.f, 1.f,
	1.f, 0.f, 1.f,
	1.f, 0.f, 0.f,

	0.f, 1.f, 0.f,
	0.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 0.f
};

void draw_cube()
{
	glVertexPointer(3, GL_FLOAT, 0, cube);
	glDrawArrays(GL_QUADS, 0, 24);
}

vec3_t* world_array = 0;
int size = 3000;

float scale = 0.001f;
float heightscale = 1.0f;
bool gradient = 0;

void computeNormals() {
	for (int z = 0; z < GIRD_SIZE - 1; z++) {
		for (int x = 0; x < GIRD_SIZE; x++) {
			// Вычислить нормаль для текущей вершины
			float dx = world[x + 1][z] - world[x][z];
			float dz = world[x][z + 1] - world[x][z];
			float dy = 1.0f;  // Предполагаем, что y-координата равна 1 (плоскость)

			// Нормализовать нормаль
			float length = sqrt(dx * dx + dy * dy + dz * dz);
			if (length != 0.0f) {
				dx /= length;
				dy /= length;
				dz /= length;
			}

			// Сохранить нормаль
			normals[x][z][0] = dx;
			normals[x][z][1] = dy;
			normals[x][z][2] = dz;
		}
	}
}

void Build()
{
	SimplexNoise snoise;
	for (int x = 0; x < GIRD_SIZE; x++) {
		for (int y = 0; y < GIRD_SIZE; y++) {
			world[y][x] = (snoise.noise((float)x * scale, (float)y * scale) + snoise.noise((float)x * 0.0111, (float)y * 0.0111)) * heightscale;
		}
	}
	computeNormals();
}

void Draw3DSGrid(CCamera camera)
{
	if (GetKeyState(VK_F3) & 0x80) {
		scale -= 0.001;
		Build();
	}

	if (GetKeyState(VK_F4) & 0x80) {
		scale += 0.001;
		Build();
	}

	if (GetKeyState(VK_F5) & 0x80) {
		heightscale -= 0.1;
		Build();
	}
	
	if (GetKeyState(VK_F6) & 0x80) {
		heightscale += 0.1;
		Build();
	}

	if (GetKeyState(VK_F7) & 0x80) {
		gradient = 0;
	}

	if (GetKeyState(VK_F8) & 0x80) {
		gradient = 1;
	}

	
	glLineWidth(5);
	glBegin(GL_LINES);

	
	glColor3ub(0, 255, 0);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.5f, 0.f);

	
	glColor3ub(255, 0, 0);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.5f, 0.f, 0.f);

	
	glColor3ub(0, 0, 255);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.5f);
	glEnd();
	glLineWidth(1);

	glPolygonMode(GL_FRONT_AND_BACK, gradient ? GL_FILL : GL_LINE);

	for (int z = 0; z < GIRD_SIZE - 1; z++) {
		glBegin(GL_TRIANGLE_STRIP);

		for (int x = 0; x < GIRD_SIZE; x++) {
			// Выберите цвет в зависимости от высоты
			float height = world[x][z];

			if (gradient) {
				// Используйте формулу градиента для цвета в зависимости от высоты
				float colorFactor = (height - 1.0f) / 9.0f;
				glColor3f(0.0f + colorFactor, 1.0f - colorFactor, 0.0f);
			}
			else {
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			// Вывести вершину с нормалями
			glNormal3fv(normals[x][z]);
			glVertex3f((float)x, height, (float)z);

			// Выберите цвет для следующей вершины в зависимости от следующей высоты
			height = world[x][z + 1];

			if (gradient) {
				// Используйте формулу градиента для цвета в зависимости от высоты
				float colorFactor = (height - 1.0f) / 9.0f;
				glColor3f(0.0f + colorFactor, 1.0f - colorFactor, 0.0f);
			}

			// Вывести вершину с нормалями
			glNormal3fv(normals[x][z + 1]);
			glVertex3f((float)x, height, (float)z + 1);
		}

		glEnd();
	}
}

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();									
	
	g_Camera.Look();

	Draw3DSGrid(g_Camera);
	
	SwapBuffers(g_hDC);	
}

 void gen_vertexes(int world_size)
 {
	 int world_x = world_size * 2;
	 int ssize = sizeof(vec3_t) * (world_x * world_x) * 2;
	 world_array = (vec3_t*)malloc(ssize);

	 int c = 0;
	 bool is_right = true;
	 for (int z = 0; z < world_x; z++) {
		 int j = z * world_x;
		 if (is_right) {
			 for (int x = 0, i = 0; x < world_x; x += 2, i++) {
				 world_array[c].z = (float)z;
				 world_array[c].x = (float)i;
				 world_array[c].y = 0.f;
				
				 world_array[c + 1].z = (float)z + 1.f;
				 world_array[c + 1].x = (float)i;
				 world_array[c + 1].y = 0.f;
				 
				 c += 2;
			 }

			 world_array[c].z = (float)z;
			 world_array[c].x = (float)world_x / 2;
			 world_array[c].y = 0.f;
			 
			 c++;

			 world_array[c].z = (float)z + 1;
			 world_array[c].x = (float)world_x / 2;
			 world_array[c].y = 0.f;
			 
			 c++;

			 if (z <= world_size-3)
			 {
				 world_array[c].z = (float)z + 2;
				 world_array[c].x = (float)world_x / 2;
				 world_array[c].y = 0.f;
				 
				 c++;

				 world_array[c].z = (float)z + 1;
				 world_array[c].x = (float)world_x / 2;
				 world_array[c].y = 0.f;
				 
				 c++;
			 }
		 }
		 else
		 {
			 for (int x = world_x, i = world_x / 2; x >= 2; x -= 2, i--) {
				 world_array[c].z = (float)z;
				 world_array[c].x = (float)i;
				 world_array[c].y = 0.f;

				 world_array[c + 1].z = (float)z + 1.f;
				 world_array[c + 1].x = (float)i - 1;
				 world_array[c + 1].y = 0.f;
				 
				 c += 2;
			 }
			 world_array[c].z = (float)z + 1;
			 world_array[c].x = 0.f;
			 world_array[c].y = 0.f;
			 
			 c++;

			 world_array[c].z = (float)z - 1;
			 world_array[c].x = 0.f;
			 world_array[c].y = 0.f;
			 
			 c++;

			 world_array[c].z = (float)z + 1;
			 world_array[c].x = 0.f;
			 world_array[c].y = 0.f;
			 
			 c++;
		 }
		 is_right = !is_right;
	 }
	 
 }


LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
	switch (uMsg)
	{
	case WM_CREATE:
		AllocConsole();
		freopen("conout$", "w", stdout);

		gen_vertexes(size); 

		break;

	case WM_LBUTTONDOWN:
		current_point = GetOGLPos(LOWORD(lParam), HIWORD(lParam));
		printf("Point: ( %f %f %f )\n", current_point.x, current_point.y, current_point.z);
		break;

	case WM_SETFOCUS:
		is_focus = true;
		break;

	case WM_KILLFOCUS:
		is_focus = false;
		break;

    case WM_SIZE:										
		if(!g_bFullScreen)								
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);				
		}
        break; 

	case WM_KEYDOWN:

		switch(wParam) {								
			case VK_ESCAPE:								
				PostQuitMessage(0);						
				break;

			case VK_F1:
				is_focus = !is_focus;
				break;
		}
		break;

    case WM_CLOSE:										
        PostQuitMessage(0);								
        break; 
     
    default:											
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										
}