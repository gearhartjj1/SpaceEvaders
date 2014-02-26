//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

//update postion
//update world matrix of where to draw
//multiple world matrix tell it to draw

#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include "Origin.h"
#include "audio.h"
#include <d3dx9math.h>
#include "gameError.h"
#include "input.h"
#include <sstream>
#include <time.h>
#include "LineObject.h"
#include "CubeHoard.h"


class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	Vector3 moveCube();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Audio* audio;
	//Input* input;
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox, redBox, greenBox, shootBox;

	CubeHoard* hitCubes;
	CubeHoard* avoidCubes;

	GameObject floor, leftWall, rightWall, ceiling;
	GameObject tiles[20];
	GameObject wall[20];
	GameObject shootCube;

	//LineObject xLine, yLine, zLine;

	Origin origin;

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	//the location of variable in the shader program like in openGL
	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	float mTheta;
	float mPhi;

    float timer;
    std::wstring mTimer;
	HRESULT hr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

Vector3 ColoredCubeApp::moveCube()
{
	Vector3 direction = Vector3(0,0,0);
	int playerSpeed = 30;

	if(GetAsyncKeyState('A') & 0x8000) direction.x = 1;
	if(GetAsyncKeyState('D') & 0x8000) direction.x = -1;
	if(GetAsyncKeyState('W') & 0x8000) direction.z = -1;
	if(GetAsyncKeyState('S') & 0x8000) direction.z = 1;
	//if(GetAsyncKeyState('O') & 0x8000) direction.y = 1;
	//if(GetAsyncKeyState('L') & 0x8000) direction.y = -1;

	if(direction!=Vector3(0,0,0))
		D3DXVec3Normalize(&direction,&direction);

	direction *= playerSpeed;
	return direction;
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	score = 0;
	timer = 30;
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	srand(time(0));
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	delete audio;
	//delete input;
	delete hitCubes;

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	
	buildFX();
	buildVertexLayouts();

	mBox.init(md3dDevice, 1.0f, D3DXCOLOR(0.5,0.5,0.5,1));
	redBox.init(md3dDevice, 1.0f, DARKBROWN);
	greenBox.init(md3dDevice, 1.0f, D3DXCOLOR(0.1,0.1,0.8,1));
	shootBox.init(md3dDevice, 1.0f, BEACH_SAND);

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	/*xLine.init(&line, mfxWVPVar, Vector3(0,0,0), 1);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&line, mfxWVPVar, Vector3(0,0,0), 1);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&line, mfxWVPVar, Vector3(0,0,0), 1);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));*/

	shootCube.init(&shootBox,mfxWVPVar,sqrt(2.0f),Vector3(10,0,10),Vector3(0,0,0),0,Vector3(1,1,1));
	//shootCube.setRotation(Vector3(ToRadian(-20),ToRadian(45),0));

	leftWall.init(&redBox,mfxWVPVar,sqrt(2.0f),Vector3(20,0,0),Vector3(0,0,0),0,Vector3(0.5,20,100));
	//leftWall.setRotation(Vector3(ToRadian(-20),ToRadian(45),0));

	floor.init(&redBox,mfxWVPVar,sqrt(2.0f),Vector3(0,-5,0),Vector3(0,0,0),0,Vector3(20,0.2,20));
	//floor.setRotation(Vector3(ToRadian(-26),ToRadian(45),0));

	ceiling.init(&mBox,mfxWVPVar,sqrt(2.0f),Vector3(0,20,0),Vector3(0,0,0),0,Vector3(20,0.2,100));
	ceiling.setRotation(Vector3(ToRadian(-20),ToRadian(45),0));

	rightWall.init(&redBox,mfxWVPVar,sqrt(2.0f),Vector3(-20,0,0),Vector3(0,0,0),0,Vector3(0.5,20,100));
	//rightWall.setRotation(Vector3(ToRadian(-20),ToRadian(45),0));

	//srand(time(0));

	for(int i = 0; i < 20; i++)
	{
		tiles[i].init(&greenBox,mfxWVPVar,sqrt(2.0f),Vector3((-5+rand()%10)*3,-100,(-5+rand()%10)*3),Vector3(0,0,0),0,Vector3(1,1,1));
	}

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	quad1.init(md3dDevice, 10, CYAN);
	quad1.setPosition(Vector3(0,0,0));

	spinAmount = 0;

	// init sound system
    audio = new Audio();
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = audio->initialize() ) )
        {
			exit(1);
            //if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system because media file not found."));
            //else
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system."));
        }
    }

	hitCubes = new CubeHoard(100,10,5,5,1,-15,15,-15,15,-100,40);
	hitCubes->init(&redBox,mfxWVPVar,sqrt(4.0f),Vector3(0,0,0),Vector3(0,100,0),70,Vector3(2,2,2));

	avoidCubes = new CubeHoard(100,20,10,10,1,-20,20,-20,20,-100,50);
	avoidCubes->init(&greenBox,mfxWVPVar,sqrt(4.0f),Vector3(0,0,0),Vector3(0,100,0),70,Vector3(2,2,2));

	audio->playCue(MUSIC);

	//input->initialize(this->getMainWnd(), false);  

}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	timer -= dt;
    std::wostringstream outs;   
	if(timer<=0)
	{
		outs.clear();
		outs << L"GAME OVER\nFINAL SCORE: " << score;
		mTimer = outs.str();
		return;
	}

	outs.precision(2);
    outs << L"Time: " << timer << L"\n";
	outs.precision(3);
	outs << "Score: " << score;
	mTimer = outs.str();

	//store old position and reset it if collide, very hackish
	shootCube.setVelocity(moveCube());
	auto oldSpot = shootCube.getPosition();
	D3DApp::updateScene(dt);

	
	leftWall.update(dt);
	//floor.update(dt);
	rightWall.update(dt);
	//ceiling.update(dt);

	hitCubes->update(dt);
	avoidCubes->update(dt);

	//from game jam
	//if(((int)timer)%5==0)
	//{
	//	for(int i = 0; i < 20; i++)
	//	{
	//		if(!tiles[i].getActiveState())
	//		{
	//			tiles[i].setActive();
	//			tiles[i].setPosition(Vector3((-5+rand()%10)*3,0,(-5+rand()%10)*3));
	//			break;
	//		}
	//	}
	//}

	//for(int i = 0; i < 20; i++)
	//{
	//	tiles[i].update(dt);
	//	//wall[i].update(dt);
	//}

	shootCube.update(dt);

	for(int i = 0; i < 20; i++)
	{
		if(shootCube.collided(&tiles[i]))
		{
			tiles[i].setInActive();
			score++;
			audio->playCue(GUN_SHOT);
			//shootCube.setPosition(oldSpot);
		}
		//if(shootCube.collided(&wall[i]))
		//{
		//	//shootCube.setPosition(oldSpot);
		//	wall[i].setInActive();
		//}
	}
	
	int numHits = hitCubes->checkCollisions(shootCube);

	score+=numHits;

	int numBadHits = avoidCubes->checkCollisions(shootCube);
	score-=numBadHits;

	if(numHits>0)
	{
		audio->playCue(GUN_SHOT);
	}

	if(numBadHits>0)
	{
		audio->playCue(TARGET_SHATTER);
	}

	if(shootCube.getPosition().x<-18 || shootCube.getPosition().x>18 || shootCube.getPosition().z<-18 || shootCube.getPosition().z>18)
	{
		shootCube.setPosition(oldSpot);
	}

	// Build the view matrix.
	D3DXVECTOR3 pos(0.0f,50.0f,0.001f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	// set some variables for the shader
	int foo[1];
	foo[0] = 0;
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	
	rightWall.draw(mView, mProj, mTech);
	//floor.draw(mView, mProj, mTech);
	leftWall.draw(mView, mProj, mTech);
	//ceiling.draw(mView, mProj, mTech);

	shootCube.draw(mView, mProj, mTech);

	hitCubes->draw(mView,mProj,mTech);

	avoidCubes->draw(mView,mProj,mTech);

	//for(int i = 0; i < 20; i++)
	//{
	//	tiles[i].draw(mView, mProj, mTech);
	//	//wall[i].draw(mView,mProj,mTech);
	//}

	//draw the origin
	origin.draw(mView, mProj, mTech);
     
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxFLIPVar = mFX->GetVariableByName("flip");

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 