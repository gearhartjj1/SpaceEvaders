#pragma once

#include "Box.h"
#include <cmath>
#include "d3dUtil.h"
#include "GameObject.h"

namespace indicatorNS {
	const D3DXCOLOR COLOR   = BEACH_SAND;
	const float     SCALE   = 0.25f;
	const int       TOTAL   = 100;
	const float     Y_INDEX = 35.0f;
}

class Indicator {
public : 
	Indicator();
	~Indicator();

	void draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique *technique);
	void init(ID3D10Device *md3dDevice, D3DXMATRIX *mWVP, ID3D10EffectMatrixVariable *mfxWVPVar);
	void update(float dt);

	GameObject go[indicatorNS::TOTAL];

private : 
	Box b;

	D3DXMATRIX *mWVP;
	ID3D10Device *md3dDevice;
	ID3D10EffectMatrixVariable *mfxWVPVar;
};