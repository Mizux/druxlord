#pragma once

#include <glib.h>
#include <array>
#include <string>

inline constexpr int DRUG_NUM = 17;
inline constexpr int CITY_NUM = 15;
inline constexpr int WEAPON_NUM = 9;
inline constexpr int COUNTRY_NUM = 7;
inline constexpr int RANK_NUM = 6;

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

// Aliases for compatibility
enum LegacyDrugType {
  DRUG_COCAINE,
  DRUG_CRACK,
  DRUG_ECSTACY,
  DRUG_HASHISH,
  DRUG_HEROIN,
  DRUG_ICE,
  DRUG_KAT,
  DRUG_LSD,
  DRUG_MDA,
  DRUG_MORPHIN,
  DRUG_MUSHROOMS,
  DRUG_OPIUM,
  DRUG_PCP,
  DRUG_PEYOTE,
  DRUG_POT,
  DRUG_SPECIALK,
  DRUG_SPEED
};

enum LegacyWeaponType {
  WEAPON_KNIFE,
  WEAPON_PISTOL,
  WEAPON_SHOTGUN,
  WEAPON_MACHINEGUN,
  WEAPON_FLAMETHROWER,
  WEAPON_DYNAMITE,
  WEAPON_GRENADE,
  WEAPON_ROCKETLAUNCHER,
  WEAPON_AREADISRUPTER
};

enum LegacyAmmoType {
  AMMO_PISTOLBULLET,
  AMMO_SHOTGUNSHELL,
  AMMO_MACHINEGUNBULLET,
  AMMO_GASCANISTER,
  AMMO_ROCKET,
  AMMO_ENERGYGLOBE
};

enum LegacyArmorType {
  ARMOR_HEAVYLEATHERCOAT,
  ARMOR_BULLETPROOFVEST
};

enum LegacyItemType {
  ITEM_CANOFNOSCENT
};

enum LegacyCityType {
  CITY_AUSTIN,
  CITY_BEIJING,
  CITY_BOSTON,
  CITY_DETROIT,
  CITY_LONDON,
  CITY_LOSANGELES,
  CITY_MIAMI,
  CITY_MOSCOW,
  CITY_NEWYORK,
  CITY_PARIS,
  CITY_SANFRANCISCO,
  CITY_STPETERSBURG,
  CITY_SYDNEY,
  CITY_TORONTO,
  CITY_VANCOUVER
};

enum LegacyCountryType {
  COUNTRY_AUSTRALIA,
  COUNTRY_CANADA,
  COUNTRY_CHINA,
  COUNTRY_FRANCE,
  COUNTRY_UK,
  COUNTRY_USA,
  COUNTRY_RUSSIA
};

enum LegacyRankType {
  RANK_WANNABE,
  RANK_SMALLTIME,
  RANK_DEALER,
  RANK_BIGDEALER,
  RANK_DISTRIBUTER,
  RANK_DRUGLORD
};

struct City {
  int id;
  int country;
};

struct Drug {
  int qty;
  int price;
  bool available;
};

struct Weapon {
  int id;
  int ammo;
  int destruction;
  bool mass_effect;
  bool has_ammo;
  int price;
};

struct Rank {
  int type;
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

extern GSList *city_list;
extern Drug drug_table[DRUG_NUM][CITY_NUM];

std::string money_string(unsigned int value);
void generate_drug();
