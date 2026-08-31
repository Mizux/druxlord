#pragma once

#include <array>
#include <string>
#include <vector>

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

struct ShopItem {
  std::string name;
  std::string plural;
  std::string ammo_name;
  std::string ammo_plural;
  int hit_chance;
  int damage;
  int armor;
  int weapon_limit;
  int ammo_limit;
  int weapon_price;
  int ammo_price;
};

inline constexpr int SHOP_ITEM_NUM = 12;

struct Enemy {
  std::string name;
  int weight;
  bool can_bribe;
  int flee_chance;
  int min_count;
  int scale;
  int weapon_idx;
  int health;
  int surrender_accept;
  int accuracy;
};

inline constexpr int ENEMY_NUM = 10;

enum class EncounterType {
  None,
  FriendDrug,
  BrushMissing,
  FakeDrug,
  DeadBodyDrug,
  PurseCash,
  WalletCash,
  RatFee,
  Mugged,
  SubwayFee,
  AddictDemand,
  BodyWeapon,
  WeaponBreak,
  Combat
};

struct Encounter {
  EncounterType type = EncounterType::None;
  std::string message;
  int enemy_idx = -1;
  int enemy_count = 0;
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
extern const std::array<ShopItem, SHOP_ITEM_NUM> shop_items;
extern const std::array<Enemy, ENEMY_NUM> enemy_info;
extern const std::array<City, CITY_NUM> city_info;

class GameState {
 public:
  GameState();

  void newgame();
  void stay_here();
  void generate_drug();
  void generate_drug_day(int d);
  void generate_rumors();
  std::string get_market_news(int loc, int d) const;
  int flight_cost(int to_city) const;

  Encounter check_random_encounter();

  int total_armor() const {
    int arm = 0;
    if (weapon_qty[9] > 0) arm += shop_items[9].armor;  // Heavy leather coat: 3
    if (weapon_qty[10] > 0)
      arm += shop_items[10].armor;  // Bullet proof vest: 15
    return arm;
  }

  int no_scent_cans() const { return weapon_qty[11]; }

  int total_drugs() const {
    int total = 0;
    for (int i = 0; i < DRUG_NUM; ++i) total += player_qty[i];
    return total;
  }

  std::vector<std::string> rumors_heard;

  // Player pocket inventory
  int player_qty[DRUG_NUM]{};
  int player_price[DRUG_NUM]{};  // average cost basis

  // Player weapons, armor, items, and ammo
  int weapon_qty[SHOP_ITEM_NUM]{};
  int ammo_qty[SHOP_ITEM_NUM]{};

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
