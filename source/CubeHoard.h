#pragma once

#include "GameObject.h"

class CubeHoard
{
public:
	CubeHoard(int numBoxes, double fireInterval,int minX, int maxX, int minZ, int maxZ, int startY,int endY);
	~CubeHoard();
	void init(Box *b, ID3D10EffectMatrixVariable* fx, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
	void update(float dt);
	int checkCollisions(GameObject& object);
private:
	GameObject* attackers;
	int numB;
	int numActive;
	double fireInterval;
	double lastFire;
	float timer;
	int minX,maxX,minZ,maxZ,startY,endY;
};