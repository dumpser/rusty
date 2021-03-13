#include "hMain.h"
#include "COM.h"
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include "smmintrin.h"
#include "xmmintrin.h"
#include "emmintrin.h"
#include "Variable.h"
using namespace std;
#pragma warning(disable:4996)

DWORD64 CAMPTR = 0x1747E40;
DWORD64 GAPTR = 0x2818118; // 0x27014A0 - 48 8B 05 CB144A02     - mov rax,[GameAssembly.dll+27014A0] { (1F213628EB0) }

//#define FULL

enum Bone_List : int 
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,

	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement,
};

class PlayerBase
{
public:
	char pad_0x0000[0x1EC]; //0x0000
	__int32 lifestate; //0x01EC 
	char pad_0x01F0[0x4]; //0x01F0
	float hp; //0x01F4 
	char pad_0x01F8[0x298]; //0x01F8
	__int64 playermodel; //0x0490 
	char pad_0x0498[0x60]; //0x0498
	__int32 team; //0x04F8 
	char pad_0x04FC[0x34]; //0x04FC
	__int32 activeitem; //0x0530 
	char pad_0x0534[0xC]; //0x0534
	__int64 modelstate; //0x0540 
	char pad_0x0548[0x68]; //0x0548
	__int32 flags; //0x05B0 
	char pad_0x05B4[0xC]; //0x05B4
	__int64 inventory; //0x05C0 
	char pad_0x05C8[0x10]; //0x05C8
	__int64 input; //0x05D8 
	__int64 movement; //0x05E0 
	char pad_0x05E8[0x30]; //0x05E8
	__int64 nameptr; //0x0618 

}; //Size=0x0620


struct Player
{
	DWORD64 base;
	DWORD64 vector;
	string name;
	int layer;
	float distance;
};

struct VECTOR3D
{
	float x;
	float y;
	float z;
};

struct MATRIX
{
	float _11;
	float _12;
	float _13;
	float _14;
	float _21;
	float _22;
	float _23;
	float _24;
	float _31;
	float _32;
	float _33;
	float _34;
	float _41;
	float _42;
	float _43;
	float _44;
};

struct Matrix34
{
	D3DXVECTOR4 vec0;
	D3DXVECTOR4 vec1;
	D3DXVECTOR4 vec2;
};

Player Gamer[1000];

int playercount = 0;

DWORD64 baseAdress;
DWORD64 baseAdressGA;
DWORD64 viewMatrixPtr = NULL;
DWORD64 vectorPtr;
DWORD64 healthDecAddress = NULL;


DWORD64 localplayer;
D3DXMATRIX viewMatrix;
D3DXVECTOR3 myloc;
D3DXVECTOR3 enemyloc;
KeInterface Driver;
MATRIX mat;
float distanceaim = NULL;
D3DXVECTOR3 aimloc;


void DR();
void GetIni();
void GE();
void BN();

//void SG(DWORD64 inventory, int currentitem);
D3DXVECTOR3 GBP(DWORD64 pTransform);
DWORD64 GEB(DWORD64 entity, int bone_index);
D3DXVECTOR2 WTS(D3DXVECTOR3 loc);
D3DXVECTOR2 SETA(D3DXVECTOR3* LocalPos, D3DXVECTOR3* EnemyPos);
wstring WN(DWORD64 inventory, int currentitem);
void SG(DWORD64 inventory, int currentitem);
D3DXVECTOR3 myheadpos;

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	GetIni();
	Driver.Create(); //\\\\.\\Global\\MyDriver1 \\\\.\\QTEnginer
	while (1)
	{
		if (GetAsyncKeyState(VK_INSERT))
			break;
		if (GetAsyncKeyState(VK_DELETE))
		{
			Driver.Close();
			exit(1);
		}
			
		Sleep(50);
	}
 	baseAdress = Driver.GetClientModule();
	baseAdressGA = Driver.GetClientModuleGA();
	
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)BN, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DR, 0, 0, 0);
	getchar();
	return 0;
}

void GetIni()
{
	screenx = GetPrivateProfileInt("cfg", "x", 0, ".\\cfg.ini");
	screeny = GetPrivateProfileInt("cfg", "y", 0, ".\\cfg.ini");
	dist = GetPrivateProfileInt("cfg", "r", 0, ".\\cfg.ini");
	sleep = GetPrivateProfileInt("cfg", "st", 0, ".\\cfg.ini");
	fps = GetPrivateProfileInt("cfg", "fp", 0, ".\\cfg.ini");
	keycard = GetPrivateProfileInt("cfg", "di", 0, ".\\cfg.ini");
	minicopter = GetPrivateProfileInt("cfg", "mc", 0, ".\\cfg.ini");
	crate = GetPrivateProfileInt("cfg", "bx", 0, ".\\cfg.ini");
	animal = GetPrivateProfileInt("cfg", "an", 0, ".\\cfg.ini");
	teamesp = GetPrivateProfileInt("cfg", "te", 0, ".\\cfg.ini");
	weaponname = GetPrivateProfileInt("cfg", "wn", 0, ".\\cfg.ini");
	ores = GetPrivateProfileInt("cfg", "or", 0, ".\\cfg.ini");
	npc = GetPrivateProfileInt("cfg", "np", 0, ".\\cfg.ini");
	bp = GetPrivateProfileInt("cfg", "bp", 0, ".\\cfg.ini");
	if (!screenx)
		MessageBox(0, "2", "Error", MB_OK);
}

float LEN(D3DXVECTOR3& playerFrom, D3DXVECTOR3& playerTo)
{
	float m_x = playerFrom.x - playerTo.x;
	float m_y = playerFrom.y - playerTo.y;
	float m_z = playerFrom.z - playerTo.z;
	return (float)sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}



void DR()
{
	HWND hwnd = GetDesktopWindow();
	HDC hdc = GetDC(hwnd);

	SetBkMode(hdc, OPAQUE);
	HFONT hFont = CreateFont(10, 0, 0, 0, FW_BOLD,false, false, false, ANSI_CHARSET,OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH,TEXT("Comic Sans MS")/*"SYSTEM_FIXED_FONT"*/);


	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));
	HPEN hPen = CreatePen(PS_INSIDEFRAME, 1, RGB(255, 0, 0));
	SelectObject(hdc, hPen);
	
	while (localplayer)
	{
		try {
			float screenDist = 0;
			bool isfirst = true;
			float nearestdist = 0;
			D3DXVECTOR3 temploc = { 0,0,0 };
			DWORD64 tempmovement;
			float tempdist;

			PlayerBase localplayerfull = Driver.ReadNewShit<PlayerBase>(localplayer, sizeof(PlayerBase));

			if (viewMatrixPtr != NULL)
			{
				mat = Driver.ReadNewShit<MATRIX>(viewMatrixPtr + 0x2E4, sizeof(MATRIX));
			}
			if ((GetAsyncKeyState(VK_F9)))
			{
				Driver.Close();
				exit(1);
			}
			if ((GetAsyncKeyState(VK_F8)))
			{
				GetIni();
			}
			if ((GetAsyncKeyState(VK_F4)))
			{
				Driver.WriteVirtualMemory(localplayer + 0x5b0, 260, sizeof(260));
				Sleep(50);
			}
			if ((GetAsyncKeyState(0x58) && localplayerfull.input))
			{
				// Jump
				SG(localplayerfull.inventory, localplayerfull.activeitem);
				//Sleep(50);
				// Restore
			}
#ifdef _DEBUG
			if ((GetAsyncKeyState(0x42)))
			{
				// Jump
				Driver.WriteVirtualMemory(localplayerfull.movement + 0xb0, 0, sizeof(0));
				//Sleep(50);
				// Restore
			}




			if (GetAsyncKeyState(0x43))//0x43
			{
				//float x = 0.1;aimloc.x
				if (aimloc.x)
				{
					wstring wpname = WN(localplayerfull.inventory, localplayerfull.activeitem);
					float vel;
					if (wpname.find(L"w.h") != string::npos || wpname.find(L"lg") != string::npos)
					{
						vel = 55;
					}
					else if (wpname.find(L"e.b") != string::npos)
					{
						vel = 656.25;
					}
					else if (wpname.find(L"w.c") != string::npos)
					{
						vel = 160.0f;
					}
					else if (wpname.find(L"l9") != string::npos)
					{
						vel = 700;
					}
					else if (wpname.find(L"e.s") != string::npos)
					{
						vel = 375;
					}
					else if (wpname.find(L"e.a") != string::npos)
					{
						vel = 375;
					}
					else if (wpname.find(L"49") != string::npos)
					{
						vel = 450;
					}
					else if (wpname.find(L"sb") != string::npos)
					{
						vel = 90;
					}
					else
					{
						vel = 300;
					}
					float time = tempdist / vel;
					//aimloc.x += enemyvel.x * time;
					//aimloc.y += enemyvel.y * time;
					//aimloc.z += enemyvel.z * time;
					aimloc.y += 0.03f + (float)((double)4.905f * (double)time * (double)time);
					myheadpos = GBP(GEB(Gamer[0].base, head));
					D3DXVECTOR2 angle = SETA(&myheadpos, &aimloc);

					Driver.WriteVirtualMemoryFloat(localplayerfull.input + 0x3c, angle.x, sizeof(float));
					Driver.WriteVirtualMemoryFloat(localplayerfull.input + 0x40, angle.y, sizeof(float));

					SG(localplayerfull.inventory, localplayerfull.activeitem);
				}
			}



#endif
			for (int sayac = 1; sayac < playercount; sayac++)
			{

				if (Gamer[sayac].layer != 17)
				{
					enemyloc = Driver.ReadNewShit<D3DXVECTOR3>(Gamer[sayac].vector + 0x90, sizeof(D3DXVECTOR3));

					myloc = Driver.ReadNewShit<D3DXVECTOR3>(vectorPtr + 0x90, sizeof(D3DXVECTOR3));

					float distance = LEN(enemyloc, myloc);
					if (distance > Gamer[sayac].distance)continue;
					RECT textpos;
					D3DXVECTOR3 translationVector = D3DXVECTOR3(mat._14, mat._24, mat._34);
					D3DXVECTOR3 up = D3DXVECTOR3(mat._12, mat._22, mat._32);
					D3DXVECTOR3 right = D3DXVECTOR3(mat._11, mat._21, mat._31);
					D3DXVECTOR3 forward = D3DXVECTOR3(mat._13, mat._23, mat._33);
					float w = D3DXVec3Dot(&translationVector, &enemyloc) + mat._44;
					if (w < 0.098f)continue;
					float y = D3DXVec3Dot(&up, &enemyloc) + mat._42;
					float x = D3DXVec3Dot(&right, &enemyloc) + mat._41;
					textpos.right = (screenx) * (1.f + x / w);
					textpos.top = (screeny / 2) * (1.f - y / w) + 20;
					textpos.bottom = 0;
					textpos.left = 0;

					DrawTextExA(hdc, (char*)Gamer[sayac].name.c_str(), Gamer[sayac].name.size(), &textpos, DT_CENTER | DT_NOCLIP | DT_NOPREFIX, NULL);
				}
				else
				{

					PlayerBase enemy = Driver.ReadNewShit<PlayerBase>(Gamer[sayac].base, sizeof(PlayerBase));


					//DWORD64 modelstate = Driver.ReadVirtualMemory<DWORD64>(Gamer[sayac].base + 0x500, sizeof(DWORD64));
					__int32 model = Driver.ReadVirtualMemory<__int32>(enemy.modelstate + 0x24, sizeof(__int32));
					if (model == 12 || model == 13 || model == 8) //1028
						continue;

					//__int64 myteam = Driver.ReadVirtualMemory<DWORD64>(localplayerfull.team, sizeof(DWORD64));
					//__int64 team = Driver.ReadVirtualMemory<DWORD64>(Gamer[sayac].base + 0x5a0, sizeof(DWORD64));

					if (!teamesp && localplayerfull.team != 0 && enemy.team == localplayerfull.team)
					{
						continue;
					}


					//__int32 lifestate = Driver.ReadVirtualMemory32<__int32>(Gamer[sayac].base + 0x1C8, sizeof(__int32));
					if (enemy.lifestate == 1 || enemy.hp <= 0)continue;

					float hp = enemy.hp;

					if (!npc)
						if (hp > 105)continue;


					enemyloc = Driver.ReadNewShit<D3DXVECTOR3>(Gamer[sayac].vector + 0x90, sizeof(D3DXVECTOR3));

					//enemyloc.x = Driver.ReadVirtualMemory32<float>(Gamer[sayac].vector + 0xB0, sizeof(float));
					//enemyloc.y = Driver.ReadVirtualMemory32<float>(Gamer[sayac].vector + 0xB4, sizeof(float));
					//enemyloc.z = Driver.ReadVirtualMemory32<float>(Gamer[sayac].vector + 0xB8, sizeof(float));

					myloc = Driver.ReadNewShit<D3DXVECTOR3>(vectorPtr + 0x90, sizeof(D3DXVECTOR3));

					float distance = LEN(enemyloc, myloc);
					if (distance > dist)continue;

					//std::cout << "Dist" << distance << std::endl;

					//viewMatrix = Driver.ReadNewShit<MATRIX16>(viewMatrixPtr + 0x2E4, sizeof(MATRIX16));



					RECT textpos;

					D3DXVECTOR3 translationVector = D3DXVECTOR3(mat._14, mat._24, mat._34);
					D3DXVECTOR3 up = D3DXVECTOR3(mat._12, mat._22, mat._32);
					D3DXVECTOR3 right = D3DXVECTOR3(mat._11, mat._21, mat._31);
					D3DXVECTOR3 forward = D3DXVECTOR3(mat._13, mat._23, mat._33);
					float w = D3DXVec3Dot(&translationVector, &enemyloc) + mat._44;
					if (w < 0.098f)continue;
					float y = D3DXVec3Dot(&up, &enemyloc) + mat._42;
					float x = D3DXVec3Dot(&right, &enemyloc) + mat._41;
					textpos.right = (screenx) * (1.f + x / w);
					float screeh = 830.0f / distance;
					textpos.top = (screeny / 2) * (1.f - y / w) + 40;
					textpos.bottom = 0;
					textpos.left = 0;
					//std::cout << screex << "-" << screey << std::endl;

					//DWORD64 nameptr = Driver.ReadVirtualMemory<DWORD64>(Gamer[sayac].base + 0x4b0, sizeof(DWORD64));
					__int32 uzunluk = Driver.ReadVirtualMemory<__int32>(enemy.nameptr + 0x10, sizeof(__int32));
					std::wstring name = Driver.ReadName(enemy.nameptr + 0x14, uzunluk);

#ifdef _DEBUG
					D3DXVECTOR3 headpos = GBP(GEB(Gamer[sayac].base, head));
					D3DXVECTOR2 HP = WTS(headpos);

					screenDist = sqrt(pow(screeny / 2 - (HP.y), 2) + pow(screenx / 2 - (HP.x), 2));
#endif
					std::wstring draw = name + L"|" + std::to_wstring((int)hp) + L"|" + std::to_wstring((int)distance);

					if (weaponname)
					{
						if (enemy.activeitem)
						{
							wstring wpname = WN(enemy.inventory, enemy.activeitem);
							if (wpname.size() >= 2)
								draw.append(L"\n" + wpname);
						}
					}

					DrawTextExW(hdc, (wchar_t*)draw.c_str(), draw.size(), &textpos, DT_CENTER | DT_NOCLIP | DT_NOPREFIX, NULL);

#ifdef _DEBUG
					if (isfirst && distance < 200 && screenDist < 150)
					{
						nearestdist = screenDist;
						temploc = headpos;
						tempdist = distance;
						isfirst = false;
					}
					else
					{
						if (nearestdist > screenDist&& distance < 200 && screenDist < 150)
						{
							nearestdist = screenDist;
							tempdist = distance;
							temploc = headpos;
						}
				}
#endif
					aimloc = temploc;
			}
		}
	}
	catch (int e)
	{
		MessageBox(NULL, "1", "5", MB_OK);
	}
	}

}

void GE()
{
	try {
		
		DWORD64 CGameObjectManager = Driver.ReadVirtualMemory<DWORD64>(baseAdress + CAMPTR, sizeof(DWORD64));

		DWORD64 ptrchain = Driver.ReadVirtualMemory<DWORD64>(CGameObjectManager, sizeof(DWORD64));

		DWORD64 ptrchain1 = Driver.ReadVirtualMemory<DWORD64>(ptrchain, sizeof(DWORD64));

		DWORD64 camera = Driver.ReadVirtualMemory<DWORD64>(ptrchain1 + 0x30, sizeof(DWORD64));

		DWORD64 ptrchain2 = Driver.ReadVirtualMemory<DWORD64>(camera + 0x30, sizeof(DWORD64));

		viewMatrixPtr = Driver.ReadVirtualMemory<DWORD64>(ptrchain2 + 0x18, sizeof(DWORD64));

		DWORD64 ptrchain3 = Driver.ReadVirtualMemory<DWORD64>(ptrchain2 + 0x8, sizeof(DWORD64));

		vectorPtr = Driver.ReadVirtualMemory<DWORD64>(ptrchain3 + 0x38, sizeof(DWORD64));
		
	}
	catch (DWORD64 e)
	{

		MessageBox(NULL, "1", "2", MB_OK);
	}

}

void BN()
{
	while (1)
	{
		int objectcounter = 0;
		DWORD64 buffer = Driver.ReadVirtualMemory<DWORD64>(baseAdressGA + GAPTR, sizeof(DWORD64));
		GE();
		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer + 0xB8, sizeof(DWORD64));
		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer, sizeof(DWORD64));
		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x10, sizeof(DWORD64));
		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x28, sizeof(DWORD64));
		//printf("%p\n", buffer);
		int size = Driver.ReadVirtualMemory<DWORD>(buffer + 0x10, sizeof(DWORD64));
		DWORD64 ObjectList = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x18, sizeof(DWORD64));


		try 
		{
		//printf("%d Size\n", size);
		for (int i = 0; i < size; i++)
		{
			DWORD64 object = Driver.ReadVirtualMemory<DWORD64>(ObjectList + 0x20 + (0x8 * i), sizeof(DWORD64));
			object = Driver.ReadVirtualMemory<DWORD64>(object + 0x10, sizeof(DWORD64));
			object = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
			int layer = Driver.ReadVirtualMemory<int>(object + 0x50, sizeof(int));

			//DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
			//string name = Driver.ReadNameS(NAMEPTR, 150);

			//myfile  << "-" << layer << "-" << name << endl;

			if (layer == 30 || layer == 21 || layer == 8)
				continue;

			if (layer == 0 && crate)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				if ((name.find("e_n") != string::npos || name.find("e_el") != string::npos) && crate)
				{
					Gamer[objectcounter].name = name.substr(37, 9);
					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));

					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
					Gamer[objectcounter].layer = 0;
					Gamer[objectcounter].distance = dist;
					objectcounter++;
					continue;
				}
				continue;
			}
			if (layer == 0 && ores)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				if (name.find("e/o") != string::npos) //e/o r-o
				{
					Gamer[objectcounter].name = name.substr(47, 7);
					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));

					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
					Gamer[objectcounter].layer = 0;
					Gamer[objectcounter].distance = 200;
					objectcounter++;
					continue;
				}
			}

			if (layer == 17)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				Gamer[objectcounter].name = name;
				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
				//Vector3D vec = Driver.ReadNewShit<Vector3D>(Gamer[objectcounter].vector + 0xB0, sizeof(Vector3D));
				ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
				Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
				if (name.find("lP") != string::npos)
					localplayer = Gamer[objectcounter].base;
				Gamer[objectcounter].layer = 17;
				Gamer[objectcounter].distance = dist;
				objectcounter++;
				//printf("%d Player Found\n", objectcounter);
				continue;
			}
			if (layer == 9 && bp)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				Gamer[objectcounter].name = name.substr(40, 8);
				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
				Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
				Gamer[objectcounter].layer = 9;
				Gamer[objectcounter].distance = dist;
				objectcounter++;
				continue;
			}
			if (layer == 11 && animal)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				Gamer[objectcounter].name = name.substr(22, 4);
				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
				Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
				Gamer[objectcounter].layer = 11;
				Gamer[objectcounter].distance = dist;
				objectcounter++;
				continue;
			}
			if (layer == 13 && minicopter)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				if ((name.find("r.e") != string::npos || name.find("r/s") != string::npos)) //name.find("r.e") != string::npos
				{
					Gamer[objectcounter].name = name.substr(35, 10);
					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
					Gamer[objectcounter].layer = 13;
					Gamer[objectcounter].distance = 3000;
					objectcounter++;
					continue;
				}
				continue;
			}
			if (layer == 26 && keycard)
			{
				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
				string name = Driver.ReadNameS(NAMEPTR, 150);
				Gamer[objectcounter].name = name;
				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
				Gamer[objectcounter].layer = 26;
				Gamer[objectcounter].distance = 50;
				objectcounter++;
				continue;
			}
		}
		}
		catch (int e)
		{
			MessageBox(NULL, "1", "3", MB_OK);
		}
		playercount = objectcounter;
		//myfile.close();
		//Driver.Close();
		Sleep(sleep);
	}
}

//void BN()
//{
//	
//	//ofstream myfile("log.txt");
//	while (healthDecAddress)
//	{
//		GE();
//		int objectcounter = 0;
//		DWORD64 buffer = Driver.ReadVirtualMemory<DWORD64>(healthDecAddress + 0x10, sizeof(DWORD64));
//		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer, sizeof(DWORD64));
//		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x10, sizeof(DWORD64));
//		buffer = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x28, sizeof(DWORD64));
//		int size = Driver.ReadVirtualMemory<int>(buffer + 0x18, sizeof(DWORD64));
//		DWORD64 ObjectList = Driver.ReadVirtualMemory<DWORD64>(buffer + 0x10, sizeof(DWORD64));
//
//		for (int i = 0; i < size; i++)
//		{
//			DWORD64 object = Driver.ReadVirtualMemory<DWORD64>(ObjectList + 0x20 + ( 0x8  * i), sizeof(DWORD64));
//			object = Driver.ReadVirtualMemory<DWORD64>(object + 0x10, sizeof(DWORD64));
//			object = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//			int layer = Driver.ReadVirtualMemory<int>(object + 0x50, sizeof(int));
//
//			//DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//			//string name = Driver.ReadNameS(NAMEPTR, 150);
//
//			//myfile  << "-" << layer << "-" << name << endl;
//
//			if (layer == 30 || layer == 21 || layer == 8)
//				continue;
//
//			if (layer == 0 && crate)
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				if ((name.find("e_n") != string::npos || name.find("e_el") != string::npos || name.find("ll_s") != string::npos) && crate)
//				{
//					Gamer[objectcounter].name = name.substr(37, 9);
//					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//					
//					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
//					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
//					Gamer[objectcounter].layer = 0;
//					Gamer[objectcounter].distance = dist;
//					objectcounter++;
//					continue;
//				}
//				continue;
//			}
//			if (layer == 0 && ores)
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				if (name.find("e/o") != string::npos) //e/o r-o
//				{
//					Gamer[objectcounter].name = name.substr(47, 7);
//					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//
//					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
//					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
//					Gamer[objectcounter].layer = 0;
//					Gamer[objectcounter].distance = 200;
//					objectcounter++;
//					continue;
//				}
//				continue;
//			}
//			if (layer == 17 )
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				Gamer[objectcounter].name = name;
//				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//				//Vector3D vec = Driver.ReadNewShit<Vector3D>(Gamer[objectcounter].vector + 0xB0, sizeof(Vector3D));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
//				Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
//				if (name.find("lP") != string::npos)
//					localplayer = Gamer[objectcounter].base;
// 				Gamer[objectcounter].layer = 17;
//				Gamer[objectcounter].distance = dist;
//				objectcounter++;
//				continue;
//			}
//
//			if (layer == 11 && animal)
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				Gamer[objectcounter].name = name.substr(22, 4);
//				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
//				Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
//				Gamer[objectcounter].layer = 11;
//				Gamer[objectcounter].distance = dist;
//				objectcounter++;
//				continue;
//			}
//			if (layer == 13 && minicopter)
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				if ((name.find("r.e") != string::npos))
//				{
//					Gamer[objectcounter].name = name.substr(35, 10);
//					DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//					Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//					ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x18, sizeof(DWORD64));
//					Gamer[objectcounter].base = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x28, sizeof(DWORD64));
//					Gamer[objectcounter].layer = 13;
//					Gamer[objectcounter].distance = 3000;
//					objectcounter++;
//					continue;
//				}
//				continue;
//			}
//			if (layer == 26 && keycard)
//			{
//				DWORD64 NAMEPTR = Driver.ReadVirtualMemory<DWORD64>(object + 0x60, sizeof(DWORD64));
//				string name = Driver.ReadNameS(NAMEPTR, 150);
//				Gamer[objectcounter].name = name;
//				DWORD64 ptr = Driver.ReadVirtualMemory<DWORD64>(object + 0x30, sizeof(DWORD64));
//				ptr = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x8, sizeof(DWORD64));
//				Gamer[objectcounter].vector = Driver.ReadVirtualMemory<DWORD64>(ptr + 0x38, sizeof(DWORD64));
//				Gamer[objectcounter].layer = 26;
//				Gamer[objectcounter].distance = 50;
//				objectcounter++;
//				continue;
//			}
//		}
//		playercount = objectcounter;
//		//myfile.close();
//		//Driver.Close();
//		GetIni();
//		Sleep(sleep);
//	}
//}

D3DXVECTOR3 GBP(DWORD64 pTransform)
{
	__m128 result;

	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	DWORD64 puf = Driver.ReadNewShit<DWORD64>(pTransform + 0x38, sizeof(DWORD64));
	int index = Driver.ReadNewShit<int>(pTransform + 0x40, sizeof(int));
	DWORD64 pTransformArray = Driver.ReadNewShit<DWORD64>(puf + 0x18, sizeof(DWORD64));
	DWORD64 pTransformIndices = Driver.ReadNewShit<DWORD64>(puf + 0x20, sizeof(DWORD64));

	if (pTransformArray && pTransformIndices)
	{
		result = Driver.ReadNewShit<__m128>(pTransformArray + 0x30 * index, sizeof(__m128));
		int transformIndex = Driver.ReadNewShit<int>(pTransformIndices + 0x4 * index, sizeof(int));


		int pSafe = 0;
		while (transformIndex >= 0 && pSafe++ < 200)
		{
			Matrix34 matrix34 = Driver.ReadNewShit<Matrix34>(pTransformArray + 0x30 * transformIndex, sizeof(Matrix34));

			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix34.vec0));

			transformIndex = Driver.ReadNewShit<int>(pTransformIndices + +0x4 * transformIndex, sizeof(Matrix34));
		}
	}
	return D3DXVECTOR3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

DWORD64 GEB(DWORD64 entity, int bone_index)
{

	DWORD64 multi_mesh = Driver.ReadNewShit<DWORD64>(entity + 0x118, sizeof(DWORD64));
	if (!multi_mesh) {
		return 0;
	}
	DWORD64 bone_dictionary = Driver.ReadNewShit<DWORD64>(multi_mesh + 0x48, sizeof(DWORD64));
	if (!bone_dictionary) {
		return 0;
	}

	DWORD64 entity_bone = Driver.ReadNewShit<DWORD64>(bone_dictionary + (0x20 + (bone_index * 0x8)), sizeof(DWORD64));
	if (!entity_bone) {
		return 0;
	}
	DWORD64 bone_transform = Driver.ReadNewShit<DWORD64>(entity_bone + 0x10, sizeof(DWORD64));
	if (!bone_transform) {
		return 0;
	}

	return bone_transform;
}

D3DXVECTOR2 WTS(D3DXVECTOR3 loc)
{
	D3DXVECTOR2 screen = D3DXVECTOR2(0,0);
	D3DXVECTOR3 translationVector = D3DXVECTOR3(mat._14, mat._24, mat._34);
	D3DXVECTOR3 up = D3DXVECTOR3(mat._12, mat._22, mat._32);
	D3DXVECTOR3 right = D3DXVECTOR3(mat._11, mat._21, mat._31);
	D3DXVECTOR3 forward = D3DXVECTOR3(mat._13, mat._23, mat._33);
	float w = D3DXVec3Dot(&translationVector, &loc) + mat._44;
	if (w < 0.098f)return screen;
	float y = D3DXVec3Dot(&up, &loc) + mat._42;
	float x = D3DXVec3Dot(&right, &loc) + mat._41;
	screen.x = (screenx / 2) * (1.f + x / w);
	screen.y = (screeny / 2) * (1.f - y / w);
	return screen;
}

D3DXVECTOR2 SETA(D3DXVECTOR3* LocalPos, D3DXVECTOR3* EnemyPos)
{
	D3DXVECTOR2 toWrite;
	D3DXVECTOR3 relative;
	D3DXVec3Subtract(&relative, LocalPos, EnemyPos);
	float magnitude = D3DXVec3Length(&relative);
	float pitch = asin(relative[1] / magnitude);
	float yaw = -atan2(relative[0], -relative[2]);

	yaw = D3DXToDegree(yaw);
	pitch = D3DXToDegree(pitch);

	toWrite.x = pitch;
	toWrite.y = yaw;
	return toWrite;
}


wstring WN(DWORD64 inventory, int currentitem)
{
	DWORD64 belt = Driver.ReadNewShit<DWORD64>(inventory + 0x28, sizeof(DWORD64));
	belt = Driver.ReadNewShit<DWORD64>(belt + 0x38, sizeof(DWORD64));
	belt = Driver.ReadNewShit<DWORD64>(belt + 0x10, sizeof(DWORD64));

	for (int m = 0; m < 6; m++)
	{
		DWORD64 item = Driver.ReadNewShit<DWORD64>(belt + 0x20 + m * 0x8, sizeof(DWORD64));
		if (currentitem == Driver.ReadNewShit<int>(item + 0x28, sizeof(int)))
		{
			DWORD64 itemname = Driver.ReadNewShit<DWORD64>(item + 0x20, sizeof(DWORD64));
			itemname = Driver.ReadNewShit<DWORD64>(itemname + 0x20, sizeof(DWORD64));
			__int32 uzunluk = Driver.ReadVirtualMemory<__int32>(itemname + 0x10, sizeof(__int32));
			std::wstring name = Driver.ReadName(itemname + 0x14, uzunluk);
			return name;
		}

	}
	return L"";
}

void SG(DWORD64 inventory, int currentitem)
{
	DWORD64 belt = Driver.ReadNewShit<DWORD64>(inventory + 0x28, sizeof(DWORD64));
	belt = Driver.ReadNewShit<DWORD64>(belt + 0x38, sizeof(DWORD64));
	belt = Driver.ReadNewShit<DWORD64>(belt + 0x10, sizeof(DWORD64));

	for (int m = 0; m < 6; m++)
	{
		DWORD64 item = Driver.ReadNewShit<DWORD64>(belt + 0x20 + m * 0x8, sizeof(DWORD64));
		if (currentitem == Driver.ReadNewShit<int>(item + 0x28, sizeof(int)))
		{
			DWORD64 itembase = Driver.ReadNewShit<DWORD64>(item + 0x98, sizeof(DWORD64));
			DWORD64 baseproj = Driver.ReadNewShit<DWORD64>(itembase + 0x2c0, sizeof(DWORD64));
			Driver.WriteVirtualMemoryFloat(baseproj + 0x18, 0, sizeof(float)); //2A0
			Driver.WriteVirtualMemoryFloat(baseproj + 0x1c, 0, sizeof(float)); //2A0
			Driver.WriteVirtualMemoryFloat(baseproj + 0x20, 0, sizeof(float)); //2A0
			Driver.WriteVirtualMemoryFloat(baseproj + 0x24, 0, sizeof(float)); //2A0

		}

	}
}
