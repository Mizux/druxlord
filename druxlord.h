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

enum class ArmorType { HeavyLeatherCoat, BulletProofVest };

enum class ItemType { CanOfNoScent };

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

enum class CountryType { Australia, Canada, China, France, UK, USA, Russia };

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
  int price_factor;
  int distance;
};

struct Drug {
  DrugType id;
  int price;
};

struct DrugState {
  int qty = 0;
  int price = 0;
  int target_price = 0;
  int event_flag = 0;
  int rumor_flag = 0;
  bool available = false;
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

inline constexpr std::array<int, RANK_NUM> rank_capacity = {10,  25,   100,
                                                            600, 3500, 20000};

std::string drug_name(DrugType type);
std::string city_name(CityType type);
std::string country_name(CountryType type);
int flight_cost(int from_city, int to_city);

extern const std::array<Drug, DRUG_NUM> drug_info;
extern const std::array<Weapon, WEAPON_NUM> weapon_info;
extern const std::array<City, CITY_NUM> city_info;

class GameState {
 public:
  GameState();

  void newgame();
  void stay_here();
  void generate_drug();
  std::string get_market_news(int loc, int d) const;
  int flight_cost(int to_city) const;

  // Player pocket inventory
  int player_qty[DRUG_NUM]{};
  int player_price[DRUG_NUM]{};  // average cost basis

  // Vault inventory
  int vault_qty[DRUG_NUM]{};

  DrugState drug_table[DRUG_NUM][CITY_NUM][DAY_NUM]{};
  int location = 0;
  int day = 0;
  int rank = 0;
  int health = 100;
  int cash = 2000;
  int bank = 0;
  int debt = 1000;
  int pocket = 0;
  int pocket_capacity = 10;
};

std::string money_string(unsigned int value);
