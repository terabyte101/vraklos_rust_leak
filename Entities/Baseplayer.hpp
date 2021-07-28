#include "../Vectors/Vector.hpp"
#include "../Vars.h"

int lengthz(uintptr_t addr) { return driver::Read<int>(addr + 0x10); }

std::string readstringz(uintptr_t addr) {
	try {
		static char buff[128] = { 0 };
		buff[lengthz(addr)] = '\0';

		for (int i = 0; i < lengthz(addr); ++i) {
			if (buff[i] < 128) {
				buff[i] = driver::Read<UCHAR>(addr + 0x14 + (i * 2));
			}
			else {
				buff[i] = '?';
				if (buff[i] >= 0xD800 && buff[i] <= 0xD8FF)
					i++;
			}
		}
		return std::string(&buff[0], &buff[lengthz(addr)]);
	}
	catch (const std::exception& exc) {}
	return xorget("Error");
}


class baseplayer {
public:
	std::string playername;
	std::string prefabname;
	
	uintptr_t transform;
	int networkid;
	uintptr_t baseentity;
	uintptr_t PlayerWalkMovement;
	uintptr_t Localplayer;
	uintptr_t Items;
	int playerflags;
	UINT_PTR playerflagsz;
	int sleeping;
	uintptr_t input;
	uintptr_t inpus;
	Vector3 position;
	uintptr_t Gameassmebly;

	void set_viewangles(Vector3 angle) {
		inpus = driver::Read<uint64_t>(this->baseentity + 0x4D0);
		driver::write<Vector3>(inpus + 0x3C, angle);
	}

	Vector3 get_viewangles() {
		inpus = driver::Read<uint64_t>(this->baseentity + 0x4D0);
		return driver::Read<Vector3>(inpus + 0x3C);
	}

	struct TransformAccessReadOnly
	{
		uint64_t pTransformData;
	};
	struct TransformData
	{
		uint64_t pTransformArray;
		uint64_t pTransformIndices;
	};
	struct Matrix34
	{
		Vector4 vec0;
		Vector4 vec1;
		Vector4 vec2;
	};

	Vector3 GetBonePosition(ULONG_PTR pTransform)
	{
		__m128 result;

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		TransformAccessReadOnly pTransformAccessReadOnly = driver::Read<TransformAccessReadOnly>(pTransform + 0x38);
		unsigned int index = driver::Read<unsigned int>(pTransform + 0x40);
		TransformData transformData = driver::Read<TransformData>(pTransformAccessReadOnly.pTransformData + 0x18);

		if (transformData.pTransformArray && transformData.pTransformIndices)
		{
			result = driver::Read<__m128>(transformData.pTransformArray + (uint64_t)0x30 * index);
			int transformIndex = driver::Read<int>(transformData.pTransformIndices + (uint64_t)0x4 * index);
			int pSafe = 0;
			while (transformIndex >= 0 && pSafe++ < 200)
			{
				Matrix34 matrix34 = driver::Read<Matrix34>(transformData.pTransformArray + (uint64_t)0x30 * transformIndex);

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

				transformIndex = driver::Read<int>(transformData.pTransformIndices + (uint64_t)0x4 * transformIndex);
			}
		}

		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}

	Vector3 get_entity_bone(uintptr_t BasePlayer, int bone_index) {

		uintptr_t player_model = driver::Read<uintptr_t>(BasePlayer + 0x120); //public Model model
		uintptr_t BoneTransforms = driver::Read<uintptr_t>(player_model + 0x48);
		uintptr_t entity_bone = driver::Read<uintptr_t>(BoneTransforms + (0x20 + (bone_index * 0x8)));
		uintptr_t bone = driver::Read<uintptr_t>(entity_bone + 0x10);
		return GetBonePosition(bone);
	}

	bool is_alive()
	{
		return driver::Read<uint32_t>(this->baseentity + 0x20C) == 0;
	}

	bool IsNpc() {
		DWORD64 PlayerModel = driver::Read<DWORD64>(this->baseentity + 0x4B0); //	public PlayerModel playerModel;
		return driver::Read<bool>(PlayerModel + 0x2C0);
	}

	bool is_sleeping(int Flg)
	{
		return (driver::Read<int>(this->baseentity + 0x638) & Flg);
	}

	bool IsVisible()
	{
			UINT64 PlayerModel = driver::Read<UINT64>(this->baseentity + 0x4B0);
			return driver::Read<UINT64>(PlayerModel + 0x248);
	}

	void setnojumpblock() {
		driver::write<Vector3>(this->PlayerWalkMovement + 0xB8, Vector3(0, 1000000, 0));
		driver::write<Vector3>(this->PlayerWalkMovement + 0xB4, Vector3(9999999, 9999999, 9999999));
	}

	void setspiderman() {
		driver::write<float>(this->PlayerWalkMovement + 0xB8, 0);
	}


	void WalkonWater() {
			INT64 Movement = this->PlayerWalkMovement;
			driver::write<bool>(Movement + 0x130, true);
			driver::write<float>(Movement + 0xB4, 0.f);
			driver::write<float>(Movement + 0xB0, 0.f);
			driver::write<float>(Movement + 0x74, 0.f);
	}

	void WalkonWater2() {
		INT64 Movement = this->PlayerWalkMovement;
		driver::write<bool>(Movement + 0x130, false);
		driver::write<float>(Movement + 0xB4, 3.31411f);
		driver::write<float>(Movement + 0xB0, 50.f);
		driver::write<float>(Movement + 0x74, 2.5f);
	}
	
	void LongNeck() {
		DWORD64 eyes = driver::Read<DWORD64>(this->baseentity + 0x640); //public PlayerEyes eyes; // 0x600
		driver::write<Vector3>(eyes + 0x38, Vector3(0, (Settings_::FloatAntInt::LongNeck), 0)); //private Vector3 viewOffset; // 0x38
	}


	//void setadminflag() {
		//driver::write<int>(this->baseentity + 0x638, 4);
	//}

	void GravituMod()
	{
		float Gravity = 2.5f;
		if (GetAsyncKeyState(Settings_::FloatAntInt::GravityButton))
		{
			driver::write<float>(this->PlayerWalkMovement + 0x74, Gravity / Settings_::FloatAntInt::JumpValue);
		}
		else
		{
			driver::write<float>(this->PlayerWalkMovement + 0x74, Gravity);
		}
	}

	uintptr_t GetActiveWeapon()
	{
		unsigned int ActUID = driver::Read<unsigned int>(this->baseentity + 0x5B0);
		if (!ActUID)
			return 0;


		for (int i = 0; i < 6; i++) //For each slot	
		{
			uint64_t WeaponInfo = driver::Read<uint64_t>(this->Items + 0x20 + (i * 0x8));

			unsigned int WeaponUID = driver::Read<unsigned int>(WeaponInfo + 0x28);
			if (!WeaponUID)
				return 0;

			if (ActUID == WeaponUID)
				return WeaponInfo;
		}
		return 0;
	}

	class weapon {
	public:
		uintptr_t activeweapon;
		uintptr_t activeweaponz;
		
		uintptr_t currentweaponzz;
		std::string weaponName;
		uintptr_t baseprojectile;
		uintptr_t baseprojectilez;
		
		uintptr_t held;

		std::string get_active_weapon_name()
		{
			uint64_t itemdefinition = driver::Read<uint64_t>(activeweaponz + 0x20);
			uint64_t displayName = driver::Read<uint64_t>(itemdefinition + 0x28);
			uint64_t english = driver::Read<uint64_t>(displayName + 0x18);
			if (!english)
				return std::string();
			else
				return readstringz(english);
		}

		void setnorecoil() {
			const auto recoil_properties = driver::Read<uintptr_t>(this->baseprojectilez + 0x2C8);
			if (!recoil_properties)
				return;

			for (int i = 0; i < 8; i++)
				driver::write<float>(recoil_properties + 0x18 + i * 4, 0);
		}

		void setnospread() {
			driver::write<float>(this->baseprojectilez + 0x30C, -3);//aimCone
			driver::write<float>(this->baseprojectilez + 0x310, -3);//hipAimCone
			driver::write<float>(this->baseprojectilez + 0x2DC, -3);//aimConePenaltyMax
			driver::write<float>(this->baseprojectilez + 0x2D8, -3);//aimconePenaltyPerShot
			driver::write<float>(this->baseprojectilez + 0x2E0, -3);//stancePenalty
		}

		void setnosway() {
			driver::write<float>(this->baseprojectilez + 0x2B8, 0);//aimSway
			driver::write<float>(this->baseprojectilez + 0x2BC, 0);//aimSwaySpeed
		}

		void seteoka() {

			driver::write<float>(this->baseprojectilez + 0x350, 500.f);
			driver::write<bool>(this->baseprojectilez + 0x358, true); //public RecoilProperties strikeRecoil;
			driver::write<bool>(this->baseprojectilez + 0x360, true); //private bool _didSparkThisFrame;
		}


		void Bulletthickness() {
			if (Settings_::Misc::FatBullet)
			{
				Settings_::Misc::fatbullet_deactivate = true;
				DWORD64 List = driver::Read<DWORD64>(this->baseprojectilez + 0x348);
				int size = driver::Read<DWORD64>(List + 0x18);
				List = driver::Read<DWORD64>(List + 0x10);
				{
					for (int i = 0; i < size; ++i)
					{
						UINT64 Item = driver::Read<DWORD64>(List + 0x20 + (i * 0x8));
						driver::write<float>(Item + 0x2C, Settings_::FloatAntInt::fatbullet_value);
					}
				}
			}

			else if (Settings_::Misc::FatBullet == false && Settings_::Misc::fatbullet_deactivate == true)
			{
				DWORD64 List = driver::Read<DWORD64>(this->baseprojectilez + 0x348);
				int size = driver::Read<DWORD64>(List + 0x18);
				List = driver::Read<DWORD64>(List + 0x10);
				{
					for (int i = 0; i < size; ++i)
					{
						UINT64 Item = driver::Read<DWORD64>(List + 0x20 + (i * 0x8));
						driver::write<float>(Item + 0x2C, 0.1f);
					}
				}

			}
		}



		void SetAutomatic()
		{
			if (Settings_::Misc::automatic)
			{
				Settings_::Misc::automatic_deactivate = true;
				UINT64 Held = this->baseprojectilez;
				driver::write<bool>(Held + 0x278, 1);
			}
			else if (Settings_::Misc::automatic == false && Settings_::Misc::automatic_deactivate == true)
			{
				UINT64 Held = this->baseprojectilez;
				driver::write<bool>(Held + 0x278, 0);
			}
		}


		float repeatdelay;
		float LongHandval;

		void RapidFire()
		{
			if (Settings_::Misc::RapidFire)
			{
				Settings_::Misc::RapidFire_deactivate = true;
				float repeatdelay = driver::Read<float>(this->baseprojectilez + 0x1E4);
				driver::write<float>(this->baseprojectilez + 0x1E4, 0.09f);

			}
			else if (Settings_::Misc::RapidFire_deactivate == true && Settings_::Misc::RapidFire == false)
			{
				driver::write<float>(this->baseprojectilez + 0x1E4, repeatdelay);

			}
		}



		void LongHand()
		{
			/*
			if (Settings_::Misc::LongHand)
			{
				driver::write<float>(this->baseprojectilez + 0x278, Settings_::FloatAntInt::LongHandValue);
				std::cout << "Longhand ON: " << driver::Read<float>(this->baseprojectilez + 0x278) << std::endl;
			}
			else
			{
				uint64_t Held = this->baseprojectilez;
				driver::write<float>(Held + 0x278, 1.3f);
				std::cout << "Longhand OFF: " << driver::Read<float>(Held + 0x278) << std::endl;
			}
			*/
		}

		bool is_weapon()
		{
			if (!this->currentweaponzz)
			{
				return false;
			}
			return driver::Read<uint32_t>(this->currentweaponzz + 0x20) == 0;
		}
	};

	baseplayer::weapon* weaponptr;
};