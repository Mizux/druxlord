#include "druxlord.h"

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
  4000,
  4000,
  3000,
  2000,
  1000,
  1000,
  1000,
  1000,
  1000,
  3000,
  200,
  900,
  900,
  900,
  800,
  600,
  500
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
  { CityType::Austin,        CountryType::USA       },
  { CityType::Beijing,       CountryType::China     },
  { CityType::Boston,        CountryType::USA       },
  { CityType::Detroit,       CountryType::USA       },
  { CityType::London,        CountryType::UK        },
  { CityType::LosAngeles,    CountryType::USA       },
  { CityType::Miami,         CountryType::USA       },
  { CityType::Moscow,        CountryType::Russia    },
  { CityType::NewYork,       CountryType::USA       },
  { CityType::Paris,         CountryType::France    },
  { CityType::SanFrancisco,  CountryType::USA       },
  { CityType::StPetersburg,  CountryType::Russia    },
  { CityType::Sydney,        CountryType::Australia },
  { CityType::Toronto,       CountryType::Canada    },
  { CityType::Vancouver,     CountryType::Canada    }
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
  std::bernoulli_distribution bool_dist(0.5);
  std::uniform_int_distribution<int> qty_dist(0, 9);
  std::uniform_int_distribution<int> divisor_dist(1, 9);
  std::uniform_real_distribution<double> range_dist(0.1, 1.0);

  for (int i = 0; i < DRUG_NUM; ++i) {
    for (int j = 0; j < CITY_NUM; ++j) {
      for (int d = 0; d < DAY_NUM; ++d) {
        drug_table[i][j][d].available = bool_dist(gen);
        if (drug_table[i][j][d].available) {
          drug_table[i][j][d].qty = qty_dist(gen);
          int divisor = (drug_table[i][j][d].qty > 0) ? drug_table[i][j][d].qty
                                                      : divisor_dist(gen);
          drug_table[i][j][d].price = static_cast<int>(
              (10.0 / divisor) * range_dist(gen) * drug_price[i]);
        } else {
          drug_table[i][j][d].qty = 0;
          drug_table[i][j][d].price = drug_price[i];
        }
      }
    }
  }
}
