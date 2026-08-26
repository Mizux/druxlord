#pragma once

#include <array>
#include <string>

inline constexpr int DRUG_NUM = 17;
inline constexpr int CITY_NUM = 15;
inline constexpr int WEAPON_NUM = 9;
inline constexpr int COUNTRY_NUM = 7;
inline constexpr int RANK_NUM = 6;
inline constexpr int DAY_NUM = 30;

enum class DrugType {
  Cocaine,
  Crack,
  Ecstasy,
  Hashish,
  Heroin,
  Ice,
  Kat,
  LSD,
  MDA,
  Morphine,
  Mushrooms,
  Opium,
  PCP,
  Peyote,
  Pot,
  SpecialK,
  Speed
};

enum class WeaponType {
  Knife,
  Pistol,
  Shotgun,
  MachineGun,
  Flamethrower,
  Dynamite,
  Grenade,
  RocketLauncher,
  AreaDisrupter
};

enum class AmmoType {
  None,
  PistolBullet,
  ShotgunShell,
  MachineGunBullet,
  GasCanister,
  Rocket,
  EnergyGlobe
};

enum class ArmorType {
  HeavyLeatherCoat,
  BulletProofVest
};

enum class ItemType {
  CanOfNoScent
};

enum class CityType {
  Austin,
  Beijing,
  Boston,
  Detroit,
  London,
  LosAngeles,
  Miami,
  Moscow,
  NewYork,
  Paris,
  SanFrancisco,
  StPetersburg,
  Sydney,
  Toronto,
  Vancouver
};

enum class CountryType {
  Australia,
  Canada,
  China,
  France,
  UK,
  USA,
  Russia
};

enum class RankType {
  Wannabe,
  SmallTime,
  Dealer,
  BigDealer,
  Distributer,
  DrugLord
};

struct City {
  CityType id;
  CountryType country;
};

struct Drug {
  int qty;
  int price;
  bool available;
};

struct Weapon {
  WeaponType id;
  AmmoType ammo;
  int destruction;
  bool mass_effect;
  bool has_ammo;
  int price;
};

struct Rank {
  RankType type;
  unsigned long cash;
  int container;
  int capacity;
};

extern const std::array<const char*, DRUG_NUM> drug_name;
extern const std::array<int, DRUG_NUM> drug_price;
extern const std::array<const char*, CITY_NUM> city_name;
extern const std::array<const char*, COUNTRY_NUM> country_name;
extern const std::array<Weapon, WEAPON_NUM> weapon_info;
extern const std::array<City, CITY_NUM> city_info;

extern Drug drug_table[DRUG_NUM][CITY_NUM][DAY_NUM];

std::string money_string(unsigned int value);
void generate_drug();
