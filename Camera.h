#ifndef _CAMERA_H
#define _CAMERA_H
#include "main.h"


class CCamera {

public:

	
	CCamera();	

	
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}
	
	
	
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	
	void RotateView(float angle, float X, float Y, float Z);

	
	void SetViewByMouse(); 

	
	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);

	
	void StrafeCamera(float speed);

	
	void MoveCamera(float speed);

	
	void CheckForMovement();

	
	void Update();

	
	void Look();



	
	CVector3 m_vPosition;					

	
	CVector3 m_vView;						

	
	CVector3 m_vUpVector;		
	
	
	CVector3 m_vStrafe;						
};

#endif












