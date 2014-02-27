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

	void updateGameState();

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Audio* audio;
	//Input* input;
	float score;
	int multiplier;
	Quad quad1;
	int foo[1];
	int normalColor[1];
	Line line, line2, line3;
	Box mBox, redBox, greenBox, shootBox;

	CubeHoard* hitCubes;
	CubeHoard* avoidCubes;
	CubeHoard* powerCubes;

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
	float lastSwitch;
    std::wstring mTimer;
	HRESULT hr;

	GameStates gamestate;
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
	int playerSpeed = 20;

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
	multiplier = 5;
	score = 0;
	foo[0] = 0;
	normalColor[0] = 0;
	lastSwitch = 100;
	timer = 100;
	gamestate = Title;
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

	shootCube.init(&shootBox,mfxWVPVar,sqrt(2.0f),Vector3(0,20,0),Vector3(0,0,0),0,Vector3(1,1,1));
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

	HoardData hitCubeData;
	hitCubeData.numBoxes = 20;//total cubes available to the hoard
	hitCubeData.maxAtTime = 6;//max number of cubes that can be sent at a time
	hitCubeData.minAtTime = 2;//min number of cubes that can be sent at a time
	hitCubeData.levelTime = -1;//time between difficulty increasing in seconds
	hitCubeData.fireInterval = 3;//number of seconds between cubes firing
	hitCubeData.minX = -15;//min point where cubes appear in the x direction
	hitCubeData.maxX = 15;//max point where cubes appear in the x direction
	hitCubeData.minZ = -15;//min point where cubes appear in the z direction
	hitCubeData.maxZ = 15;//max point where cubes appear in the z direction
	hitCubeData.startY = -100;//starting point of cubes in the y direction
	hitCubeData.endY = 40;//end point of cubes in the y direction, so where cubes disappear
	hitCubeData.startTime = 5;

	hitCubes = new CubeHoard(hitCubeData);
	hitCubes->init(&redBox,mfxWVPVar,sqrt(4.0f),Vector3(0,0,0),Vector3(0,100,0),70,Vector3(2,2,2));

	HoardData avoidCubeData;
	avoidCubeData.numBoxes = 100;
	avoidCubeData.maxAtTime = 30;
	avoidCubeData.minAtTime = 15;
	avoidCubeData.levelTime = 20;
	avoidCubeData.fireInterval = 3;
	avoidCubeData.minX = -20;
	avoidCubeData.maxX = 20;
	avoidCubeData.minZ = -20;
	avoidCubeData.maxZ = 20;
	avoidCubeData.startY = -100;
	avoidCubeData.endY = 50;

	avoidCubes = new CubeHoard(avoidCubeData);
	avoidCubes->init(&greenBox,mfxWVPVar,sqrt(4.0f),Vector3(0,0,0),Vector3(0,100,0),70,Vector3(2,2,2));

	HoardData powerCubeData;
	powerCubeData.numBoxes = 10;
	powerCubeData.maxAtTime = 5;
	powerCubeData.minAtTime = 1;
	powerCubeData.levelTime = 40;
	powerCubeData.fireInterval = 5;
	powerCubeData.minX = -20;
	powerCubeData.maxX = 20;
	powerCubeData.minZ = -20;
	powerCubeData.maxZ = 20;
	powerCubeData.startY = -100;
	powerCubeData.endY = 50;
	powerCubeData.startTime = 10;
	//adjust these Values!!!

	powerCubes = new CubeHoard(powerCubeData);
	powerCubes->init(&shootBox,mfxWVPVar,sqrt(4.0f),Vector3(0,0,0),Vector3(0,100,0),70,Vector3(1,1,1));

	audio->playCue(MUSIC);

	//input->initialize(this->getMainWnd(), false);  

}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateGameState()
{
	if(gamestate == Title && (GetAsyncKeyState('E') & 0x8000))
		gamestate = Gameplay;
	if(multiplier <= 0)
		gamestate = Retry;
	if(gamestate == Retry && (GetAsyncKeyState('Y') & 0x8000))
	{
		multiplier = 1;
		score = 0;
		foo[0] = 0;
		normalColor[0] = 0;
		lastSwitch = 100;
		timer = 100;
		gamestate = Gameplay;
	}
}

void ColoredCubeApp::updateScene(float dt)
{
	updateGameState();
	std::wostringstream outs;  
	if(gamestate == Title)
	{
		outs.clear();
		outs << L"Press E to play";
		mTimer = outs.str();
	}
	if(gamestate == Gameplay)
	{
		timer -= dt;
		//outs.precision(4);
		outs << L"Score: " << score << L"\n";
		//outs.precision(3);
		outs << "Multiplier: " << multiplier;
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
		powerCubes->update(dt);
		shootCube.update(dt);
	
		int numHits = hitCubes->checkCollisions(shootCube);
		multiplier += numHits;

		int numBadHits = avoidCubes->checkCollisions(shootCube);
		multiplier -= numBadHits;

		int numPowerHits = powerCubes->checkCollisions(shootCube);
		//do stuff with this!!!

		if(numHits>0)
		{
			audio->playCue(GUN_SHOT);
		}

		if(numBadHits>0)
		{
			audio->playCue(TARGET_SHATTER);
		}

		if(shootCube.getPosition().x<-16 || shootCube.getPosition().x>16 || shootCube.getPosition().z<-12 || shootCube.getPosition().z>12)
		{
			shootCube.setPosition(oldSpot);
		}
	}
	if(gamestate == Retry)
	{
		outs.clear();
		outs << L"Press Y to play again";
		mTimer = outs.str();
		avoidCubes->reset();
		hitCubes->reset();
		shootCube.setPosition(Vector3(0,20,0));
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
	//int foo[1];
	//foo[0] = 2;
	if((lastSwitch-timer)>1)
	{
		lastSwitch = timer;
		foo[0]++;
		if(foo[0]>3)
		{
			foo[0] = 0;
		}
		score += multiplier;
	}
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	if(gamestate == Title)
	{
		RECT R = {GAME_WIDTH/2 - 60, GAME_HEIGHT/2, 0, 0};
		mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	}
	if(gamestate == Gameplay)
	{
		rightWall.draw(mView, mProj, mTech);
		//floor.draw(mView, mProj, mTech);
		leftWall.draw(mView, mProj, mTech);
		//ceiling.draw(mView, mProj, mTech);

		mfxFLIPVar->SetRawValue(&normalColor[0], 0, sizeof(int));
		shootCube.draw(mView, mProj, mTech);

		hitCubes->draw(mView,mProj,mTech);

		avoidCubes->draw(mView,mProj,mTech);

		powerCubes->draw(mView,mProj,mTech);

		//for(int i = 0; i < 20; i++)
		//{
		//	tiles[i].draw(mView, mProj, mTech);
		//	//wall[i].draw(mView,mProj,mTech);
		//}

		//draw the origin
		//origin.draw(mView, mProj, mTech);
     
		// We specify DT_NOCLIP, so we do not care about width/height of the rect.
		RECT R = {5, 5, 0, 0};
		mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	}
	if(gamestate == Retry)
	{
		RECT R = {GAME_WIDTH/2 - 75, GAME_HEIGHT/2, 0, 0};
		mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, RED);
	}
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
 