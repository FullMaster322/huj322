#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
#include "color.h"

using namespace hazedumper;
using namespace netvars;
using namespace signatures;
using namespace Colors;
using namespace std::literals::chrono_literals;

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;
LPDIRECT3DDEVICE9 pDevice = nullptr;

WNDPROC oWndProc;
static HWND window = NULL;

int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);

uintptr_t BaseAddress;
uintptr_t baseEngine;
uintptr_t LocalPlayer;
uintptr_t Entity;
uintptr_t Team;
int* iShotsFired;
Vec3* aimRecoilPunch;
Vec3 oPunch{ 0,0,0 };
Vec3* viewAngles;

bool WorldTooScreen(Vec3 pos, Vec2& screen, float matrix[16], int width, int height) {
    Vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f) {
        return false;
    }

    Vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
    screen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

    return true;
}

Vec2 GetBonePosition(uintptr_t Entity, uintptr_t Team ,int bone) {
    uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + Team + m_dwBoneMatrix);
    BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * bone);
    Vec3 Location = { Bone.x, Bone.y, Bone.z };
    Vec2 ScreenCoords;
    float vMatrix[16];
    memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
    if (WorldTooScreen(Location, ScreenCoords, vMatrix, screenX, screenY)) {

        return ScreenCoords;
    }

    return { 0, 0 };
}

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
    ImGui::CreateContext();
    ImGui::myStyleColor();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(xorstr("C://Windows//Fonts//Arial.ttf"), 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
}

// �������
bool espbox = false;
bool cornerbox = false;
bool fillbox = false;
bool aimbot = false;
bool antiflash = false;
bool radarhack = false;
bool awpcrosshair = false;
bool fovchanger = false;
bool rcs = false;
bool bunnyhop = false;
bool triggerbot = false;
bool hueta = false;

// ��������
int aimbotBone = 9;
float aimbotRCS = 1.f;
float aimbotSmoothing = 1.f;
float rcs_amount = 1.f;
int fov = 90;
int aimfov = 100;
int awpcrosshairWiHe = 1;

// ������� ���������
float boxwidth = 0.5f;
int boxThickness = 2;
float popcorn = 0.5f;

bool init = false;
bool menu = true;
bool antialias_all = true;

Vec3 CalcAngle(Vec3 src, Vec3 dst) {
    float PI = 3.1415926535897932384626433832795f;

    Vec3 angle;
    Vec3 a = { dst.x - src.x, dst.y - src.y, dst.z - src.z };
    float ah = sqrt(a.x * a.x + a.y * a.y);
    angle.x = atan2(-a.z, ah) * 180 / PI;
    angle.y = atan2(a.y, a.x) * 180 / PI;
    angle.z = 0;

    return angle;
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!pDevice) {
        pDevice = o_pDevice;
    }

    if (!init)
    {
        BaseAddress = (DWORD)GetModuleHandle(xorstr("client.dll"));
        baseEngine = (DWORD)GetModuleHandle(xorstr("engine.dll"));
        InitImGui(pDevice);
        init = true;
    }

    LocalPlayer = *(uintptr_t*)(BaseAddress + dwLocalPlayer);
    if (LocalPlayer != NULL) {

        for (int x = 1; x < 32; x++) {
            Entity = *(uintptr_t*)(BaseAddress + dwEntityList + x * 0x10);
            Team = *(uintptr_t*)(BaseAddress + dwEntityList + x * 0x10);
            if (Entity != NULL) {

               if (espbox) {


                    int localTeam = *(int*)(LocalPlayer + m_iTeamNum);

                    Vec2 ScreenPosition;
                    Vec2 HeadPosition;

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityHealth = *(int*)(Entity + m_iHealth);
                    int teamHealth = *(int*)(Team + m_iHealth);

                    
                    if (localTeam != entityTeam && entityHealth > 0) {

                        Vec3 EntityLocation = *(Vec3*)(Entity + m_vecOrigin);
                        float vMatrix[16];
                        memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                        if (WorldTooScreen(EntityLocation, ScreenPosition, vMatrix, screenX, screenY)) {

                            uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                            BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * 9);
                            Vec3 Location = { Bone.x, Bone.y, Bone.z + 10 };
                            Vec2 ScreenCoords;
                            float vMatrix[16];
                            memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                            if (WorldTooScreen(Location, ScreenCoords, vMatrix, screenX, screenY))
                            {

                                HeadPosition = ScreenCoords;
                                DrawBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                    ScreenPosition.y - HeadPosition.y,
                                    boxThickness, antialias_all, FLOAT4TOD3DCOLOR(Colors::boxColor)
                                );
                               
                            }
                           
                        }
                    }
                }

              

                if (cornerbox) {

                    int localTeam = *(int*)(LocalPlayer + m_iTeamNum);

                    Vec2 ScreenPosition;
                    Vec2 HeadPosition;

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityHealth = *(int*)(Entity + m_iHealth);
                    

                    
                    if (localTeam != entityTeam && entityHealth > 0) {

                        Vec3 EntityLocation = *(Vec3*)(Entity + m_vecOrigin);
                        float vMatrix[16];
                        memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                        if (WorldTooScreen(EntityLocation, ScreenPosition, vMatrix, screenX, screenY)) {

                            uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                            BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * 9);
                            Vec3 Location = { Bone.x, Bone.y, Bone.z + 10 };
                            Vec2 ScreenCoords;
                            float vMatrix[16];
                            memcpy(&vMatrix, (PBYTE*)(BaseAddress + dwViewMatrix), sizeof(vMatrix));
                            if (WorldTooScreen(Location, ScreenCoords, vMatrix, screenX, screenY))
                            {

                                HeadPosition = ScreenCoords;
                                CornerBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                    ScreenPosition.y - HeadPosition.y,
                                    boxThickness, popcorn / 2, antialias_all, FLOAT4TOD3DCOLOR(Colors::cornerColor)
                                );
                            }
                        }
                    }
                }

                if (bunnyhop) {

                    if (LocalPlayer == NULL) continue;

                    int flag = *(int*)(LocalPlayer + m_fFlags);

                    if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0))
                    {
                       
                        *(int*)(BaseAddress + dwForceJump) = 6;
                        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                    }
                    
                }


                if (antiflash) {

                    int flashDur = 0;
                    flashDur = *(int*)(LocalPlayer + m_flFlashDuration);
                    if (flashDur > 0) {
                        *(int*)(LocalPlayer + m_flFlashDuration) = 0;
                    }
                }
               

                if (radarhack) {

                    int localTeam = *(int*)(LocalPlayer + m_iTeamNum);

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityDormant = *(int*)(Entity + m_bDormant);

                    if (entityTeam != localTeam && !entityDormant) {

                        *(int*)(Entity + m_bSpotted) = 1;
                    }
                }

                if (awpcrosshair) {
                    DrawFilledRect(screenX / 2 - (awpcrosshairWiHe / 2), screenY / 2 - (awpcrosshairWiHe / 2), awpcrosshairWiHe, awpcrosshairWiHe, FLOAT4TOD3DCOLOR(Colors::crosshairColor));
                }

               

                if (fovchanger) {

                    *(int*)(LocalPlayer + m_iDefaultFOV) = fov;

                }

                if (rcs) {

                    iShotsFired = (int*)(LocalPlayer + m_iShotsFired);
                    aimRecoilPunch = (Vec3*)(LocalPlayer + m_aimPunchAngle);
                    viewAngles = (Vec3*)(*(uintptr_t*)(baseEngine + dwClientState) + dwClientState_ViewAngles);

                    Vec3 punchAngle = *aimRecoilPunch * (rcs_amount * 2);
                    if (*iShotsFired > 1 && GetAsyncKeyState(VK_LBUTTON)) {
                        Vec3 newAngle = *viewAngles + oPunch - punchAngle;
                        newAngle.normalize();

                        *viewAngles = newAngle;
                    }
                    oPunch = punchAngle;
                }

                if (triggerbot) {

                    int CrosshairId = *(int*)(LocalPlayer + m_iCrosshairId);
                    int localTeam = *(int*)(LocalPlayer + m_iTeamNum);
                    int attak = *(int*)(LocalPlayer + dwForceAttack);

                    if (CrosshairId != 0 && CrosshairId < 32)
                    {
                        int Enemy = *(int*)(BaseAddress + dwEntityList + (CrosshairId - 1) * 0x10);
                        if (Enemy == NULL) continue;

                        int eHealth = *(int*)(Enemy + m_iHealth);
                        int EnemyTeam = *(int*)(Enemy + m_iTeamNum);

                        if (EnemyTeam != localTeam && eHealth > 0)
                        {
                            std::this_thread::sleep_for(std::chrono::microseconds(1));
                            *(int*)(BaseAddress + dwForceAttack) = 6;
                            
                        }
                       
                    }
                    
                }
               
            }
        }
      
    }
    
         

    // Unhook
    if (GetAsyncKeyState(VK_END)) {

        kiero::shutdown();
    }

    // Open Menu
    if (GetAsyncKeyState(VK_HOME) & 1) {
        menu = !menu;
    }

    if (menu) {

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        auto flags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        static int ttab;

        ImGui::Begin(xorstr("DarwinTap"), nullptr, flags);

        if (ImGui::Button(xorstr(u8"aim"), ImVec2(100.f, 0.f))) {
            ttab = 1;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"wallhack"), ImVec2(100.f, 0.f))) {
            ttab = 2;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"other"), ImVec2(100.f, 0.f))) {
            ttab = 3;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"colors"), ImVec2(100.f, 0.f))) {
            ttab = 4;
        }

        if (ttab == 1) {
            ImGui::Checkbox(xorstr(u8"�������� ������"), &rcs);
            ImGui::Checkbox(xorstr(u8"������� ���"), &triggerbot);
            ImGui::Checkbox(xorstr(u8"������"), &awpcrosshair);
            if (awpcrosshair)
            {
                ImGui::SliderInt(xorstr(u8"������ �������"), &awpcrosshairWiHe, 1, 10);
            }
        }
        else if (ttab == 2) {
            ImGui::Checkbox(xorstr(u8"�������"), &espbox);
            ImGui::Checkbox(xorstr(u8"������� �������"), &cornerbox);
            ImGui::Checkbox(xorstr(u8"����� �� ������"), &radarhack);
        }
        else if (ttab == 3) {
            ImGui::Checkbox(xorstr(u8"���� ������"), &antiflash);
            ImGui::Checkbox(xorstr(u8"��������"), &bunnyhop);
            ImGui::Checkbox(xorstr(u8"�����"), &hueta);
            ImGui::Checkbox(xorstr(u8"�������� FOV"), &fovchanger);
            if (fovchanger) {
                ImGui::SliderInt(xorstr(u8"�������� FOV"), &fov, -200, 200);
            }
            if (ImGui::Button(xorstr(u8"������� ��� �������"))) {
                espbox = true;
                bunnyhop = true;
                radarhack = true;
                antiflash = true;
                rcs = true;
                cornerbox = true;
                awpcrosshair = true;
            }
        }
        else if (ttab == 4) {
            ImGui::ColorEdit3(xorstr(u8"���� �������"), Colors::boxColor);
            ImGui::ColorEdit3(xorstr(u8"���.�������"), Colors::cornerColor);
            ImGui::ColorEdit3(xorstr(u8"���� �������"), Colors::glowColor);
            ImGui::ColorEdit3(xorstr(u8"���� �������"), Colors::crosshairColor);
        }

        ImGui::End();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    DWORD wndProcId;
    GetWindowThreadProcessId(handle, &wndProcId);

    if (GetCurrentProcessId() != wndProcId)
        return TRUE; 

    window = handle;
    return FALSE; 
}

HWND GetProcessWindow()
{
    window = NULL;
    EnumWindows(EnumWindowsCallback, NULL);
    return window;
}


int mainThread()
{
    if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {
        
            kiero::bind(42, (void**)&oEndScene, hkEndScene);
            window = GetProcessWindow();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
        }
    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{

    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainThread, NULL, 0, NULL));
    }
    return TRUE;
}