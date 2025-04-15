#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <ctime>
#include <mmsystem.h>
#include <WinUser.h>


void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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

int main() {

    SetConsoleTitleA("Assault Cube External - Made by lukeismadhot on discord");

    DWORD pid;
    if (!GetWindowThreadProcessId(FindWindowA(NULL, "AssaultCube"), &pid)) {
        std::cout << "Failed to find AssaultCube\n";
        Sleep(1000);
        return 1;
    }

    std::cout << "Connected to Game\nProcessID: " << pid << std::endl;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cout << "Failed to open AssaultCube\n";
        Sleep(1000);
        return 1;
    }

    DWORD baseAddress = GetModuleBaseAddress(pid, L"ac_client.exe");
    if (baseAddress == 0) {
        std::cout << "Failed to get base address of ac_client.exe\n";
        Sleep(1000);
        return 1;
    }

    std::cout << "Base Address: " << std::hex << baseAddress << std::endl;
    Sleep(620);
    system("cls");

    int SetFOVstatus;
    int StaticAmmostatus;
    std::string setName;
    int setName1 = 1;
    int StaticHealthstatus;
    int setFastShoot;
    int setAntiRecoil;
    int setInfJump;
    int fastGrenadeStatus;
    int config;
    int speedStatus;

    system("cls");
    std::cout << "Press 1 if you like to enable speed or 2 to disable it: ";
    std::cin >> speedStatus;
    system("cls");
    std::cout << "Press 1 if you like to enable fast grenades or 2 to disable it: ";
    std::cin >> fastGrenadeStatus;
    system("cls");
    std::cout << "Press 1 if you like to enable fly or 2 to disable it: ";
    std::cin >> setInfJump;
    system("cls");
    std::cout << "Press 1 if you like to enable fast shooting or 2 to disable it (strongly advised to use with static ammo): ";
    std::cin >> setFastShoot;
    system("cls");
    std::cout << "Press 1 if you like to have static health or 2 to disable it: ";
    std::cin >> StaticHealthstatus;
    system("cls");
    std::cout << "Press 1 if you like to have static ammo or 2 to disable it: ";
    std::cin >> StaticAmmostatus;
    system("cls");
    std::cout << "Press 1 if you like to have a changing name or 2 to disable it: ";
    std::cin >> setName1;
    system("cls");
    std::cout << "Enter 1 to enable FOV or 2 to disable FOV (Enabling FOV may cause rendering glitches) ";
    std::cin >> SetFOVstatus;
    system("cls");
    std::cout << "Enter 1 to enable anti-recoil or 2 to disable anti-recoil: ";
    std::cin >> setAntiRecoil;
    Sleep(300);
    system("cls");

    std::cout << "Current config is:\n";
    if (setFastShoot == 1) { SetColor(10), std::cout << "(1)", SetColor(7); std::cout << "Fast shoot is enabled\n"; }
    else { SetColor(4), std::cout << "(1)", SetColor(7); std::cout << "Fast shoot is disabled\n"; };

    if (StaticHealthstatus == 1) { SetColor(10), std::cout << "(2)", SetColor(7); std::cout << "Static health is enabled\n"; }
    else { SetColor(4), std::cout << "(2)", SetColor(7); std::cout << "Static health is disabled\n"; };

    if (StaticAmmostatus == 1) { SetColor(10), std::cout << "(3)", SetColor(7); std::cout << "Static ammo is enabled\n"; }
    else { SetColor(4), std::cout << "(3)", SetColor(7); std::cout << "Static ammo is disabled\n"; };

    if (SetFOVstatus == 1) { SetColor(10), std::cout << "(5)", SetColor(7); std::cout << "FOV is enabled\n"; }
    else { SetColor(4), std::cout << "(5)", SetColor(7); std::cout << "FOV is disabled\n"; };

    if (setAntiRecoil == 1) { SetColor(10), std::cout << "(6)", SetColor(7); std::cout << "Anti-recoil is enabled\n"; }
    else { SetColor(4), std::cout << "(6)", SetColor(7); std::cout << "Anti-recoil is disabled\n"; };

    if (setInfJump == 1) { SetColor(10), std::cout << "(7)", SetColor(7); std::cout << "Inf jump is enabled\n"; }
    else { SetColor(4), std::cout << "(7)", SetColor(7); std::cout << "Inf jump is disabled\n"; };

    if (fastGrenadeStatus == 1) { SetColor(10), std::cout << "(8)", SetColor(7); std::cout << "Fast grenades is enabled\n"; }
    else { SetColor(4), std::cout << "(8)", SetColor(7); std::cout << "Fast grenades is disabled\n"; };

    if (speedStatus == 1) { SetColor(10), std::cout << "(9)", SetColor(7); std::cout << "Fast speed is enabled\n"; }
    else { SetColor(4), std::cout << "(9)", SetColor(7); std::cout << "Fast speed is disabled\n"; };

    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
    std::cout << "(Enter 10 to go back to options)\n";

    //offsets
    DWORD botXOffset = 0x310;
    DWORD botY = 0x314;
    DWORD botZ = 0x318;
    DWORD healthOffset = 0xEC;
    DWORD infJumpOffsett = 0x5C;
    DWORD staticHealthOffsset = 0x04;
    DWORD SheildOffset = 0xF0;
    DWORD GrenadeOffset = 0x144;
    DWORD fastShooting[3] = { 0x158, 0x160, 0x164 };
    DWORD ammoOffsets[6] = { 0x12C, 0x134, 0x138, 0x13C, 0x140, 0x144 };
    DWORD nameOffset = 0x205;
    DWORD posY = 0x28;
    DWORD posZ = 0x30;
    DWORD posX = 0x2C;
    DWORD team = 0x224;

    //static address's
    DWORD localPlayerAddress = baseAddress + 0x0017E0A8;
    DWORD entityList = baseAddress + 0x18AC04;
    DWORD playerBase = baseAddress + 0x0017E0A8;
    DWORD FOVAddress = baseAddress + 0x18A7CC;
    DWORD ViewMatrix = baseAddress + 0x185C14;
    DWORD PlayerCount = baseAddress + 0x18AC0C;
    DWORD staticammoAddress = baseAddress + 0xC73EF;
    DWORD fastshootingAddress = baseAddress + 0xC73EA;
    DWORD arAntiRecoil = baseAddress + 0xC2EC3;
    DWORD statichealthAddress = baseAddress + 0x1C223; 
    DWORD fastGrenadeAddress = baseAddress + 0xC7E69;
    DWORD fastGrenadeAddress1 = baseAddress + 0xC77A8;
    DWORD staticGrenadeAddress = baseAddress + 0xC7E5D;
    DWORD infJumpAddress = baseAddress + 0xC140E;
    DWORD infJumpAddress2 = baseAddress + 0xC23CB;
    DWORD shootthing = baseAddress + 0xBFBCD;
    DWORD spectatorModeOffsett = baseAddress + 0x74;
    DWORD writingtospectarAddress = baseAddress + 0xAC170;
    DWORD speedAddress = baseAddress + 0x0017E0A8;
        ;
    //bytes
    BYTE ammonop[2] = { 0x90, 0x90 };
    BYTE nop[1] = { 0x90 };
    BYTE healthnop[3] = { 0x90, 0x90, 0x90 };
    BYTE healthOrig[3] = { 0x29, 0x9B, 0x4 };
    BYTE grenadenop[2] = { 0x90, 0x90 };
    BYTE kockbacknop[6] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    BYTE fastshootnop[2] = { 0x90, 0x90 };
    BYTE fastshootorig[2] = {0x89, 0x08};
    BYTE staticammoOrig[2] = { 0xFE, 0x08 }; 
    BYTE setArRecoil = 0;
    BYTE setArRecoilOrig = 0xF3;
    BYTE fastGreandeOrig[2] = { 0x89, 0x08 };
    BYTE infJumpOrig2[3] = { 0x88, 0x45, 0x5D,};
    BYTE speedValue[1] = { 0x4 };

    int val = 256;
    int infinteJump = 1;
    int infinteJumpOrig = 0;
    int setHealth = 1337;
    int setStaticHealth = 0;
    int setAmmo = 1337;
    int SetSheild = 1337;
    int setGrenadeAmmo = 1337;
    int setFOV = 1125999999;
    int setFOVOringinal = 1123024896;
    int setSheildOriginal = 0;
    int x = time(0);

    //fast greandes
    if (fastGrenadeStatus == 1) {
        if (!WriteProcessMemory(hProcess, (LPVOID)fastGrenadeAddress, &ammonop, sizeof(ammonop ), NULL)) {
            std::cout << "Error Writing fast grenade memory: " << GetLastError() << std::endl;
        }
    }
    if (fastGrenadeStatus == 2) {
        if (!WriteProcessMemory(hProcess, (LPVOID)fastGrenadeAddress, &fastGreandeOrig, sizeof(fastGreandeOrig), NULL)) {
            std::cout << "Error Writing fast grenade memory: " << GetLastError() << std::endl;
        }
    }

    //infinitejump
    if (setInfJump == 1) {
        if (!WriteProcessMemory(hProcess, (LPVOID)infJumpAddress, &healthnop, sizeof(healthnop), NULL)) {
            std::cout << "Error Writing inf jump memory: " << GetLastError() << std::endl;
        }
    }
    if (setInfJump == 2) {
        if (!WriteProcessMemory(hProcess, (LPVOID)infJumpAddress, &infJumpOrig2, sizeof(infJumpOrig2), NULL)) {
            std::cout << "Error Writing inf jump memory: " << GetLastError() << std::endl;
        }
    }

    //no recoil
    if (setAntiRecoil == 1) {
        if (!WriteProcessMemory(hProcess, (LPVOID)arAntiRecoil, &setArRecoil, sizeof(setArRecoil), NULL)) {
            std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
        }
    }
    if (setAntiRecoil == 2) {
        if (!WriteProcessMemory(hProcess, (LPVOID)arAntiRecoil, &setArRecoilOrig, sizeof(setArRecoilOrig), NULL)) {
            std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
        }
    }

    //fast shoot/staciammo
    if (setFastShoot == 1) {
        if (!WriteProcessMemory(hProcess, (LPVOID)fastshootingAddress, &fastshootnop, sizeof(fastshootnop), NULL)) {
            std::cout << "Error Writing fast shooting memory: " << GetLastError() << std::endl;
        }
        if (!WriteProcessMemory(hProcess, (LPVOID)staticammoAddress, &ammonop, sizeof(ammonop), NULL)) {
                std::cout << "Error Writing staic ammo " << GetLastError() << std::endl;
         }
    }
    if (setFastShoot == 2) {
        if (!WriteProcessMemory(hProcess, (LPVOID)fastshootingAddress, &fastshootorig, sizeof(fastshootorig), NULL)) {
            std::cout << "Error Writing fast shooting memory: " << GetLastError() << std::endl;
        }
        
    }

    //static health
    if (StaticHealthstatus == 1) {

        if (!WriteProcessMemory(hProcess, (LPVOID)statichealthAddress, &healthnop, sizeof(healthnop), NULL)) {
            std::cout << "Error Writing FOV: " << GetLastError() << std::endl;
        }
    }
    else
    {

    }

    //static ammo
    if (StaticAmmostatus == 1) {

    if (!WriteProcessMemory(hProcess, (LPVOID)staticammoAddress,  &ammonop, sizeof(ammonop), NULL)) {
        std::cout << "Error Writing FOV: " << GetLastError() << std::endl;
    }
}
    else {

    }

    //FOV
    if (SetFOVstatus == 1) {

        if (!WriteProcessMemory(hProcess, (LPVOID)FOVAddress, &setFOV, sizeof(setFOV), NULL)) {
            std::cout << "Error Writing FOV: " << GetLastError() << std::endl;
        }
    }

    else if (SetFOVstatus == 2) {
        if (!WriteProcessMemory(hProcess, (LPVOID)FOVAddress, &setFOVOringinal, sizeof(setFOVOringinal), NULL)) {
            std::cout << "Error Writing FOV to its original state: " << GetLastError() << std::endl;
        }

    }
    while (true) {
        //config
        std::cin >> config;


        //aimbot
        float entity_distance = 999;
        float viewmatrix[16];
        int Players;
        DWORD temp_entity;
        DWORD entity_last;

        for (int a = 1; a <= Players; a++) {
         
         ReadProcessMemory(hProcess, (LPVOID)(entityList), &temp_entity, 4, 0);

        }


        //fly
        if (config == 7) {
            std::cout << "Press 1 if you like to enable fly exploit or 2 to disable it: ";
            std::cin >> setInfJump;
            system("cls");

            if (setInfJump == 2) {
                std::cout << "fly exploit disabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)infJumpAddress, &infJumpOrig2, sizeof(infJumpOrig2), NULL)) {
                    std::cout << "Error Writing fly exploit memory: " << GetLastError() << std::endl;
                }
                
            }
            if (setInfJump == 1) {
                std::cout << "fly exploit enabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)infJumpAddress, &healthnop, sizeof(healthnop), NULL)) {
                    std::cout << "Error Writing fly exploit memory: " << GetLastError() << std::endl;
                }

            }
        }

        // fast shooting
        if (config == 1) {
            std::cout << "Press 1 if you like to enable fast shooting or 2 to disable it (strongly advised to use with static ammo): ";
            std::cin >> setFastShoot;
            system("cls");

            if (setFastShoot == 2) {
                std::cout << "fast shoot disabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)fastshootingAddress, &fastshootorig, sizeof(fastshootorig), NULL)) {
                    std::cout << "Error Writing fast shooting memory: " << GetLastError() << std::endl;
                }
            }
            if (setFastShoot == 1) {
                std::cout << "static health enabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)fastshootingAddress, &fastshootnop, sizeof(fastshootnop), NULL)) {
                    std::cout << "Error Writing fast shooting memory: " << GetLastError() << std::endl;
                }
            }
            //static health (broken fix later)
        }
        if (config == 2) {
            std::cout << "Press 1 if you like to have static health or 2 to disable it: ";
            std::cin >> setStaticHealth;
            system("cls");
        
            if (setStaticHealth == 2) {
                std::cout << "static health disabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)statichealthAddress, &healthOrig, sizeof(healthOrig), NULL)) {
                    std::cout << "Error Writing static health memory: " << GetLastError() << std::endl;
                }
            }
        
            if (setStaticHealth == 1) {
                std::cout << "static health enabled\n";
                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
                if (!WriteProcessMemory(hProcess, (LPVOID)statichealthAddress, &healthnop, sizeof(healthnop), NULL)) {
                    std::cout << "Error Writing static health memory: " << GetLastError() << std::endl;
                }
            }
        }
            //static ammo
            if (config == 3) {
                std::cout << "Press 1 if you like to have static ammo or 2 to disable it: ";
                std::cin >> StaticAmmostatus;
                system("cls");

                if (StaticAmmostatus == 2) {
                    std::cout << "static ammo disabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)staticammoAddress, &staticammoOrig, sizeof(staticammoOrig), NULL)) {
                        std::cout << "Error Writing static ammo memory: " << GetLastError() << std::endl;
                    }
                    //fix later
                    if (!WriteProcessMemory(hProcess, (LPVOID)staticGrenadeAddress, &ammonop, sizeof(ammonop), NULL)) {
                        std::cout << "Error Writing static ammo memory: " << GetLastError() << std::endl;
                    }

                }
                if (StaticAmmostatus == 1) {
                    std::cout << "static ammo enabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)staticammoAddress, &ammonop, sizeof(ammonop), NULL)) {
                        std::cout << "Error Writing static health memory: " << GetLastError() << std::endl;
                    }
                    if (!WriteProcessMemory(hProcess, (LPVOID)staticGrenadeAddress, &ammonop, sizeof(ammonop), NULL)) {
                        std::cout << "Error Writing static ammo memory: " << GetLastError() << std::endl;
                    }
                }
            }
            //anti recoil
            if (config == 6) {
                std::cout << "Enter 1 to enable anti-recoil or 2 to disable anti-recoil: ";
                std::cin >> setAntiRecoil;
                system("cls");

                if (setAntiRecoil == 2) {
                    std::cout << "Anti-recoil disabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";

                    if (!WriteProcessMemory(hProcess, (LPVOID)arAntiRecoil, &setArRecoilOrig, sizeof(setArRecoilOrig), NULL)) {
                        std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
                    }
                }
                if (setAntiRecoil == 1) {
                    std::cout << "Anti-recoil enabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)arAntiRecoil, &setArRecoil, sizeof(setArRecoil), NULL)) {
                        std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
                    }

                }
            }

            //FOV
            if (config == 5) {
                std::cout << "Enter 1 to enable FOV or 2 to disable FOV: ";
                std::cin >> SetFOVstatus;
                system("cls");

                if (SetFOVstatus == 2) {
                    std::cout << "FOV disabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)FOVAddress, &setFOVOringinal, sizeof(setFOVOringinal), NULL)) {
                        std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
                    }
                }
                if (SetFOVstatus == 1) {
                    std::cout << "FOV enabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)FOVAddress, &setFOV, sizeof(setFOV), NULL)) {
                        std::cout << "Error Writing anti-recoil memory: " << GetLastError() << std::endl;
                    }
                }
            }

            //fast grenades
            if (config == 8) {
                std::cout << "Press 1 if you like to enable fast grenades or 2 to disable it: ";
                std::cin >> fastGrenadeStatus;
                system("cls");

                if (fastGrenadeStatus == 1) {
                    std::cout << "Fast grenades enabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)fastGrenadeAddress, &ammonop, sizeof(ammonop), NULL)) {
                        std::cout << "Error Writing fast grenade memory: " << GetLastError() << std::endl;
                    }
                }
                if (fastGrenadeStatus == 2) {
                    std::cout << "Fast grenades disabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (!WriteProcessMemory(hProcess, (LPVOID)fastGrenadeAddress, &fastGreandeOrig, sizeof(fastGreandeOrig), NULL)) {
                        std::cout << "Error Writing fast grenade memory: " << GetLastError() << std::endl;
                    }
                }
            }

            //speed exploit
            if (config == 9) {
                std::cout << "Press 1 if you like to fast speed or 2 to disable it: ";
                std::cin >> speedStatus;
                system("cls");

                if (speedStatus == 1) {
                    std::cout << "Fast speed is enabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                    if (GetAsyncKeyState(VK_LCONTROL)) {
                        while (VK_LCONTROL) {
                            if (!WriteProcessMemory(hProcess, (LPVOID)speedAddress, speedValue, sizeof(speedValue), NULL)) {
                                std::cout << "Error Writing fast grenade memory: " << GetLastError() << std::endl;
                            }
                        }
                    }
                }
                if (speedStatus == 2) {
                    std::cout << "Fast speed is disabled\n";
                    std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                    std::cout << "(Enter 10 to go back to options)\n";
                }
            }

            //options list 
            if (config == 10) {
                system("cls");
                std::cout << "Current config is:\n";
                if (setFastShoot == 1) { SetColor(10), std::cout << "(1)", SetColor(7); std::cout << "Fast shoot is enabled\n"; }
                else { SetColor(4), std::cout << "(1)", SetColor(7); std::cout << "Fast shoot is disabled\n"; };

                if (StaticHealthstatus == 1) { SetColor(10), std::cout << "(2)", SetColor(7); std::cout << "Static health is enabled\n"; }
                else { SetColor(4), std::cout << "(2)", SetColor(7); std::cout << "Static health is disabled\n"; };

                if (StaticAmmostatus == 1) { SetColor(10), std::cout << "(3)", SetColor(7); std::cout << "Static ammo is enabled\n"; }
                else { SetColor(4), std::cout << "(3)", SetColor(7); std::cout << "Static ammo is disabled\n"; };

                if (SetFOVstatus == 1) { SetColor(10), std::cout << "(5)", SetColor(7); std::cout << "FOV is enabled\n"; }
                else { SetColor(4), std::cout << "(5)", SetColor(7); std::cout << "FOV is disabled\n"; };

                if (setAntiRecoil == 1) { SetColor(10), std::cout << "(6)", SetColor(7); std::cout << "Anti-recoil is enabled\n"; }
                else { SetColor(4), std::cout << "(6)", SetColor(7); std::cout << "Anti-recoil is disabled\n"; };

                if (setInfJump == 1) { SetColor(10), std::cout << "(7)", SetColor(7); std::cout << "Inf jump is enabled\n"; }
                else { SetColor(4), std::cout << "(7)", SetColor(7); std::cout << "Inf jump is disabled\n"; };

                if (fastGrenadeStatus == 1) { SetColor(10), std::cout << "(8)", SetColor(7); std::cout << "Fast grenades is enabled\n"; }
                else { SetColor(4), std::cout << "(8)", SetColor(7); std::cout << "Fast grenades is disabled\n"; };

                if (speedStatus == 1) { SetColor(10), std::cout << "(9)", SetColor(7); std::cout << "Fast speed is enabled\n"; }
                else { SetColor(4), std::cout << "(9)", SetColor(7); std::cout << "Fast speed is disabled\n"; };

                std::cout << "Enter exploit number (e.g press 1 to change fast shoot)\n";
                std::cout << "(Enter 10 to go back to options)\n";
            }
            


            
        }
        CloseHandle(hProcess);
        return 0;
}


    