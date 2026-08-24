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
  { WEAPON_KNIFE,           0,                     20, false, false, 100    },
  { WEAPON_PISTOL,          AMMO_PISTOLBULLET,     40, false, true,  500    },
  { WEAPON_SHOTGUN,         AMMO_SHOTGUNSHELL,     50, false, true,  2500   },
  { WEAPON_MACHINEGUN,      AMMO_MACHINEGUNBULLET, 60, true,  true,  4000   },
  { WEAPON_FLAMETHROWER,    AMMO_GASCANISTER,      70, true,  true,  7500   },
  { WEAPON_DYNAMITE,        0,                     35, true,  false, 250    },
  { WEAPON_GRENADE,         0,                     50, true,  false, 500    },
  { WEAPON_ROCKETLAUNCHER,  AMMO_ROCKET,           70, true,  true,  10000  },
  { WEAPON_AREADISRUPTER,   AMMO_ENERGYGLOBE,      99, true,  true,  500000 }
}};

const std::array<City, CITY_NUM> city_info = {{
  { CITY_AUSTIN,        COUNTRY_USA       },
  { CITY_BEIJING,       COUNTRY_CHINA     },
  { CITY_BOSTON,        COUNTRY_USA       },
  { CITY_DETROIT,       COUNTRY_USA       },
  { CITY_LONDON,        COUNTRY_UK        },
  { CITY_LOSANGELES,    COUNTRY_USA       },
  { CITY_MIAMI,         COUNTRY_USA       },
  { CITY_MOSCOW,        COUNTRY_RUSSIA    },
  { CITY_NEWYORK,       COUNTRY_USA       },
  { CITY_PARIS,         COUNTRY_FRANCE    },
  { CITY_SANFRANCISCO,  COUNTRY_USA       },
  { CITY_STPETERSBURG,  COUNTRY_RUSSIA    },
  { CITY_SYDNEY,        COUNTRY_AUSTRALIA },
  { CITY_TORONTO,       COUNTRY_CANADA    },
  { CITY_VANCOUVER,     COUNTRY_CANADA    }
}};

GSList *city_list = nullptr;
Drug drug_table[DRUG_NUM][CITY_NUM];

std::string money_string(unsigned int value) {
  std::string s = std::to_string(value);
  int insert_pos = static_cast<int>(s.length()) - 3;
  while (insert_pos > 0) {
    s.insert(static_cast<size_t>(insert_pos), ",");
    insert_pos -= 3;
  }
  return s;
}

void generate_drug() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::bernoulli_distribution bool_dist(0.5);
  std::uniform_int_distribution<int> qty_dist(0, 9);
  std::uniform_int_distribution<int> divisor_dist(1, 9);
  std::uniform_real_distribution<double> range_dist(0.1, 1.0);

  for (int i = 0; i < DRUG_NUM; ++i) {
    for (int j = 0; j < CITY_NUM; ++j) {
      drug_table[i][j].available = bool_dist(gen);
      if (drug_table[i][j].available) {
        drug_table[i][j].qty = qty_dist(gen);
        int divisor = (drug_table[i][j].qty > 0) ? drug_table[i][j].qty : divisor_dist(gen);
        drug_table[i][j].price = static_cast<int>((10.0 / divisor) * range_dist(gen) * drug_price[i]);
      }
    }
  }
}
