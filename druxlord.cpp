#include "druxlord.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <random>
#include <string>

std::string drug_name(DrugType type) {
  switch (type) {
    case DrugType::Cocaine:
      return "Cocaine";
    case DrugType::Crack:
      return "Crack";
    case DrugType::Ecstasy:
      return "Ecstasy";
    case DrugType::Hashish:
      return "Hashish";
    case DrugType::Heroin:
      return "Heroin";
    case DrugType::Ice:
      return "Ice";
    case DrugType::Kat:
      return "Kat";
    case DrugType::LSD:
      return "LSD";
    case DrugType::MDA:
      return "MDA";
    case DrugType::Morphine:
      return "Morphine";
    case DrugType::Mushrooms:
      return "Mushrooms";
    case DrugType::Opium:
      return "Opium";
    case DrugType::PCP:
      return "PCP";
    case DrugType::Peyote:
      return "Peyote";
    case DrugType::Pot:
      return "Pot";
    case DrugType::SpecialK:
      return "Special K";
    case DrugType::Speed:
      return "Speed";
  }
  return "Unknown";
}

std::string city_name(CityType type) {
  switch (type) {
    case CityType::Austin:
      return "Austin";
    case CityType::Beijing:
      return "Beijing";
    case CityType::Boston:
      return "Boston";
    case CityType::Detroit:
      return "Detroit";
    case CityType::London:
      return "London";
    case CityType::LosAngeles:
      return "Los Angeles";
    case CityType::Miami:
      return "Miami";
    case CityType::Moscow:
      return "Moscow";
    case CityType::NewYork:
      return "New York";
    case CityType::Paris:
      return "Paris";
    case CityType::SanFrancisco:
      return "San Francisco";
    case CityType::StPetersburg:
      return "St Petersburg";
    case CityType::Sydney:
      return "Sydney";
    case CityType::Toronto:
      return "Toronto";
    case CityType::Vancouver:
      return "Vancouver";
  }
  return "Unknown";
}

std::string country_name(CountryType type) {
  switch (type) {
    case CountryType::Australia:
      return "Australia";
    case CountryType::Canada:
      return "Canada";
    case CountryType::China:
      return "China";
    case CountryType::France:
      return "France";
    case CountryType::UK:
      return "UK";
    case CountryType::USA:
      return "USA";
    case CountryType::Russia:
      return "Russia";
  }
  return "Unknown";
}

const std::array<Drug, DRUG_NUM> drug_info = {{{DrugType::Cocaine, 5100},
                                               {DrugType::Crack, 7000},
                                               {DrugType::Ecstasy, 3000},
                                               {DrugType::Hashish, 1600},
                                               {DrugType::Heroin, 7000},
                                               {DrugType::Ice, 3000},
                                               {DrugType::Kat, 800},
                                               {DrugType::LSD, 1000},
                                               {DrugType::MDA, 1000},
                                               {DrugType::Morphine, 2000},
                                               {DrugType::Mushrooms, 400},
                                               {DrugType::Opium, 1500},
                                               {DrugType::PCP, 800},
                                               {DrugType::Peyote, 1000},
                                               {DrugType::Pot, 800},
                                               {DrugType::SpecialK, 1500},
                                               {DrugType::Speed, 800}}};

const std::array<Weapon, WEAPON_NUM> weapon_info = {
    {{WeaponType::Knife, AmmoType::None, 20, false, false, 100},
     {WeaponType::Pistol, AmmoType::PistolBullet, 40, false, true, 500},
     {WeaponType::Shotgun, AmmoType::ShotgunShell, 50, false, true, 2500},
     {WeaponType::MachineGun, AmmoType::MachineGunBullet, 60, true, true, 4000},
     {WeaponType::Flamethrower, AmmoType::GasCanister, 70, true, true, 7500},
     {WeaponType::Dynamite, AmmoType::None, 35, true, false, 250},
     {WeaponType::Grenade, AmmoType::None, 50, true, false, 500},
     {WeaponType::RocketLauncher, AmmoType::Rocket, 70, true, true, 10000},
     {WeaponType::AreaDisrupter, AmmoType::EnergyGlobe, 99, true, true,
      500000}}};

const std::array<ShopItem, SHOP_ITEM_NUM> shop_items = {{
    {"knife", "knives", "", "", 35, 1, 0, 1, 0, 100, 0},
    {"pistol", "pistols", "pistol bullet", "pistol bullets", 45, 1, 0, 1, 100,
     500, 5},
    {"shot gun", "shot guns", "shot gun shell", "shot gun shells", 55, 2, 0, 1,
     100, 2500, 5},
    {"machine gun", "machine guns", "machine gun bullet", "machine gun bullets",
     85, 1, 0, 1, 250, 4000, 5},
    {"flame thrower", "flame throwers", "gas canister", "gas canisters", 75, 8,
     0, 1, 10, 7500, 200},
    {"", "", "dynamite", "sticks of dynamite", 90, 10, 0, 0, 10, 0, 250},
    {"", "", "hand grenade", "hand grenades", 95, 18, 0, 0, 10, 0, 500},
    {"rocket launcher", "rocket launchers", "rocket", "rockets", 80, 35, 0, 1,
     5, 10000, 500},
    {"area disrupter", "area disrupters", "energy globe", "energy globes", 95,
     120, 0, 1, 10, 500000, 25000},
    {"heavy leather coat", "heavy leather coats", "", "", 0, 0, 3, 1, 0, 1000,
     0},
    {"bullet proof vest", "bullet proof vests", "", "", 0, 0, 15, 1, 0, 10000,
     0},
    {"can of no-scent", "cans of no-scent", "", "", 0, 0, 0, 10, 0, 1000, 0},
}};

const std::array<Enemy, ENEMY_NUM> enemy_info = {{
    {"the drug force", 15, true, 2, 2, 20, 3, 25, 75, 50},
    {"the police", 15, true, 5, 5, 10, 4, 20, 75, 25},
    {"a youth gang", 15, false, 20, 25, 7, 3, 20, 20, 10},
    {"some street toughs", 15, false, 20, 25, 7, 2, 40, 20, 20},
    {"ATF", 3, true, 1, 5, 15, 3, 30, 75, 35},
    {"a group of concerned citizens", 7, true, 10, 40, 5, 5, 10, 35, 5},
    {"a group of wild dogs", 5, true, 0, 60, 5, 5, 15, 0, 0},
    {"a pack of sewer rats", 3, true, 0, 80, 5, 10, 5, 0, 0},
    {"the swat team", 2, true, 1, 2, 25, 4, 25, 75, 45},
    {"airport security", 0, true, 1, 2, 25, 20, 25, 75, 35},
}};

const std::array<City, CITY_NUM> city_info = {
    {{CityType::Austin, CountryType::USA, 100, 1861},
     {CityType::Beijing, CountryType::China, 190, 5307},
     {CityType::Boston, CountryType::USA, 120, 2509},
     {CityType::Detroit, CountryType::USA, 80, 1963},
     {CityType::London, CountryType::UK, 110, 4725},
     {CityType::LosAngeles, CountryType::USA, 110, 1072},
     {CityType::Miami, CountryType::USA, 90, 2802},
     {CityType::Moscow, CountryType::Russia, 160, 5113},
     {CityType::NewYork, CountryType::USA, 100, 2435},
     {CityType::Paris, CountryType::France, 90, 4937},
     {CityType::SanFrancisco, CountryType::USA, 80, 791},
     {CityType::StPetersburg, CountryType::Russia, 150, 4761},
     {CityType::Sydney, CountryType::Australia, 110, 7757},
     {CityType::Toronto, CountryType::Canada, 100, 2091},
     {CityType::Vancouver, CountryType::Canada, 100, 0}}};

int flight_cost(int from_city, int to_city) {
  if (from_city < 0 || from_city >= CITY_NUM || to_city < 0 ||
      to_city >= CITY_NUM) {
    return 0;
  }
  return std::abs(city_info[from_city].distance - city_info[to_city].distance);
}

int GameState::flight_cost(int to_city) const {
  return ::flight_cost(location, to_city);
}

std::string money_string(unsigned int value) {
  std::string s = std::to_string(value);
  int insert_pos = static_cast<int>(s.length()) - 3;
  while (insert_pos > 0) {
    s.insert(static_cast<size_t>(insert_pos), ",");
    insert_pos -= 3;
  }
  return s;
}

GameState::GameState() { newgame(); }

void GameState::newgame() {
  location = 0;
  health = 100;
  day = 0;
  rank = 0;
  cash = 2000;
  bank = 0;
  debt = 1000;
  pocket = 0;
  pocket_capacity = 10;
  for (int i = 0; i < DRUG_NUM; ++i) {
    player_qty[i] = 0;
    player_price[i] = 0;
    vault_qty[i] = 0;
  }
  for (int i = 0; i < SHOP_ITEM_NUM; ++i) {
    weapon_qty[i] = 0;
    ammo_qty[i] = 0;
  }
  for (int i = 0; i < DRUG_NUM; ++i) {
    for (int j = 0; j < CITY_NUM; ++j) {
      for (int d = 0; d < DAY_NUM; ++d) {
        drug_table[i][j][d] = DrugState{};
      }
    }
  }
  for (int d = 0; d < DAY_NUM; ++d) {
    cash_history[d] = 0;
    debt_history[d] = 0;
    health_history[d] = 0;
  }
  rumors_heard.clear();
  generate_drug();
  record_daily_history();
}

Encounter GameState::check_random_encounter() {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  // Day of peace on day 0
  if (day < 1) return {EncounterType::None};

  // 50% chance of random encounter
  if (gen() % 2 != 0) return {EncounterType::None};

  // 15 events weighted according to the original binary
  static const int event_weights[] = {5, 2, 1, 5, 3,  3,  1, 3,
                                      3, 1, 5, 1, 10, 10, 10};
  int total_weight = 63;
  int roll = static_cast<int>(gen() % total_weight);
  int ev = 0;
  int accum = 0;
  for (int i = 0; i < 15; ++i) {
    accum += event_weights[i];
    if (roll < accum) {
      ev = i;
      break;
    }
  }

  // Combat encounters: Events 12, 13, 14
  if (ev >= 12) {
    int total_enemy_weight = 0;
    for (int i = 0; i < ENEMY_NUM - 1; ++i) {
      total_enemy_weight += enemy_info[i].weight;
    }
    int eroll = static_cast<int>(gen() % total_enemy_weight);
    int e_idx = 0;
    accum = 0;
    for (int i = 0; i < ENEMY_NUM - 1; ++i) {
      accum += enemy_info[i].weight;
      if (eroll < accum) {
        e_idx = i;
        break;
      }
    }
    int scale = enemy_info[e_idx].scale;
    int max_add = std::max(1, scale * (rank + 1));
    int count = enemy_info[e_idx].min_count + static_cast<int>(gen() % max_add);

    std::string msg;
    if (ev == 12) {
      msg = "Some days just aren't worth it...";
    } else if (ev == 13) {
      msg = "Did you hear that?";
    } else {
      msg =
          "In the distance you catch sight of something. You try to run, but "
          "it's of no use.";
    }
    return {EncounterType::Combat, msg, e_idx, count};
  }

  if (ev == 0) {
    int d = static_cast<int>(gen() % DRUG_NUM);
    int q = 1 + static_cast<int>(gen() % 5);
    q = std::min(q, pocket_capacity - pocket);
    if (q > 0) {
      player_qty[d] += q;
      pocket += q;
      return {EncounterType::FriendDrug,
              std::format("A friend stops by and gives you {} units of {}!", q,
                          drug_name(drug_info[d].id))};
    }
    return {EncounterType::FriendDrug,
            "A friend stops by to give you some drugs, but your pockets are "
            "completely full!"};
  }

  if (ev == 1) {
    std::vector<int> owned;
    for (int i = 0; i < DRUG_NUM; ++i) {
      if (player_qty[i] > 0) owned.push_back(i);
    }
    if (owned.empty()) return {EncounterType::None};
    int d = owned[gen() % owned.size()];
    int q = std::min(player_qty[d], 1 + static_cast<int>(gen() % 3));
    player_qty[d] -= q;
    pocket -= q;
    return {EncounterType::BrushMissing,
            std::format("You feel someone brush against you and discover that "
                        "{} units of {} are missing!",
                        q, drug_name(drug_info[d].id))};
  }

  if (ev == 2) {
    std::vector<int> owned;
    for (int i = 0; i < DRUG_NUM; ++i) {
      if (player_qty[i] > 0) owned.push_back(i);
    }
    if (owned.empty()) return {EncounterType::None};
    int d = owned[gen() % owned.size()];
    int q = std::min(player_qty[d], 1 + static_cast<int>(gen() % 2));
    player_qty[d] -= q;
    pocket -= q;
    return {
        EncounterType::FakeDrug,
        std::format("Close inspection reveals that {} units of {} are fake!", q,
                    drug_name(drug_info[d].id))};
  }

  if (ev == 3) {
    int d = static_cast<int>(gen() % DRUG_NUM);
    int q = 1 + static_cast<int>(gen() % 5);
    q = std::min(q, pocket_capacity - pocket);
    if (q > 0) {
      player_qty[d] += q;
      pocket += q;
      return {
          EncounterType::DeadBodyDrug,
          std::format("You stumble across a dead body and find {} units of {}!",
                      q, drug_name(drug_info[d].id))};
    }
    return {EncounterType::DeadBodyDrug,
            "You stumble across a dead body with drugs, but your pockets are "
            "completely full!"};
  }

  if (ev == 4) {
    int amount =
        (1 + static_cast<int>(gen() % 5)) * 100 + static_cast<int>(gen() % 100);
    cash += amount;
    return {
        EncounterType::PurseCash,
        std::format("You find a woman's purse in a trash can. Inside is ${}!",
                    money_string(amount))};
  }

  if (ev == 5) {
    int amount =
        (1 + static_cast<int>(gen() % 5)) * 100 + static_cast<int>(gen() % 100);
    cash += amount;
    return {
        EncounterType::WalletCash,
        std::format("You find a wallet with ${} in it!", money_string(amount))};
  }

  if (ev == 6) {
    if (cash <= 100) return {EncounterType::None};
    int fee = std::min(cash, static_cast<int>((1 + (gen() % 4)) * 100));
    cash -= fee;
    return {EncounterType::RatFee,
            std::format("Your home is infested with rats! You have to pay an "
                        "exterminator ${} to get rid of them!",
                        money_string(fee))};
  }

  if (ev == 7) {
    if (cash <= 100) return {EncounterType::None};
    int stolen = std::min(cash, static_cast<int>((1 + (gen() % 5)) * 100));
    cash -= stolen;
    return {EncounterType::Mugged,
            std::format("You have been mugged! He takes ${} and runs!",
                        money_string(stolen))};
  }

  if (ev == 8) {
    if (cash <= 100) return {EncounterType::None};
    int lifted = std::min(cash, static_cast<int>((1 + (gen() % 4)) * 100));
    cash -= lifted;
    return {EncounterType::SubwayFee,
            std::format(
                "You get off the subway and find ${} has been lifted from you!",
                money_string(lifted))};
  }

  if (ev == 9) {
    std::vector<int> owned;
    for (int i = 0; i < DRUG_NUM; ++i) {
      if (player_qty[i] > 0) owned.push_back(i);
    }
    if (owned.empty()) return {EncounterType::None};
    int d = owned[gen() % owned.size()];
    int q = player_qty[d];
    player_qty[d] = 0;
    pocket -= q;
    return {
        EncounterType::AddictDemand,
        std::format("A {} addict jumps you and demands all your {}! You "
                    "have no choice but to hand it over.",
                    drug_name(drug_info[d].id), drug_name(drug_info[d].id))};
  }

  if (ev == 10) {
    int idx = static_cast<int>(gen() % 7);
    if (idx == 5 || idx == 6) {
      ammo_qty[idx] = std::min(shop_items[idx].ammo_limit, ammo_qty[idx] + 2);
      return {EncounterType::BodyWeapon,
              std::format("You trip over a body. You search it and find 2 {}!",
                          shop_items[idx].ammo_plural)};
    } else {
      weapon_qty[idx] = 1;
      if (shop_items[idx].ammo_limit > 0) {
        ammo_qty[idx] =
            std::min(shop_items[idx].ammo_limit, ammo_qty[idx] + 15);
      }
      return {EncounterType::BodyWeapon,
              std::format("You trip over a body. You search it and find a {}!",
                          shop_items[idx].name)};
    }
  }

  if (ev == 11) {
    std::vector<int> owned;
    for (int i = 0; i < 8; ++i) {
      if (weapon_qty[i] > 0) owned.push_back(i);
    }
    if (owned.empty()) return {EncounterType::None};
    int w = owned[gen() % owned.size()];
    weapon_qty[w] = 0;
    return {EncounterType::WeaponBreak,
            std::format("OH MAN! You are cleaning your {} and it falls apart!",
                        shop_items[w].name)};
  }

  return {EncounterType::None};
}

void GameState::generate_drug() {
  generate_drug_day(0);
  generate_rumors();
}

void GameState::generate_drug_day(int d) {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  int cap = (rank >= 0 && rank < RANK_NUM) ? rank_capacity[rank] : 10;

  for (int j = 0; j < CITY_NUM; ++j) {
    int city_factor = city_info[j].price_factor;
    for (int i = 0; i < DRUG_NUM; ++i) {
      int base_price = drug_info[i].price;
      int mean = (base_price * city_factor) / 100;
      int half = mean / 2;
      int min_price = mean - half;

      if (d == 0) {
        // Day 0: Initial target and current price within [Half, Half + Mean -
        // 1]
        int target_price = half + static_cast<int>(gen() % mean);
        int current_price = half + static_cast<int>(gen() % mean);

        double price_ratio = static_cast<double>(current_price - min_price) /
                             static_cast<double>(mean);
        double target_qty = cap * (1.0 - price_ratio);
        int qty = std::max(0, static_cast<int>(target_qty));

        int event_flag = 0;
        if (gen() % 50 == 0) {
          if (gen() % 2 == 0) {
            // Price Spike (+1)
            event_flag = 1;
            int multiplier = static_cast<int>(gen() % 5) + 5;
            current_price =
                (mean + static_cast<int>(gen() % half)) * multiplier;
            int reduction = static_cast<int>(gen() % 5) + 2;
            qty = qty / reduction;
          } else {
            // Price Crash (-1)
            event_flag = -1;
            int divisor = static_cast<int>(gen() % 5) + 5;
            current_price = (mean - static_cast<int>(gen() % half)) / divisor;
            int flood = static_cast<int>(gen() % 5) + 2;
            qty = qty * flood;
          }
        }

        drug_table[i][j][0].qty = qty;
        drug_table[i][j][0].price = current_price;
        drug_table[i][j][0].target_price = target_price;
        drug_table[i][j][0].event_flag = event_flag;
        drug_table[i][j][0].rumor_flag = 0;
        drug_table[i][j][0].available = (qty > 0);
      } else {
        const auto& prev = drug_table[i][j][d - 1];

        // Step 2: Price Drift Towards Dynamic Target
        int drift_current_price = prev.price;
        int drift_target_price = prev.target_price;
        int delta = drift_target_price - drift_current_price;
        int step = 0;
        if (delta > 0) {
          step = static_cast<int>(gen() % delta);
        } else if (delta < 0) {
          step = -static_cast<int>(gen() % std::abs(delta));
        }
        int new_current_price = drift_current_price + step;

        // Step 3: Target Regeneration
        int new_delta = drift_target_price - new_current_price;
        int threshold = (new_current_price * 10) / 100;
        int new_target_price = drift_target_price;
        if (std::abs(new_delta) < threshold) {
          new_target_price = half + static_cast<int>(gen() % mean);
        }

        // Step 4: Supply & Demand Curve
        double new_price_ratio =
            static_cast<double>(new_current_price - min_price) /
            static_cast<double>(mean);
        double new_target_qty = cap * (1.0 - new_price_ratio);
        int new_qty =
            std::max(0, static_cast<int>((new_target_qty + prev.qty) / 2.0));

        // Step 5: Market Events (Spikes and Crashes)
        int day_event_flag = drug_table[i][j][d].event_flag;
        int day_rumor_flag = drug_table[i][j][d].rumor_flag;

        if (day_event_flag == 1) {
          int multiplier = static_cast<int>(gen() % 5) + 5;
          new_current_price =
              (mean + static_cast<int>(gen() % half)) * multiplier;
          int reduction = static_cast<int>(gen() % 5) + 2;
          new_qty = new_qty / reduction;
        } else if (day_event_flag == -1) {
          int divisor = static_cast<int>(gen() % 5) + 5;
          new_current_price = (mean - static_cast<int>(gen() % half)) / divisor;
          int flood = static_cast<int>(gen() % 5) + 2;
          new_qty = new_qty * flood;
        } else {
          if (gen() % 50 == 0) {
            if (gen() % 2 == 0) {
              day_event_flag = 1;
              int multiplier = static_cast<int>(gen() % 5) + 5;
              new_current_price =
                  (mean + static_cast<int>(gen() % half)) * multiplier;
              int reduction = static_cast<int>(gen() % 5) + 2;
              new_qty = new_qty / reduction;
            } else {
              day_event_flag = -1;
              int divisor = static_cast<int>(gen() % 5) + 5;
              new_current_price =
                  (mean - static_cast<int>(gen() % half)) / divisor;
              int flood = static_cast<int>(gen() % 5) + 2;
              new_qty = new_qty * flood;
            }
          }
        }

        drug_table[i][j][d].qty = new_qty;
        drug_table[i][j][d].price = new_current_price;
        drug_table[i][j][d].target_price = new_target_price;
        drug_table[i][j][d].event_flag = day_event_flag;
        drug_table[i][j][d].rumor_flag = day_rumor_flag;
        drug_table[i][j][d].available = (new_qty > 0);
      }
    }
  }
}

void GameState::generate_rumors() {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  rumors_heard.clear();
  if (day >= DAY_NUM - 1) return;

  // Local rumor (1 in 3 chance)
  if (gen() % 3 == 0) {
    int drug_idx = static_cast<int>(gen() % DRUG_NUM);
    int r = static_cast<int>(gen() % 10);
    int r_flag = (r <= 6) ? 1 : -1;
    int base_event = (r <= 6) ? 1 : (r == 9 ? -1 : 0);

    if (gen() % 2 == 0) {
      int event_flag = base_event;
      drug_table[drug_idx][location][day + 1].event_flag = event_flag;
      drug_table[drug_idx][location][day + 1].rumor_flag = r_flag;
      rumors_heard.push_back(
          std::format("You hear a rumor that {} will be scarce tomorrow.",
                      drug_name(drug_info[drug_idx].id)));
    } else {
      int event_flag = -base_event;
      drug_table[drug_idx][location][day + 1].event_flag = event_flag;
      drug_table[drug_idx][location][day + 1].rumor_flag = r_flag;
      rumors_heard.push_back(
          std::format("You hear a rumor that {} will be abundant tomorrow.",
                      drug_name(drug_info[drug_idx].id)));
    }
  }

  // Remote rumor (1 in 3 chance)
  if (gen() % 3 == 0) {
    int other_city = static_cast<int>(gen() % (CITY_NUM - 1));
    if (other_city >= location) other_city++;

    int drug_idx = static_cast<int>(gen() % DRUG_NUM);
    int r = static_cast<int>(gen() % 10);
    int r_flag = (r <= 6) ? 1 : -1;
    int base_event = (r <= 6) ? 1 : (r == 9 ? -1 : 0);

    if (gen() % 2 == 0) {
      int event_flag = base_event;
      drug_table[drug_idx][other_city][day + 1].event_flag = event_flag;
      drug_table[drug_idx][other_city][day + 1].rumor_flag = r_flag;
      rumors_heard.push_back(
          std::format("You hear a rumor that {} will be scarce in {} tomorrow.",
                      drug_name(drug_info[drug_idx].id),
                      city_name(city_info[other_city].id)));
    } else {
      int event_flag = -base_event;
      drug_table[drug_idx][other_city][day + 1].event_flag = event_flag;
      drug_table[drug_idx][other_city][day + 1].rumor_flag = r_flag;
      rumors_heard.push_back(std::format(
          "You hear a rumor that {} will be abundant in {} tomorrow.",
          drug_name(drug_info[drug_idx].id),
          city_name(city_info[other_city].id)));
    }
  }
}

void GameState::record_daily_history() {
  if (day >= 0 && day < DAY_NUM) {
    cash_history[day] = cash;
    debt_history[day] = debt;
    health_history[day] = health;
  }
}

void GameState::stay_here() {
  record_daily_history();
  if (day < DAY_NUM - 1) {
    day += 1;
    if (debt > 0) {
      debt = debt + (debt * 10) / 100;
    }
    if (bank > 0) {
      bank = bank + (bank * 5) / 100;
    }
    int net_worth = cash + bank - debt;
    if (net_worth >= 50000000 && rank < 5) {
      rank = 5;
    } else if (net_worth >= 10000000 && rank < 4) {
      rank = 4;
    } else if (net_worth >= 1000000 && rank < 3) {
      rank = 3;
    } else if (net_worth >= 100000 && rank < 2) {
      rank = 2;
    } else if (net_worth >= 20000 && rank < 1) {
      rank = 1;
    }
    pocket_capacity = rank_capacity[rank];
    generate_drug_day(day);
    generate_rumors();
    record_daily_history();
  }
}

std::string GameState::get_market_news(int loc, int d) const {
  std::string news;

  // 1. Rumor resolutions for current location on day d
  for (int i = 0; i < DRUG_NUM; ++i) {
    int r_flag = drug_table[i][loc][d].rumor_flag;
    if (r_flag != 0) {
      if (!news.empty()) news += "\n\n";
      news += std::format("The {} rumor was {}!", drug_name(drug_info[i].id),
                          r_flag == 1 ? "true" : "false");
    }
  }

  // 2. Event headlines and reasons for location on day d
  auto format_spike_reason = [](size_t idx, const std::string& name) {
    switch (idx) {
      case 0:
        return std::format(
            "The pilot of a {} shipment fell asleep and crashed.", name);
      case 1:
        return std::format(
            "The smuggler of some {} was killed by a rival dealer.", name);
      case 2:
        return std::format(
            "Cops burst into a {} warehouse, seizing everything.", name);
      case 3:
        return std::format(
            "Racoons broke into a crate of {}, eating some and dying on the "
            "rest.",
            name);
      default:
        return std::format("Gang warfare is keeping {} off the streets.", name);
    }
  };

  static const std::string spike_headlines[] = {
      "Prices are higher than the people who use your goods!",
      "Prices go through the roof!", "Prices are outrageous!",
      "Prices are insanely high!", "Prices are astronomical!"};

  auto format_crash_reason = [](size_t idx, const std::string& name) {
    switch (idx) {
      case 0:
        return std::format(
            "Crates of {} were discovered floating in the ocean.", name);
      case 1:
        return std::format("A new {} dealer has arrived in town.", name);
      case 2:
        return std::format("A new source of {} is found.", name);
      case 3:
        return std::format(
            "A police warehouse is broken into and {} is stolen.", name);
      default:
        return std::format("A boatload of {} arrives.", name);
    }
  };

  static const std::string crash_headlines[] = {
      "Prices are rock bottom!", "Prices drop like lead balloons!",
      "Prices plummet!", "Prices nose dive!",
      "Prices are lower than the Marianas Trench!"};

  for (int i = 0; i < DRUG_NUM; ++i) {
    int e_flag = drug_table[i][loc][d].event_flag;
    if (e_flag == 1) {
      if (!news.empty()) news += "\n\n";
      size_t r_idx = (i + loc + d) % 5;
      size_t h_idx = (i * 2 + loc + d) % 5;
      news +=
          std::format("{}\n{}", spike_headlines[h_idx],
                      format_spike_reason(r_idx, drug_name(drug_info[i].id)));
    } else if (e_flag == -1) {
      if (!news.empty()) news += "\n\n";
      size_t r_idx = (i + loc + d) % 5;
      size_t h_idx = (i * 2 + loc + d) % 5;
      news +=
          std::format("{}\n{}", crash_headlines[h_idx],
                      format_crash_reason(r_idx, drug_name(drug_info[i].id)));
    }
  }

  // 3. Rumors heard today for tomorrow (only shown in player's current
  // location)
  if (loc == location) {
    for (const auto& rumor : rumors_heard) {
      if (!news.empty()) news += "\n\n";
      news += rumor;
    }
  }

  // 4. Last day warning
  if (d >= DAY_NUM - 1) {
    if (!news.empty()) news += "\n\n";
    news += "This is the last day! Better sell all you can!";
  }

  return news;
}
