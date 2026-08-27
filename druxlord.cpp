#include "druxlord.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <random>
#include <string>

const std::array<const char*, DRUG_NUM> drug_name = {
  "Cocaine",
  "Crack",
  "Ecstasy",
  "Hashish",
  "Heroin",
  "Ice",
  "Kat",
  "LSD",
  "MDA",
  "Morphine",
  "Mushrooms",
  "Opium",
  "PCP",
  "Peyote",
  "Pot",
  "Special K",
  "Speed"
};

const std::array<int, DRUG_NUM> drug_price = {
  5100, // Cocaine
  7000, // Crack
  3000, // Ecstasy
  1600, // Hashish
  7000, // Heroin
  3000, // Ice
  800,  // Kat
  1000, // LSD
  1000, // MDA
  2000, // Morphine
  400,  // Mushrooms
  1500, // Opium
  800,  // PCP
  1000, // Peyote
  800,  // Pot
  1500, // Special K
  800   // Speed
};

const std::array<const char*, CITY_NUM> city_name = {
  "Austin",
  "Beijing",
  "Boston",
  "Detroit",
  "London",
  "Los Angeles",
  "Miami",
  "Moscow",
  "New York",
  "Paris",
  "San Francisco",
  "St Petersburg",
  "Sydney",
  "Toronto",
  "Vancouver"
};

const std::array<const char*, COUNTRY_NUM> country_name = {
  "Australia",
  "Canada",
  "China",
  "England",
  "France",
  "USA",
  "Russia"
};

const std::array<Weapon, WEAPON_NUM> weapon_info = {{
  { WeaponType::Knife,           AmmoType::None,              20, false, false, 100    },
  { WeaponType::Pistol,          AmmoType::PistolBullet,     40, false, true,  500    },
  { WeaponType::Shotgun,         AmmoType::ShotgunShell,     50, false, true,  2500   },
  { WeaponType::MachineGun,      AmmoType::MachineGunBullet, 60, true,  true,  4000   },
  { WeaponType::Flamethrower,    AmmoType::GasCanister,      70, true,  true,  7500   },
  { WeaponType::Dynamite,        AmmoType::None,              35, true,  false, 250    },
  { WeaponType::Grenade,         AmmoType::None,              50, true,  false, 500    },
  { WeaponType::RocketLauncher,  AmmoType::Rocket,           70, true,  true,  10000  },
  { WeaponType::AreaDisrupter,   AmmoType::EnergyGlobe,      99, true,  true,  500000 }
}};

const std::array<City, CITY_NUM> city_info = {{
  { CityType::Austin,        CountryType::USA,       100 },
  { CityType::Beijing,       CountryType::China,     190 },
  { CityType::Boston,        CountryType::USA,       120 },
  { CityType::Detroit,       CountryType::USA,        80 },
  { CityType::London,        CountryType::UK,        110 },
  { CityType::LosAngeles,    CountryType::USA,       110 },
  { CityType::Miami,         CountryType::USA,        90 },
  { CityType::Moscow,        CountryType::Russia,    160 },
  { CityType::NewYork,       CountryType::USA,       100 },
  { CityType::Paris,         CountryType::France,     90 },
  { CityType::SanFrancisco,  CountryType::USA,        80 },
  { CityType::StPetersburg,  CountryType::Russia,    150 },
  { CityType::Sydney,        CountryType::Australia, 110 },
  { CityType::Toronto,       CountryType::Canada,    100 },
  { CityType::Vancouver,     CountryType::Canada,    100 }
}};

std::string money_string(unsigned int value) {
  std::string s = std::to_string(value);
  int insert_pos = static_cast<int>(s.length()) - 3;
  while (insert_pos > 0) {
    s.insert(static_cast<size_t>(insert_pos), ",");
    insert_pos -= 3;
  }
  return s;
}

GameState::GameState() {
  generate_drug();
}

void GameState::generate_drug() {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  int cap = (rank >= 0 && rank < RANK_NUM) ? rank_capacity[rank] : 10;

  for (int j = 0; j < CITY_NUM; ++j) {
    int city_factor = city_info[j].price_factor;
    for (int i = 0; i < DRUG_NUM; ++i) {
      int base_price = drug_price[i];
      int mean = (base_price * city_factor) / 100;
      int half = mean / 2;
      int min_price = mean - half;

      // Day 0: Initial target and current price within [Half, Half + Mean - 1]
      int target_price = half + static_cast<int>(gen() % mean);
      int current_price = half + static_cast<int>(gen() % mean);

      double price_ratio = static_cast<double>(current_price - min_price) / static_cast<double>(mean);
      double target_qty = cap * (1.0 - price_ratio);
      int qty = std::max(0, static_cast<int>(target_qty));

      int event_flag = 0;
      if (gen() % 50 == 0) {
        if (gen() % 2 == 0) {
          // Price Spike (+1)
          event_flag = 1;
          int multiplier = static_cast<int>(gen() % 5) + 5;
          current_price = (mean + static_cast<int>(gen() % half)) * multiplier;
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

      // Subsequent days (1 to DAY_NUM - 1)
      for (int d = 1; d < DAY_NUM; ++d) {
        const auto &prev = drug_table[i][j][d - 1];

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
        double new_price_ratio = static_cast<double>(new_current_price - min_price) / static_cast<double>(mean);
        double new_target_qty = cap * (1.0 - new_price_ratio);
        int new_qty = std::max(0, static_cast<int>((new_target_qty + prev.qty) / 2.0));

        // Step 5: Market Events (Spikes and Crashes)
        int day_event_flag = 0;
        if (gen() % 50 == 0) {
          if (gen() % 2 == 0) {
            // Price Spike (+1)
            day_event_flag = 1;
            int multiplier = static_cast<int>(gen() % 5) + 5;
            new_current_price = (mean + static_cast<int>(gen() % half)) * multiplier;
            int reduction = static_cast<int>(gen() % 5) + 2;
            new_qty = new_qty / reduction;
          } else {
            // Price Crash (-1)
            day_event_flag = -1;
            int divisor = static_cast<int>(gen() % 5) + 5;
            new_current_price = (mean - static_cast<int>(gen() % half)) / divisor;
            int flood = static_cast<int>(gen() % 5) + 2;
            new_qty = new_qty * flood;
          }
        }

        drug_table[i][j][d].qty = new_qty;
        drug_table[i][j][d].price = new_current_price;
        drug_table[i][j][d].target_price = new_target_price;
        drug_table[i][j][d].event_flag = day_event_flag;
        drug_table[i][j][d].rumor_flag = 0;
        drug_table[i][j][d].available = (new_qty > 0);
      }
    }
  }
}

void GameState::stay_here() {
  if (day < DAY_NUM - 1) {
    day += 1;
  }
}

std::string GameState::get_market_news(int loc, int d) const {
  std::string news;
  for (int i = 0; i < DRUG_NUM; ++i) {
    if (drug_table[i][loc][d].event_flag == 1) {
      if (!news.empty()) news += "\n\n";
      news += std::format("Prices go through the roof!\nCops burst into a {} warehouse, seizing everything.", drug_name[i]);
    } else if (drug_table[i][loc][d].event_flag == -1) {
      if (!news.empty()) news += "\n\n";
      news += std::format("Prices plummet!\nCrates of {} were discovered floating in the ocean.", drug_name[i]);
    }
  }
  return news;
}
