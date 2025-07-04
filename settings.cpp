#include "settings.h"

namespace aim {
	bool aimbot_toggle = false;
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
}

namespace values {
	float fov_value = 120.0f;
	float fly_height = 5.0f;

	float wp_posX;
	float wp_posY;
	float wp_posZ;

	bool save_position = false;
	bool load_position = false;
}