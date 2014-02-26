#include "CubeHoard.h"

CubeHoard::CubeHoard(int numBoxes, int maxAtTime,int minAtTime, double levelTime, double fInterval, int minX, int maxX, int minZ, int maxZ, int startY, int endY)
{
	attackers = new GameObject[numBoxes];
	numB = numBoxes;
	numActive = 0;
	fireInterval = fInterval;
	lastFire = 0;
	timer = 0;
	lastUpdate = 0;
	this->minX = minX;
	this->maxX = maxX;
	this->minZ = minZ;
	this->maxZ = maxZ;
	this->startY = startY;
	this->endY = endY;
	this->numSent = maxAtTime;
	this->minSent = minAtTime;
	this->levelTime = levelTime;
}

CubeHoard::~CubeHoard()
{
	delete [] attackers;
}

void CubeHoard::init(Box *b, ID3D10EffectMatrixVariable* fx, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	for(int i = 0; i < numB; i++)
	{
		attackers[i].init(b,fx,r,pos,vel,sp,s);
	}
}

void CubeHoard::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique)
{
	for(int i = 0; i < numB; i++)
	{
		if(attackers[i].getActiveState())
			attackers[i].draw(model,projection,technique);
	}
}

void CubeHoard::update(float dt)
{
	//need to work on the timer
	timer+=dt;
	if((timer-lastFire)>fireInterval)
	{
		lastFire = timer;
		int send = minSent + rand()%numSent;
		for(int i = 0; i < send; i++)
		{
			//might not be the best implementation for this
			for(int j = 0; j < numB; j++)
			{
				if(!attackers[j].getActiveState())
				{
					attackers[j].setActive();
					attackers[j].setPosition(Vector3(minX + rand()%(maxX-minX),startY,minZ + rand()%(maxZ-minZ)));
					break;
				}
			}
		}
		//numActive++;
	}
	for(int i = 0; i < numB; i++)
	{
		if(attackers[i].getActiveState())
			attackers[i].update(dt);
	}
	for(int i = 0; i < numB; i++)
	{
		if(attackers[i].getActiveState())
		{
			if(attackers[i].getPosition().y >= endY)
			{
				attackers[i].setInActive();
			}
		}
	}

	if(levelTime>0 && (timer-lastUpdate)>levelTime)
	{
		lastUpdate = timer;
		fireInterval*=0.9;
		numSent++;
	}
}

int CubeHoard::checkCollisions(GameObject& object)
{
	int numHits = 0;
	for(int i = 0; i < numB; i++)
	{
		if(attackers[i].getActiveState())
		{
			if(attackers[i].collided(&object))
			{
				attackers[i].setInActive();
				numHits++;
			}
		}
	}
	return numHits;
}