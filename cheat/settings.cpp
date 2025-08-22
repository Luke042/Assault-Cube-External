#include "settings.h"
#include <string>
#include <vector>
#include <fstream>

namespace aim {
	bool aimbot_toggle = false;
	bool perfect_aim = false;
	bool bezier_aim = false;

	int aim_key = 0x06;

	bool aimbot_team_check = false;
	bool aimbot_visable_check = false;

	float aimbot_smoothening = 0.5;

	bool silent_aim = false;
	int silent_key = 0x06;

	bool silent_team_check = false;
	bool silent_visable_check = false;

	bool triggerbot = false;
	int trigger_key = 0x06;

	bool trigger_team_check = false;
	bool trigger_visable_check = false;

	bool bezier_team_check = false;
	bool bezier_visable_check = false;
}

namespace visuals {
	bool esp_toggle = false;

}

namespace exploits {
	bool inf_jump = false;
	bool fast_shoot = false;
	bool static_ammo = false;
	bool anti_recoil = false;
	bool static_health = false;
	bool fov = false;
	bool fast_grenade = false;
	bool fly = false;
	bool better_fly = false;
	bool overwrite_health = false;
	bool overwrite_ammo = false;
	bool kill_aura = false;
}

namespace values {
	float fov_value;
	float fov_orig;
	float fly_height = 5;

	float wp_posX;
	float wp_posY;
	float wp_posZ;

	float wp_posX2;
	float wp_posY2;
	float wp_posZ2;

	float player_X;
	float player_Y;
	float player_Z;

	bool save_position = false;
	bool load_position = false;

	bool save_position2 = false;
	bool load_position2 = false;

	bool show_menu = true;

	float menu_pos;

	float fps;

	bool low_end = false;

	float bezier_speed = 0.5f;

	float best_pitch = 0.0f;
	float best_yaw = 0.0f;
}