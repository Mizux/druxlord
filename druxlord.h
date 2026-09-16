#pragma once

#include <array>
#include <string>
#include <vector>

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
  bool traded = false;
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

inline constexpr std::array rank_capacity = {10, 25, 100, 600, 3500, 20000};

inline constexpr std::array drug_info = {
    Drug{DrugType::Cocaine, 5100},  Drug{DrugType::Crack, 7000},
    Drug{DrugType::Ecstasy, 3000},  Drug{DrugType::Hashish, 1600},
    Drug{DrugType::Heroin, 7000},   Drug{DrugType::Ice, 3000},
    Drug{DrugType::Kat, 800},       Drug{DrugType::LSD, 1000},
    Drug{DrugType::MDA, 1000},      Drug{DrugType::Morphine, 2000},
    Drug{DrugType::Mushrooms, 400}, Drug{DrugType::Opium, 1500},
    Drug{DrugType::PCP, 800},       Drug{DrugType::Peyote, 1000},
    Drug{DrugType::Pot, 800},       Drug{DrugType::SpecialK, 1500},
    Drug{DrugType::Speed, 800},
};

inline constexpr std::array weapon_info = {
    Weapon{WeaponType::Knife, AmmoType::None, 20, false, false, 100},
    Weapon{WeaponType::Pistol, AmmoType::PistolBullet, 40, false, true, 500},
    Weapon{WeaponType::Shotgun, AmmoType::ShotgunShell, 50, false, true, 2500},
    Weapon{WeaponType::MachineGun, AmmoType::MachineGunBullet, 60, true, true,
           4000},
    Weapon{WeaponType::Flamethrower, AmmoType::GasCanister, 70, true, true,
           7500},
    Weapon{WeaponType::Dynamite, AmmoType::None, 35, true, false, 250},
    Weapon{WeaponType::Grenade, AmmoType::None, 50, true, false, 500},
    Weapon{WeaponType::RocketLauncher, AmmoType::Rocket, 70, true, true, 10000},
    Weapon{WeaponType::AreaDisrupter, AmmoType::EnergyGlobe, 99, true, true,
           500000},
};

inline const std::array shop_items = {
    ShopItem{"knife", "knives", "", "", 35, 1, 0, 1, 0, 100, 0},
    ShopItem{"pistol", "pistols", "pistol bullet", "pistol bullets", 45, 1, 0,
             1, 100, 500, 5},
    ShopItem{"shot gun", "shot guns", "shot gun shell", "shot gun shells", 55,
             2, 0, 1, 100, 2500, 5},
    ShopItem{"machine gun", "machine guns", "machine gun bullet",
             "machine gun bullets", 85, 1, 0, 1, 250, 4000, 5},
    ShopItem{"flame thrower", "flame throwers", "gas canister", "gas canisters",
             75, 8, 0, 1, 10, 7500, 200},
    ShopItem{"", "", "dynamite", "sticks of dynamite", 90, 10, 0, 0, 10, 0,
             250},
    ShopItem{"", "", "hand grenade", "hand grenades", 95, 18, 0, 0, 10, 0, 500},
    ShopItem{"rocket launcher", "rocket launchers", "rocket", "rockets", 80, 35,
             0, 1, 5, 10000, 500},
    ShopItem{"area disrupter", "area disrupters", "energy globe",
             "energy globes", 95, 120, 0, 1, 10, 500000, 25000},
    ShopItem{"heavy leather coat", "heavy leather coats", "", "", 0, 0, 3, 1, 0,
             1000, 0},
    ShopItem{"bullet proof vest", "bullet proof vests", "", "", 0, 0, 15, 1, 0,
             10000, 0},
    ShopItem{"can of no-scent", "cans of no-scent", "", "", 0, 0, 0, 10, 0,
             1000, 0},
};

inline const std::array enemy_info = {
    Enemy{"the drug force", 15, true, 2, 2, 20, 3, 25, 75, 50},
    Enemy{"the police", 15, true, 5, 5, 10, 4, 20, 75, 25},
    Enemy{"a youth gang", 15, false, 20, 25, 7, 3, 20, 20, 10},
    Enemy{"some street toughs", 15, false, 20, 25, 7, 2, 40, 20, 20},
    Enemy{"ATF", 3, true, 1, 5, 15, 3, 30, 75, 35},
    Enemy{"a group of concerned citizens", 7, true, 10, 40, 5, 5, 10, 35, 5},
    Enemy{"a group of wild dogs", 5, true, 0, 60, 5, 5, 15, 0, 0},
    Enemy{"a pack of sewer rats", 3, true, 0, 80, 5, 10, 5, 0, 0},
    Enemy{"the swat team", 2, true, 1, 2, 25, 4, 25, 75, 45},
    Enemy{"airport security", 0, true, 1, 2, 25, 20, 25, 75, 35},
};

inline constexpr std::array city_info = {
    City{CityType::Austin, CountryType::USA, 100, 1861},
    City{CityType::Beijing, CountryType::China, 190, 5307},
    City{CityType::Boston, CountryType::USA, 120, 2509},
    City{CityType::Detroit, CountryType::USA, 80, 1963},
    City{CityType::London, CountryType::UK, 110, 4725},
    City{CityType::LosAngeles, CountryType::USA, 110, 1072},
    City{CityType::Miami, CountryType::USA, 90, 2802},
    City{CityType::Moscow, CountryType::Russia, 160, 5113},
    City{CityType::NewYork, CountryType::USA, 100, 2435},
    City{CityType::Paris, CountryType::France, 90, 4937},
    City{CityType::SanFrancisco, CountryType::USA, 80, 791},
    City{CityType::StPetersburg, CountryType::Russia, 150, 4761},
    City{CityType::Sydney, CountryType::Australia, 110, 7757},
    City{CityType::Toronto, CountryType::Canada, 100, 2091},
    City{CityType::Vancouver, CountryType::Canada, 100, 0},
};

std::string drug_name(DrugType type);
std::string city_name(CityType type);
std::string country_name(CountryType type);
int flight_cost(CityType from_city, CityType to_city);

class GameState {
 public:
  GameState();

  void newgame();
  void stay_here();
  void generate_drug();
  void generate_drug_day(int d);
  void generate_rumors();
  void record_daily_history();
  std::string get_market_news(int loc, int d) const;
  int flight_cost(CityType to_city) const;

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
    for (size_t i = 0; i < player_qty.size(); ++i) total += player_qty[i];
    return total;
  }

  std::vector<std::string> rumors_heard;

  // Player pocket inventory
  std::array<int, drug_info.size()> player_qty{};
  std::array<int, drug_info.size()> player_price{};  // average cost basis

  // Player weapons, armor, items, and ammo
  std::array<int, shop_items.size()> weapon_qty{};
  std::array<int, shop_items.size()> ammo_qty{};

  // Vault inventory
  std::array<int, drug_info.size()> vault_qty{};

  std::array<std::array<std::array<DrugState, DAY_NUM>, city_info.size()>,
             drug_info.size()>
      drug_table{};
  std::array<int, DAY_NUM> cash_history{};
  std::array<int, DAY_NUM> debt_history{};
  std::array<int, DAY_NUM> health_history{};
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
