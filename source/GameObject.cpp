#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	speed = 0;
	active = true;
	rotation = Vector3(0,0,0);
	Identity(&world);
}

GameObject::~GameObject()
{
	box = NULL;
}

void GameObject::draw(D3DXMATRIX model, D3DXMATRIX projection, ID3D10EffectTechnique* technique)
{
	if (!active)
		return;

	mWVP = getWorldMatrix()  *model*projection;
	//sends world view matrix to the shader
	//handle to matrix variable in the shader
	mfxWVPVar->SetMatrix((float*)&mWVP);
	//sets the technique
	setMTech(technique);

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        box->draw();
    }
}

void GameObject::init(Box *b, ID3D10EffectMatrixVariable* fx, float r, Vector3 pos, Vector3 vel, float sp, Vector3 s)
{
	mfxWVPVar = fx;
	box = b;
	radius = r * s.x;
	radius *= 1.3; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
}

void GameObject::update(float dt)
{
	Matrix rotXM, rotYM, rotZM, transM, scaleM;
	position += velocity * dt;
	Identity(&world);
	RotateX(&rotXM,rotation.x);
	RotateY(&rotYM,rotation.y);
	RotateZ(&rotZM,rotation.z);
	Scale(&scaleM,scale.x,scale.y,scale.z);
	Translate(&transM, position.x, position.y, position.z);

	world = scaleM * transM * rotXM * rotYM * rotZM;

}

bool GameObject::collided(GameObject *gameObject)
{
	//extra code that makes sure something is active might change for specific circumstances
	if(!gameObject->getActiveState())
	{
		return false;
	}
	Vector3 diff = position - gameObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + gameObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}