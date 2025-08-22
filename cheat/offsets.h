#pragma once

namespace global_offsets {
	inline uintptr_t local_player = 0x0017E0A8;
	inline uintptr_t entity_list_addr = 0x18AC04;
	inline uintptr_t player_count = 0x18AC0C;
	inline uintptr_t fov = 0x18A7CC;
	inline uintptr_t game_mode = 0x00183828 + 0x8 + 0x5E5 + 0x78 + 0x28;
}

namespace world_offsets {
	inline uintptr_t pos_x = 0x28;
	inline uintptr_t pos_y = 0x2C;
	inline uintptr_t pos_z = 0x30; 

	inline uintptr_t head_x = 0x4;
	inline uintptr_t head_y = 0x8;
	inline uintptr_t head_z = 0xC;

	inline uintptr_t cam_x = 0x34;
	inline uintptr_t cam_y = 0x38;
}

namespace player_offsets {
	inline uintptr_t health = 0xEC;
	inline uintptr_t armor = 0xF0;
	inline uintptr_t name = 0x205;

	inline uintptr_t pistol_ammo = 0x12C;
	inline uintptr_t shotgun_ammo = 0x134;
	inline uintptr_t smg_ammo = 0x138;
	inline uintptr_t sniper_ammo = 0x13C;
	inline uintptr_t rifle_ammo = 0x140;
	inline uintptr_t grenade_ammo = 0x144;

	inline uintptr_t rifle_fastfire = 0x164;
	inline uintptr_t sniper_fastfire = 0x160;
	inline uintptr_t shotgun_fastfire = 0x158;

	inline uintptr_t shoot = 0x204;
}
