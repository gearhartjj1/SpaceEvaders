#pragma once

#include "GameObject.h"

//make levelTime negative to prevent the cube variables from changing
class CubeHoard
{
public:
	CubeHoard(int numBoxes, int maxAtTime,int minAtTime, double levelTime, double fireInterval,int minX, int maxX, int minZ, int maxZ, int startY,int endY);
	~CubeHoard();
	void init(Box *b, ID3D10EffectMatrixVariable* fx, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s);
	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique);
	void update(float dt);
	int checkCollisions(GameObject& object);
private:
	GameObject* attackers;
	int numB;
	int numActive;
	int numSent;
	int minSent;
	double fireInterval;
	double lastFire;
	float timer;
	double levelTime;
	double lastUpdate;
	int minX,maxX,minZ,maxZ,startY,endY;
};