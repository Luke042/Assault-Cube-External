#include "gui.h"
#include "settings.h"
#include <thread>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

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
    return baseAddress != 0;
}

void CheatThread() {
	SetWindowPos(gui::window, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    DWORD fov = baseAddress + 0x18A7CC;
    float fov_orig;
    ReadProcessMemory(hProcess, (LPVOID)fov, &fov_orig, sizeof(fov_orig), NULL);
    while (gui::isRunning) {
        //byte sizes
        BYTE nop[1] = { 0x90 };
        BYTE nop2[2] = { 0x90, 0x90 };
        BYTE nop3[3] = { 0x90, 0x90, 0x90 };
        BYTE nop4[4] = { 0x90, 0x90, 0x90, 0x90 };
        BYTE nop5[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        BYTE nop6[6] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

        //infinite jump exploit 
        DWORD inf_jump_address = 0xC140E;
        if (exploits::inf_jump == true) {
            WriteProcessMemory(hProcess, (LPVOID)inf_jump_address, &nop3, sizeof(nop3), NULL);
        }
        if (exploits::inf_jump == false) {
            BYTE inf_jump_orig[3] = { 0x88, 0x45, 0x5D };
            WriteProcessMemory(hProcess, (LPVOID)inf_jump_address, &inf_jump_orig, sizeof(inf_jump_orig), NULL);
        }

        //fast fire exploit
        DWORD fast_shoot = baseAddress + 0xC73EA;
        
        if (exploits::fast_shoot == true) {
            
            WriteProcessMemory(hProcess, (LPVOID)fast_shoot, &nop2, sizeof(nop2), NULL);
        }
        if (exploits::fast_shoot == false) {
            BYTE fs_orig[2] = { 0x89, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)fast_shoot, &fs_orig, sizeof(fs_orig), NULL);
        }

        //static ammo
        DWORD static_ammo = baseAddress + 0xC73EF;
        if (exploits::static_ammo == true) {
            WriteProcessMemory(hProcess, (LPVOID)static_ammo, &nop2, sizeof(nop2), NULL);
        }
        if (exploits::static_ammo == false) {
            BYTE sa_orig[2] = { 0xFE, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)static_ammo, &sa_orig, sizeof(sa_orig), NULL);
        }

        //BROKEN/FIX LATER static health
        DWORD static_health = baseAddress + 0x1C223;
        if (exploits::static_health == true) {
            WriteProcessMemory(hProcess, (LPVOID)static_health, nop6, sizeof(nop6), NULL);
        }
        if (exploits::static_health == false) {
            BYTE health_orig[3] = { 0x29, 0x73, 0x04 };
            WriteProcessMemory(hProcess, (LPVOID)static_health, &health_orig, sizeof(health_orig), NULL);
        }

        //anti recoil
        DWORD anti_recoil = baseAddress + 0xC2EC3;
        if (exploits::anti_recoil == true) {
            BYTE recoil_value = NULL;
            WriteProcessMemory(hProcess, (LPVOID)anti_recoil, &recoil_value, sizeof(recoil_value), NULL);
        }
        if (exploits::anti_recoil == false) {
            BYTE recoil_orig = 0xF3;
            WriteProcessMemory(hProcess, (LPVOID)anti_recoil, &recoil_orig, sizeof(recoil_orig), NULL);
        }

        //FOV
        if (exploits::fov == true) {
            WriteProcessMemory(hProcess, (LPVOID)fov, &values::fov_value, sizeof(values::fov_value), NULL);
        }
        if (exploits::fov == false) {
            WriteProcessMemory(hProcess, (LPVOID)fov, &fov_orig, sizeof(fov_orig), NULL);
        }
        
        //Fast grenades
        DWORD fast_grenade = baseAddress + 0xC7E69;
        if (exploits::fast_grenade == true) {
            WriteProcessMemory(hProcess, (LPVOID)fast_grenade, &nop2, sizeof(nop2), NULL);
        }
        if (exploits::fast_grenade == false) {
            BYTE fg_orig[2] = { 0x89, 0x08 };
            WriteProcessMemory(hProcess, (LPVOID)fast_grenade, &fg_orig, sizeof(fg_orig), NULL);
        }

        //fly exploit
		DWORD local_player = baseAddress + 0x0017E0A8;
        uintptr_t lp = 0;
        DWORD lp_z_adrs = lp + 0x30;
		
        if (exploits::fly == true) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);
            
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                
                WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::fly_height, sizeof(values::fly_height), NULL);
            }
            else if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                float down = 2.0f;
                WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &down, sizeof(down), NULL);
            }
        }

        //way point cheat
        DWORD lp_x_adrs = lp + 0x28;
        DWORD lp_y_adrs = lp + 0x2C;
        if (values::save_position == true) {
            ReadProcessMemory(hProcess, (LPVOID)local_player, &lp, sizeof(lp), NULL);

            //read our players current coordinates
            ReadProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ, sizeof(values::wp_posZ), NULL);
            
            ReadProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX, sizeof(values::wp_posX), NULL);
            
            ReadProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY, sizeof(values::wp_posY), NULL);
        }

        if (values::load_position == true) {
            WriteProcessMemory(hProcess, (LPVOID)lp_z_adrs, &values::wp_posZ, sizeof(values::wp_posZ), NULL);
            WriteProcessMemory(hProcess, (LPVOID)lp_x_adrs, &values::wp_posX, sizeof(values::wp_posX), NULL);
            WriteProcessMemory(hProcess, (LPVOID)lp_y_adrs, &values::wp_posY, sizeof(values::wp_posY), NULL);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    if (!AttachToGame()) {
        MessageBoxA(NULL, "Failed to attach to AssaultCube!", "Error", MB_ICONERROR);
        return 1;
    }

    std::thread cheatThread(CheatThread);

    gui::CreateHWindow("Assault Cube External");
    gui::CreateDevice();
    gui::CreateImGui();

    while (gui::isRunning) {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    cheatThread.join();
    CloseHandle(hProcess);

    return 0;
}
