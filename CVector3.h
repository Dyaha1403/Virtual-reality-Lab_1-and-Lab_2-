#pragma once


struct CVector3
{
public:

	
	CVector3() {}

	
	CVector3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	
	CVector3 operator+(CVector3 vVector)
	{
		
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	
	CVector3 operator-(CVector3 vVector)
	{
		
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}

	
	CVector3 operator*(float num)
	{
		
		return CVector3(x * num, y * num, z * num);
	}

	
	CVector3 operator/(float num)
	{
		
		return CVector3(x / num, y / num, z / num);
	}

	bool operator==(CVector3 vVector)
	{
		return vVector.x == x && vVector.y == y && vVector.z == z;
	}

	float x, y, z;
};
