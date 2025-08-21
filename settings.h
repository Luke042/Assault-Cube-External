#pragma once
#include <string>
#include <vector>
#include <fstream>
namespace aim {
	extern bool aimbot_toggle;
	extern bool perfect_aim;
	extern bool bezier_aim;

	extern int aim_key;

	extern bool aimbot_team_check;
	extern bool aimbot_visable_check;

	extern float aimbot_smoothening;

	extern bool silent_aim;
	extern int silent_key;

	extern bool silent_team_check;
	extern bool silent_visable_check;

	extern bool triggerbot;
	extern int trigger_key;

	extern bool trigger_team_check;
	extern bool trigger_visable_check;

	extern bool bezier_visable_check;
	extern bool bezier_team_check;

}
namespace visuals {
	extern bool esp_toggle;
	
}

namespace exploits {
	extern bool inf_jump;
	extern bool fast_shoot;
	extern bool static_ammo;
	extern bool anti_recoil;
	extern bool static_health;
	extern bool fov;
	extern bool fast_grenade;
	extern bool fly;
	extern bool better_fly;
	extern bool overwrite_health;
	extern bool overwrite_ammo;
	extern bool kill_aura;
}

namespace values {
	extern float fov_value;
	extern float fov_orig;
	extern float fly_height;

	extern float wp_posX;
	extern float wp_posY;
	extern float wp_posZ;

	extern float wp_posX2;
	extern float wp_posY2;
	extern float wp_posZ2;

	extern float player_X;
	extern float player_Y;
	extern float player_Z;

	extern bool save_position;
	extern bool load_position;

	extern bool save_position2;
	extern bool load_position2;

	extern bool show_menu;

	extern float fps;

	extern bool low_end;

	extern float bezier_speed;

}