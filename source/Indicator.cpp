#include "Indicator.h"

Indicator::Indicator() { }
Indicator::~Indicator() { }

void Indicator::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique *technique) {
	for(int i = 0; i < indicatorNS::TOTAL; ++i) {
		go[i].draw(model, projection, technique);
	}
}

void Indicator::init(ID3D10Device *md3dDevice, D3DXMATRIX *mWVP, ID3D10EffectMatrixVariable *mfxWVPVar) {
	b.init(md3dDevice, indicatorNS::SCALE, indicatorNS::COLOR);

	for(int i = 0; i < indicatorNS::TOTAL; ++i) {
		go[i].init(&b, mfxWVPVar, sqrt(2.0f), D3DXVECTOR3(0, indicatorNS::Y_INDEX, 0), D3DXVECTOR3(0, 0, 0), 0, Vector3(1,1,1));
	}

	this->md3dDevice = md3dDevice;
	this->mfxWVPVar = mfxWVPVar;
	this->mWVP = mWVP;
}

void Indicator::update(float dt) {
	for(int i = 0; i < indicatorNS::TOTAL; ++i) {
		go[i].update(dt);
	}
}