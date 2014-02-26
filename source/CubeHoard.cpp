#include "CubeHoard.h"

HoardData::HoardData()
{
	numBoxes = 10;
	maxAtTime = 1;
	minAtTime = 10;
	levelTime = -1;
	fireInterval = 5;
	minX = -20;
	maxX = 20;
	minZ = -20;
	maxZ = 20;
	startY = -100;
	endY = 40;
}

CubeHoard::CubeHoard(HoardData data)
{
	attackers = new GameObject[data.numBoxes];
	numB = data.numBoxes;
	numActive = 0;
	fireInterval = data.fireInterval;
	lastFire = 0;
	timer = 0;
	lastUpdate = 0;
	this->minX = data.minX;
	this->maxX = data.maxX;
	this->minZ = data.minZ;
	this->maxZ = data.maxZ;
	this->startY = data.startY;;
	this->endY = data.endY;
	this->numSent = data.maxAtTime;
	this->minSent = data.minAtTime;
	this->levelTime = data.levelTime;
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
					attackers[j].setPosition(Vector3(((minX/4) + rand()%((maxX-minX)/4))*4,startY,((minZ/4) + rand()%((maxZ-minZ)/4))*4));
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