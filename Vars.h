#pragma once

inline namespace Settings_
{
	inline namespace Other
	{
		inline int Width = 1920;
		inline int Height = 1080;
	}

	inline namespace Visuals
	{
		inline bool IgnoreSliping = true;
	}

	inline namespace Aim
	{
		inline bool AimBot = false;
		inline bool Fov_circle = true;
		inline bool Crosshair = false;
		inline int Croshair_size = 1.f;
		inline bool VisibleCheck = true;
		inline bool IgnoreTeam = true;
		inline int Range = 300;
		inline int BoneToAim = 46;//1 - body;
		inline int Fov = 75.f;
	}

	inline namespace FloatAntInt
	{
		inline float TimeOfDay = 12;
		inline float JumpValue = 1.6;
		inline float RCC_VALUE_X = 0;
		inline float RCC_VALUE_Y = 0;
		inline float fatbullet_value = 0.240f;
		inline float GraphicsFov = 100.f;
		inline float LongHandValue = 2.5f;
		inline float LongNeck = 1.5f;
		inline float speedvalue = 1.f;
		inline int GravityButton = VK_MBUTTON;

	}

	inline namespace Misc
	{
		inline bool FatBullet = false;
		inline bool FakeAdmin = true;
		inline bool automatic = false;
		inline bool AlwaysDay = false;
		inline bool SpiderMan = false;
		inline bool anti_spread = false;
		inline bool RecoilNew = false;
		inline bool no_recoil = false;
		inline bool SetGravitu = false;
		inline bool skychange = false;
		inline bool SuperBow = false;
		inline bool InstantEoka = false;
		inline bool WalkonWater = false;
		inline bool nojumpblock = false;
		inline bool FakeLag = false;
		inline bool SilentWalk = false;
		inline bool compound = false;
		inline bool Longhand = false;
		inline bool CustomFov = false;
		inline bool fastcompbow = false;
		inline bool camspeedfix = false;
		inline bool camspeedloop = false;
		inline bool modelrun = false;
		inline bool norecoil_deactivate = false;
		inline bool RapidFire_deactivate = false;
		inline bool fatbullet_deactivate = false;
		inline bool antispread_deactivate = false;
		inline bool automatic_deactivate = false;
		inline bool LongHand_deactivate = false;
		inline bool RapidFire = false;
		inline bool RunonHit = false;
		inline bool Fathand = false;
		inline bool LongHand = false;
		inline bool NoSway = false;
		inline bool SlideWalk = false;
		inline bool SwimOnGround = false;
		inline bool FlyHack = false;
		inline bool BuildAsUWant = false;
		inline bool NightMode = false;
		inline bool SetGravituStarted = false;
		inline bool LongNeck = false;
		inline bool noRain = false;
		inline bool Speedhack = false;
	}

}