#ifndef Constants_H
#define Constants_H

//macros from kelly code
//-----------------------------------------------
// Useful macros
//-----------------------------------------------
// Safely delete pointer referenced item
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// Safely release pointer referenced item
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// Safely delete pointer referenced array
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }
// Safely call onLostDevice
#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }
// Safely call onResetDevice
#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }
#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255)  // transparent color (magenta)

const UINT GAME_WIDTH =  1000;              // width of game in pixels
const UINT GAME_HEIGHT = 600;               // height of game in pixels

//Defines to make life easier
#define Vector3 D3DXVECTOR3
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define Scale D3DXMatrixScaling
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree

//audio stuff
//C:\Users\GEARHARTJJ1\Documents\College\Junior year\Spring\Games 2\Game Jam 1 Gearhart Young
const char WAVE_BANK[] = "C:\\Users\\GEARHARTJJ1\\Documents\\College\\Junior year\\Spring\\Games 2\\Game Jam 1 Gearhart Young\\audio\\win\\Wave Bank.xwb";
const char SOUND_BANK[] = "C:\\Users\\GEARHARTJJ1\\Documents\\College\\Junior year\\Spring\\Games 2\\Game Jam 1 Gearhart Young\\audio\\win\\Sound Bank.xsb";
//sound cues
const char GUN_SHOT[] = "gun_sound_effect";
const char TARGET_SHATTER[] = "Light Bulb Breaking-SoundBible.com-53066515";
const char MUSIC[] = "04 All of Us";



#endif