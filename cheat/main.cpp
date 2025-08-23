#include "gui.h"
#include "settings.h"
#include "offsets.h"
#include <thread>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>


DWORD pid;
HANDLE hProcess;
DWORD baseAddress;
DWORD GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName) {
    DWORD baseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W moduleEntry;
        moduleEntry.dwSize = sizeof(moduleEntry);
        if (Module32FirstW(hSnapshot, &moduleEntry)) {
            do {
                if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
                    baseAddress = (DWORD)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnapshot, &moduleEntry));
        }
        CloseHandle(hSnapshot);
    }
    return baseAddress;
}

bool AttachToGame() {
    HWND hWnd = FindWindowA(NULL, "AssaultCube");
    if (!hWnd)
        return false;

    if (!GetWindowThreadProcessId(hWnd, &pid))
        return false;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
        return false;

    baseAddress = GetModuleBaseAddress(pid, L"ac_client.exe");
	std::cout << baseAddress;
	return baseAddress != 0;
}
uintptr_t lp = 0;

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };

bool WorldToScreen(const Vec3& pos, Vec2& screen, float matrix[16], int windowWidth, int windowHeight) {
    float clipX = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    float clipY = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    float clipZ = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    float clipW = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipW < 0.1f)
        return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (windowWidth / 2.0f) * (ndcX + 1.0f);
    screen.y = (windowHeight / 2.0f) * (1.0f - ndcY);

    return true;
}

void save_pos(uintptr_t lp) {
	DWORD lp_x_adrs = lp + 0x28;
	DWORD lp_y_adrs = lp + 0x2C;
	DWORD lp_z_adrs = lp + 0x30;
	// read our player's current coordinates
	ReadProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ, sizeof(values::wp_posZ), NULL);
	ReadProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX, sizeof(values::wp_posX), NULL);
	ReadProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY, sizeof(values::wp_posY), NULL);
	values::save_position = false;
}
void load_pos(uintptr_t lp) {
	DWORD lp_x_adrs = lp + 0x28;
	DWORD lp_y_adrs = lp + 0x2C;
	DWORD lp_z_adrs = lp + 0x30;
	WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ, sizeof(values::wp_posZ), NULL);
	WriteProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX, sizeof(values::wp_posX), NULL);
	WriteProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY, sizeof(values::wp_posY), NULL);
	values::load_position = false;
}
void save_pos2(uintptr_t lp) {
	DWORD lp_x_adrs = lp + 0x28;
	DWORD lp_y_adrs = lp + 0x2C;
	DWORD lp_z_adrs = lp + 0x30;
	// read our player's current coordinates
	ReadProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ2, sizeof(values::wp_posZ2), NULL);
	ReadProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX2, sizeof(values::wp_posX2), NULL);
	ReadProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY2, sizeof(values::wp_posY2), NULL);
	values::save_position2 = false;
}
void load_pos2(uintptr_t lp) {
	DWORD lp_x_adrs = lp + 0x28;
	DWORD lp_y_adrs = lp + 0x2C;
	DWORD lp_z_adrs = lp + 0x30;
	WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ2, sizeof(values::wp_posZ2), NULL);
	WriteProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX2, sizeof(values::wp_posX2), NULL);
	WriteProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY2, sizeof(values::wp_posY2), NULL);
	values::load_position2 = false;
}

void Click() {
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
	SendInput(1, &input, sizeof(INPUT));
    
}

void CheatThread() {
    SetWindowPos(gui::window, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    //read current fov
    DWORD fov = baseAddress + 0x18A7CC;
    ReadProcessMemory(hProcess, (LPVOID)fov, &values::fov_orig, sizeof(values::fov_orig), NULL);
    values::fov_value = values::fov_orig;
    while (gui::isRunning) {
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            values::show_menu = !values::show_menu;
        }
        uintptr_t local_player = baseAddress + 0x0017E0A8;
        //byte sizes
        BYTE nop[1] = { 0x90 };
        BYTE nop2[2] = { 0x90, 0x90 };
        BYTE nop3[3] = { 0x90, 0x90, 0x90 };
        BYTE nop4[4] = { 0x90, 0x90, 0x90, 0x90 };
        BYTE nop5[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        BYTE nop6[6] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

        //infinite jump exploit 
        DWORD inf_jump_address = baseAddress + 0xC140E;
        if (exploits::inf_jump) {
            WriteProcessMemory(hProcess, (LPVOID)inf_jump_address, &nop3, sizeof(nop3), NULL);
        }
        else if (!exploits::inf_jump) {
            BYTE inf_jump_orig[3] = { 0x88, 0x45, 0x5D };
            WriteProcessMemory(hProcess, (LPVOID)inf_jump_address, &inf_jump_orig, sizeof(inf_jump_orig), NULL);
        }

        //fast fire exploit
        if (exploits::fast_shoot) {
            WriteProcessMemory(hProcess, (LPVOID)(baseAddress + 0xC73EA), &nop2, sizeof(nop2), NULL);
        }
        if (!exploits::fast_shoot) {
            BYTE fs_orig[2] = { 0x89, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)(baseAddress + 0xC73EA), &fs_orig, sizeof(fs_orig), NULL);
        }

        //static ammo
        DWORD static_ammo = baseAddress + 0xC73EF;
        DWORD static_grenade = baseAddress + 0xC7E5D;
        if (exploits::static_ammo) {
            WriteProcessMemory(hProcess, (LPVOID)static_ammo, &nop2, sizeof(nop2), NULL);
            WriteProcessMemory(hProcess, (LPVOID)static_grenade, &nop2, sizeof(nop2), NULL);
        }
        if (!exploits::static_ammo) {
            BYTE sa_orig[2] = { 0xFE, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)static_ammo, &sa_orig, sizeof(sa_orig), NULL);
            WriteProcessMemory(hProcess, (LPVOID)static_grenade, &sa_orig, sizeof(sa_orig), NULL);
        }

        //static health
        DWORD static_health = baseAddress + 0x1C223;
        if (exploits::static_health) {
            WriteProcessMemory(hProcess, (LPVOID)static_health, nop3, sizeof(nop3), NULL);
        }
        if (!exploits::static_health) {
            BYTE health_orig[3] = { 0x29, 0x73, 0x04 };
            WriteProcessMemory(hProcess, (LPVOID)static_health, &health_orig, sizeof(health_orig), NULL);
        }

        //anti recoil
        DWORD anti_recoil = baseAddress + 0xC2EC3;
        if (exploits::anti_recoil) {
            BYTE recoil_value = NULL;
            WriteProcessMemory(hProcess, (LPVOID)anti_recoil, &recoil_value, sizeof(recoil_value), NULL);
        }
        if (!exploits::anti_recoil) {
            BYTE recoil_orig = 0xF3;
            WriteProcessMemory(hProcess, (LPVOID)anti_recoil, &recoil_orig, sizeof(recoil_orig), NULL);
        }

        //FOV
        if (exploits::fov) {
            WriteProcessMemory(hProcess, (LPVOID)fov, &values::fov_value, sizeof(values::fov_value), NULL);
        }
        if (exploits::fov == false) {
            WriteProcessMemory(hProcess, (LPVOID)fov, &values::fov_orig, sizeof(values::fov_orig), NULL);
        }

        //Fast grenades
        DWORD fast_grenade = baseAddress + 0xC7E69;
        if (exploits::fast_grenade) {
            WriteProcessMemory(hProcess, (LPVOID)fast_grenade, &nop2, sizeof(nop2), NULL);
        }
        if (!exploits::fast_grenade) {
            BYTE fg_orig[2] = { 0x89, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)fast_grenade, &fg_orig, sizeof(fg_orig), NULL);
        }

        //fly exploit
        uintptr_t lp = 0;
        if (exploits::fly) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            DWORD lp_z_adrs = lp + 0x30;
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::fly_height, sizeof(values::fly_height), NULL);
            }
            else if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                float down = 2.0f;
                WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &down, sizeof(down), NULL);
            }
        }

        //way point
        if (values::save_position) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            save_pos(lp);
        }
        else if (values::load_position) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            load_pos(lp);
        }
        if (values::save_position2) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            save_pos2(lp);
        }
        else if (values::load_position2) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            load_pos2(lp);
        }

            //perfect aim
            uintptr_t local_player_addr = baseAddress + 0x0017E0A8;
            //read the local player
            ReadProcessMemory(hProcess, (LPVOID)local_player_addr, &local_player, sizeof(local_player), NULL);

            uintptr_t entity_list_addr = baseAddress + 0x18AC04;
            uintptr_t entity_base;
            // Read pointer to the entity list
            ReadProcessMemory(hProcess, (LPCVOID)entity_list_addr, &entity_base, sizeof(entity_base), NULL);

            int player_count = 0;
            ReadProcessMemory(hProcess, (LPCVOID)(baseAddress + 0x18AC0C), &player_count, sizeof(player_count), NULL);

            //read your players pitch/yaw
            float lp_pitch, lp_yaw;
            ReadProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &lp_pitch, sizeof(lp_pitch), NULL);
            ReadProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &lp_yaw, sizeof(lp_yaw), NULL);

            int local_player_team;
            ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x30C), &local_player_team, sizeof(local_player_team), NULL);

            uintptr_t best_target = 0;
            float best_pitch = 0.f, best_yaw = 0.f;
            float best_angle_diff = FLT_MAX;


            //loop throgh the entiy list
            for (int i = 1; i < player_count; i++) {
                    uintptr_t entity_ptr = entity_base + i * 0x4;
                    uintptr_t player_addr = 0;
                    ReadProcessMemory(hProcess, (LPCVOID)entity_ptr, &player_addr, sizeof(player_addr), NULL);
                    if (player_addr == 0 || player_addr == local_player)
                        continue;

                    int health, team;
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0xEC), &health, sizeof(health), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x30C), &team, sizeof(team), NULL);
                    float visable;
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x3F8), &visable, sizeof(visable), NULL);

                    // aim checks
                    if (health <= 0 || health >= 101)
                        continue;
                    if (aim::aimbot_team_check && team == local_player_team )
                        continue;
                    if (aim::silent_team_check && team == local_player_team)
                        continue;
                    if (aim::bezier_team_check && team == local_player_team)
                        continue;
                    
                    if (aim::aimbot_visable_check && visable <= 1)
                        continue;
                    if (aim::silent_aim && visable == -1)
                        continue;
                    if (aim::bezier_visable_check&& visable <= 1)
                        continue;

                    if (aim::triggerbot) {

                    if (aim::trigger_visable_check && visable <= 1)
                        continue;
                    if (aim::trigger_team_check && team == local_player_team)
                        continue;

                    }          
                    //read the view matrix
                    float viewMatrix[16];
                    ReadProcessMemory(hProcess, (LPCVOID)(baseAddress + 0x17DFD0), &viewMatrix, sizeof(viewMatrix), NULL);

                    //read enemy position
                    float enemy_posX, enemy_posY, enemy_posZ;
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x28), &enemy_posX, sizeof(enemy_posX), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x2C), &enemy_posY, sizeof(enemy_posY), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x30), &enemy_posZ, sizeof(enemy_posZ), NULL);

                    //read your position
                    float lp_posX, lp_posY, lp_posZ;
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x28), &lp_posX, sizeof(lp_posX), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x2C), &lp_posY, sizeof(lp_posY), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x30), &lp_posZ, sizeof(lp_posZ), NULL);

                    //read enemy head position
                    float ex, ey, ez;
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x8), &ex, sizeof(ex), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0x4), &ey, sizeof(ey), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(player_addr + 0xC), &ez, sizeof(ez), NULL);

                    //read your head position
                    float lx, ly, lz;
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x8), &lx, sizeof(lx), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0x4), &ly, sizeof(ly), NULL);
                    ReadProcessMemory(hProcess, (LPCVOID)(local_player + 0xC), &lz, sizeof(lz), NULL);

                    //calculate pitch/yaw needed to aim at enemy
                    float dx = ex - lx;
                    float dy = ey - ly;
                    float pi = 3.14159265358979323846264338327f;
                    float yaw = atan2f(dx, dy) * (180.f / pi);
                    float hyp = sqrtf(dx * dx + dy * dy);
                    float dz = ez - lz;                  float pitch = atan2f(dz, hyp) * (180.f / pi);

                    if (yaw < 0) yaw += 360.f;
                    if (yaw >= 360.f) yaw -= 360.f;

                    //calculate angle difference from current angle
                    float d_pitch = pitch - lp_pitch;
                    float d_yaw = yaw - lp_yaw;

                    if (d_yaw > 180.f) d_yaw -= 360.f;
                    if (d_yaw < -180.f) d_yaw += 360.f;

                    float angle_diff = sqrtf(d_pitch * d_pitch + d_yaw * d_yaw);

                    //keep track of closest enemy
                    if (angle_diff < best_angle_diff) {
                        best_angle_diff = angle_diff;
                        best_pitch = pitch;
                        best_yaw = yaw + 90;
                        best_target = player_addr;
                        values::best_pitch = best_pitch;
                        values::best_yaw = best_yaw;
                    }
                    
                    //emergency exit
                    if (GetAsyncKeyState(0x77)) {
                        exit(0);
                    }
                    //aimbot
                    if (best_target != 0 && aim::aimbot_toggle && GetAsyncKeyState(aim::aim_key) & 0x8000) {
                        //calculate smoothing
                        float smoothning = aim::aimbot_smoothening * 1000;
                        float delta_yaw = best_yaw - lp_yaw;
                        float delta_pitch = best_pitch - lp_pitch;
                        lp_pitch += delta_pitch / smoothning;
                        lp_yaw += delta_yaw / smoothning;

                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &lp_pitch, sizeof(lp_pitch), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &lp_yaw, sizeof(lp_yaw), NULL);
                    }

                    //bezier curve aimbot
                    if (best_target != 0 && aim::bezier_aim && GetAsyncKeyState(aim::aim_key) & 0x8000) {
                        float P0_yaw = lp_yaw;
                        float P0_pitch = lp_pitch;

                        float P3_yaw = best_yaw;
                        float P3_pitch = best_pitch;

                        float midYaw = (P0_yaw + P3_yaw) / 2.0f;
                        float midPitch = (P0_pitch + P3_pitch) / 2.0f;

                        float P1_yaw = midYaw + ((rand() % 20 - 10) * 0.05f);
                        float P1_pitch = midPitch + ((rand() % 20 - 10) * 0.05f);

                        float P2_yaw = midYaw + ((rand() % 20 - 10) * 0.05f);
                        float P2_pitch = midPitch + ((rand() % 20 - 10) * 0.05f);

                        float t = 0.01f * values::bezier_speed;

                        auto bezier = [](float P0, float P1, float P2, float P3, float t) {
                            float u = 1.0f - t;
                            return (u * u * u) * P0 + 3 * (u * u) * t * P1 + 3 * u * (t * t) * P2 + (t * t * t) * P3;
                            };

                        float newYaw = bezier(P0_yaw, P1_yaw, P2_yaw, P3_yaw, t);
                        float newPitch = bezier(P0_pitch, P1_pitch, P2_pitch, P3_pitch, t);

                        // Write back to game
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &newPitch, sizeof(newPitch), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &newYaw, sizeof(newYaw), NULL);

                    }

                    //triggerbot
                    if (aim::triggerbot && GetAsyncKeyState(aim::trigger_key) & 0x8000
                        && lp_yaw >= best_yaw - 1.0f && lp_yaw <= best_yaw + 1.0f
                        && lp_pitch >= best_pitch - 2.0f && lp_pitch <= best_pitch + 2.0f)
                    {
                        Click();
                    }

                    //perfect aim
                    if (best_target != 0 && aim::perfect_aim && GetAsyncKeyState(aim::aim_key) & 0x8000) {
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &best_pitch, sizeof(best_pitch), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &values::best_yaw, sizeof(best_yaw), NULL);
                    }                        

                    //silent aim
                    if (best_target != 0 && aim::silent_aim && GetAsyncKeyState(aim::silent_key) & 0x8000) {
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &best_pitch, sizeof(best_pitch), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &best_yaw, sizeof(best_yaw), NULL);
                        Click();
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x38), &lp_pitch, sizeof(lp_pitch), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x34), &lp_yaw, sizeof(lp_yaw), NULL);
                        std::this_thread::sleep_for(std::chrono::milliseconds(225));
                    }

                    //kill aura
                    if (exploits::kill_aura && best_target != 0) {
                        if (team == local_player_team)
                            continue;
                        //we activate static ammo so you dont have to reload and we also activate fast shoot to remove shooting delays which makes it way way more blatant
                        WriteProcessMemory(hProcess, (LPVOID)static_ammo, &nop2, sizeof(nop2), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)static_grenade, &nop2, sizeof(nop2), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(baseAddress + 0xC73EA), &nop2, sizeof(nop2), NULL);  

                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x28), &enemy_posX, sizeof(enemy_posX), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x2C), &enemy_posY, sizeof(enemy_posY), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)(local_player + 0x30), &enemy_posZ, sizeof(enemy_posZ), NULL);
                        Click();
                    }
                    if (!exploits::kill_aura) {
                        BYTE sa_orig[2] = { 0xFE, 0x08 };
                        BYTE fs_orig[2] = { 0x89, 0x08 };
                        WriteProcessMemory(hProcess, (LPVOID)static_ammo, &sa_orig, sizeof(sa_orig), NULL);
                        WriteProcessMemory(hProcess, (LPVOID)static_grenade, &sa_orig, sizeof(sa_orig), NULL);   
                        WriteProcessMemory(hProcess, (LPVOID)(baseAddress + 0xC73EA), &fs_orig, sizeof(fs_orig), NULL);
                    }


            }
            //overwrite health / ammo
            int l337 = 1337;
            if (exploits::overwrite_ammo) {
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::rifle_ammo), &l337, sizeof(l337), NULL);
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::pistol_ammo), &l337, sizeof(l337), NULL);
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::shotgun_ammo), &l337, sizeof(l337), NULL);
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::smg_ammo), &l337, sizeof(l337), NULL);
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::sniper_ammo), &l337, sizeof(l337), NULL);
                WriteProcessMemory(hProcess, (LPVOID)(local_player + player_offsets::grenade_ammo), &l337, sizeof(l337), NULL);
            }
            if (exploits::overwrite_health) {
                WriteProcessMemory(hProcess, (LPVOID)(local_player + 0xEC), &l337, sizeof(l337), NULL);
            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    if (!AttachToGame()) {
        std::cout << "[!]Failed to attach to Assault Cube\n";
        MessageBoxA(NULL, "Failed to attach to AssaultCube", "Error", MB_ICONERROR);
        return 1;
    }
    if (AttachToGame()) { std::cout << "[+]Successfully attached to Assault Cube\n"; }
    std::thread cheatThread(CheatThread);
   
    gui::CreateHWindow("Assault Cube External");
    gui::CreateDevice();
    gui::CreateImGui();

	while (gui::isRunning) {
        gui::BeginRender();
		gui::Render();
		gui::EndRender();

        if (values::low_end) {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }

	}

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    cheatThread.join();
    CloseHandle(hProcess);
    FreeConsole();
    return 0;
}
