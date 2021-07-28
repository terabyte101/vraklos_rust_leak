#include "../rust/rust.hpp"
#include "../Entities/config.hpp"
#include <mutex>
#include <iostream>

Entity currentent;
baseplayer currentplayer;
baseplayer::weapon wep;

namespace basenet {
	std::string getobjectname(std::uint64_t class_object)
	{
		const auto object_unk = driver::Read<uintptr_t>(class_object);

		if (!object_unk)
			return {};

		return rust::readchar(driver::Read<uintptr_t>(object_unk + 0x10));
	}

	void basenetworkable_loop() {
		uintptr_t last_object_list_size = NULL;
		while (true) {
			const auto unk1 = driver::Read<uintptr_t>(rust::basenetworkable + 0xB8);
			if (!unk1)
				return;

			const auto client_entities = driver::Read<uintptr_t>(unk1);
			if (!client_entities)
				return;

			const auto entity_realm = driver::Read<uintptr_t>(client_entities + 0x10);
			if (!entity_realm)
				return;

			const auto buffer_list = driver::Read<uintptr_t>(entity_realm + 0x28);
			if (!buffer_list)
				return;

			std::vector<baseplayer> local_players;
			std::vector<Entity> local_corpse;
			std::vector<Entity> local_vehicles;
			std::vector<Entity> local_Stash;
			std::vector<Entity> local_Raid;
			std::vector<Entity> local_Airdrop;
			std::vector<Entity> local_patrol_heli;
			std::vector<Entity> local_tool_cupboard;
			std::vector<Entity> local_hackable_crate;
			std::vector<Entity> local_hemp;
			std::vector<Entity> local_food;
			std::vector<Entity> local_cargo_ship;
			std::vector<Entity> local_DroppedItem;
			std::vector<Entity> local_Animal;
			std::vector<Entity> local_SulfurNodes;
			std::vector<Entity> local_StoneNodes;
			std::vector<Entity> local_MetalNodes;
			std::vector<Entity> local_high_tier_crates;
			std::vector<Entity> local_low_tier_crates;

			auto current_object_list_size = driver::Read<std::uint32_t>(buffer_list + 0x10);
			try {
				for (auto i = 0; i <= current_object_list_size; i++)
				{
					const auto object_list = driver::Read<uintptr_t>(buffer_list + 0x18);
					if (!object_list)
						continue;

					const auto current_object = driver::Read<uintptr_t>(object_list + (0x20 + (i * 8)));//baseObject
					if (!current_object)
						continue;

					const auto baseObject = driver::Read<uintptr_t>(current_object + 0x10);//entity
					if (!baseObject)
						continue;

					uintptr_t object = driver::Read<uintptr_t>(baseObject + 0x30);
					if (!object)
						continue;

					uintptr_t objectclass = driver::Read<uintptr_t>(object + 0x30);

					WORD tag = driver::Read<WORD>(object + 0x54);

					uintptr_t ent_name = driver::Read<UINT_PTR>(object + 0x60);//object name
					std::string objectname = rust::readchar(ent_name);

					uintptr_t transform = driver::Read<uintptr_t>(objectclass + 0x8);
					currentent.transform = transform;

					uintptr_t entity = driver::Read<uintptr_t>(objectclass + 0x18);
					
					uintptr_t BasePlayer = driver::Read<uintptr_t>(entity + 0x28);
					currentent.baseentity = BasePlayer;

					uintptr_t networkable = driver::Read<uintptr_t>(BasePlayer + 0x50);
					int id = driver::Read<int>(networkable + 0x10);
					currentent.networkid = id;

					uintptr_t visualstate = driver::Read<uintptr_t>(transform + 0x38);
					Vector3 position = driver::Read<Vector3>(visualstate + 0x90);

					currentent.position = position;
					if (config.node_esp) {
						if (config.sulfur_node) {
							if ((objectname.find(xorget("ORE_SULFUR")) != std::string::npos) ||
								(objectname.find(xorget("sulfur-ore")) != std::string::npos) ||
								(objectname.find(xorget("ORE_SULFUR")) != std::string::npos)) {
								int id = driver::Read<int>(networkable + 0x10);

								local_SulfurNodes.push_back(currentent);
								
							}
						}
						if (config.stone_node) {
							if ((objectname.find(xorget("ORE_STONE")) != std::string::npos) ||
								(objectname.find(xorget("stone-ore")) != std::string::npos) ||
								(objectname.find(xorget("ORE_STONE")) != std::string::npos)) {

								local_StoneNodes.push_back(currentent);
								
							}
						}
						if (config.metal_node) {
							if ((objectname.find(xorget("ORE_METAL")) != std::string::npos) ||
								(objectname.find(xorget("metal-ore")) != std::string::npos) ||
								(objectname.find(xorget("ORE_METAL")) != std::string::npos)) {

								local_MetalNodes.push_back(currentent);
								
							}
						}
					}
					if (config.food_esp) {
						if (objectname.find(xorget("pumpkin-collectable")) != std::string::npos || objectname.find(xorget("potato-collectable")) != std::string::npos || objectname.find(xorget("corn-collectable")) != std::string::npos || objectname.find(xorget("mushroom-cluster-5")) != std::string::npos || objectname.find(xorget("mushroom-cluster-6")) != std::string::npos) {

							local_food.push_back(currentent);
							
						}
					}
					if (config.high_tier_crate) {
						if (objectname.find(xorget("_crate.prefab")) != std::string::npos || objectname.find(xorget("crate_elite.prefab")) != std::string::npos || objectname.find(xorget("crate_normal.prefab")) != std::string::npos) {
							currentent.name = objectname;

							local_high_tier_crates.push_back(currentent);
							
						}
					}
					if (config.low_tier_crate) {
						if (objectname.find(xorget("crate_")) != std::string::npos && objectname.find(xorget("wooden_crate")) == std::string::npos && objectname.find(xorget("crate_cover_300x600.prefab")) == std::string::npos && objectname.find(xorget("crate_elite.prefab")) == std::string::npos && objectname.find(xorget("crate_normal.prefab")) == std::string::npos) {
							currentent.name = objectname;

							local_low_tier_crates.push_back(currentent);
							
						}
					}
					if (config.hemp) {
						if (objectname.find(xorget("hemp-collectable")) != std::string::npos) {

							local_hemp.push_back(currentent);
							
						}
					}

					if (config.airdrop) {
						if (objectname.find(xorget("event_airdrop")) != std::string::npos) {

							local_Airdrop.push_back(currentent);
							
						}
					}
					if (config.cargo_esp) {
						if (objectname.find(xorget("cargoship")) != std::string::npos) {

							local_cargo_ship.push_back(currentent);
							
						}
					}

					if (config.tool_cupboard) {
						if (objectname.find(xorget("cupboard.tool.deployed.prefab")) != std::string::npos) {

							local_tool_cupboard.push_back(currentent);
							
						}
					}

					if (config.hackable_crate) {
						if (objectname.find(xorget("codelockedhackablecrate")) != std::string::npos) {
							local_hackable_crate.push_back(currentent);
							
						}
					}

					if (config.patrol_heli) {
						if (objectname.find(xorget("patrolhelicopter.prefab")) != std::string::npos) {

							local_patrol_heli.push_back(currentent);
							
						}
					}
					//player
					if (tag == 6) {
						int pHealth = (int)(driver::Read<float>(BasePlayer + 0x214));

						uintptr_t StringPlayerName = driver::Read<uintptr_t>(BasePlayer + 0x698);

						uint64_t Inventory = driver::Read<uint64_t>(BasePlayer + 0x648);
						uint64_t Belt = driver::Read<uint64_t>(Inventory + 0x28);
						uint64_t ItemList = driver::Read<uint64_t>(Belt + 0x38);
						uint64_t Items = driver::Read<uint64_t>(ItemList + 0x10);

						currentplayer.playerflags = driver::Read<int>(currentplayer.baseentity + 0x638);
						currentplayer.Items = Items;
						currentplayer.prefabname = objectname;
						currentplayer.playername = rust::readstring(StringPlayerName);
						currentplayer.transform = transform;
						currentplayer.baseentity = BasePlayer;
						currentplayer.networkid = id;
						currentplayer.input = driver::Read<uint64_t>(currentplayer.baseentity + 0x4D0); // public PlayerInput input;
						currentplayer.position = position;
						currentplayer.sleeping = driver::Read<int>(currentplayer.baseentity + 0x638);
					
						if (objectname.find(xorget("LocalPlayer")) != std::string::npos) {
							currentplayer.PlayerWalkMovement = driver::Read<UINT_PTR>(BasePlayer + 0x4D8);
							

							if (config.setadmin) { driver::write<int>(currentplayer.baseentity + 0x638, 4); }
							
							wep.held = driver::Read<DWORD64>(currentplayer.baseentity + 0x98);
							
							wep.activeweaponz = currentplayer.GetActiveWeapon();
							wep.currentweaponzz = driver::Read<uintptr_t>(wep.activeweaponz + 0x28);
							if (wep.currentweaponzz)
							{
								wep.baseprojectilez = driver::Read<uint64_t>(wep.activeweaponz + 0x98);
							}

							rust::local = &currentplayer;
							rust::localclass = currentplayer;
						}
						else {
							local_players.push_back(currentplayer);
						}
						wep.activeweapon = currentplayer.GetActiveWeapon();
						wep.baseprojectile = driver::Read<uint64_t>(wep.activeweapon + 0x98);
						currentplayer.weaponptr = &wep;
					}

					if (config.heli_esp) {
						if (objectname.find(xorget("minicopter.entity")) != std::string::npos || objectname.find(xorget("scraptransporthelicopter")) != std::string::npos || objectname.find(xorget("rhib.prefab")) != std::string::npos || objectname.find(xorget("rowboat.prefab")) != std::string::npos) {
							float _health = driver::Read<float>(BasePlayer + 0x214);
							float health_max = driver::Read<float>(BasePlayer + 0x210);

							currentent.name = objectname;

							auto health_pc = min(_health / health_max, 1.f);

							local_vehicles.push_back(currentent);
							
						}
					}

					if (config.Raid_esp) {
						if (objectname.find(xorget("assets/prefabs/tools/c4/effects/c4_explosion.prefab")) != std::string::npos) {

							local_Raid.push_back(currentent);

						}
					}

					if (config.stash_esp) {
						if (objectname.find(xorget("small_stash_deployed")) != std::string::npos) {

							local_Stash.push_back(currentent);
							
						}
					}

					if (config.dropped_esp) {
						std::string class_name = getobjectname(current_object);

						if (class_name.find(xorget("DroppedItem")) != std::string::npos) {
							currentent.name = objectname;

							local_DroppedItem.push_back(currentent);
							
						}
					}

					//Corpse
					if (config.corpse_esp) {
						if (tag == 20009) {
							local_corpse.push_back(currentent);
							
						}
					}
				}
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				last_object_list_size = current_object_list_size;
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				printf(xorget("ERROR \n"));
			}

			std::lock_guard<std::mutex> lk(entity_mutex);
			otherplayers = std::move(local_players);
			corpse = std::move(local_corpse);
			vehicles = std::move(local_vehicles);
			Stash = std::move(local_Stash);
			explosion = std::move(local_Raid);
			DroppedItem = std::move(local_DroppedItem);
			Animal = std::move(local_Animal);
			SulfurNodes = std::move(local_SulfurNodes);
			StoneNodes = std::move(local_StoneNodes);
			MetalNodes = std::move(local_MetalNodes);

			Airdrop = std::move(local_Airdrop);
			patrol_heli = std::move(local_patrol_heli);
			tool_cupboard = std::move(local_tool_cupboard);
			hackable_crate = std::move(local_hackable_crate);
			hemp = std::move(local_hemp);
			food = std::move(local_food);
			cargo_ship = std::move(local_cargo_ship);
			high_tier_crates = std::move(local_high_tier_crates);
			low_tier_crates = std::move(local_low_tier_crates);
		}
	}
}