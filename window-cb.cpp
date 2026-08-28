#include "window-cb.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <algorithm>
#include <format>
#include <string>

#include "config.h"
#include "druxlord.h"
#include "window.h"

void window_main_button_buy_clicked_cb() {
  if (!window_main.game_state || !window_main.treeview_market) return;
  auto *item = window_main.treeview_market->currentItem();
  if (!item) {
    if (window_main.treeview_market->topLevelItemCount() > 0) {
      item = window_main.treeview_market->topLevelItem(0);
      window_main.treeview_market->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(COLUMN_NAME, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;

  int j = window_main.game_state->location;
  int d = window_main.game_state->day;
  int price = window_main.game_state->drug_table[drug_idx][j][d].price;
  int available_qty = window_main.game_state->drug_table[drug_idx][j][d].qty;
  int max_afford = (price > 0) ? (window_main.game_state->cash / price) : 0;
  int space = window_main.game_state->pocket_capacity - window_main.game_state->pocket;
  int max_buy = std::min(available_qty, std::min(max_afford, space));

  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Buying {}", name);
  std::string message = std::format("{} is currently selling for ${} per unit. With your available funds, you can buy {}.",
                                    name, money_string(price), max_buy);

  create_window_input(title.c_str(), message.c_str(), "How many to buy?");
  window_input.spinbutton_value->setRange(1, std::max(1, max_buy));
  window_input.spinbutton_value->setValue(std::max(1, max_buy));
  if (max_buy <= 0) {
    window_input.button_ok->setEnabled(false);
  }

  QObject::connect(window_input.button_ok, &QPushButton::clicked, [drug_idx, price, max_buy, j, d]() {
    int count = window_input.spinbutton_value->value();
    if (count > 0 && count <= max_buy && window_main.game_state) {
      int cost = count * price;
      window_main.game_state->cash -= cost;
      window_main.game_state->pocket += count;
      int prev_qty = window_main.game_state->player_qty[drug_idx];
      int prev_price = window_main.game_state->player_price[drug_idx];
      window_main.game_state->player_price[drug_idx] = (prev_price * prev_qty + cost) / (prev_qty + count);
      window_main.game_state->player_qty[drug_idx] += count;
      window_main.game_state->drug_table[drug_idx][j][d].qty -= count;
      if (window_main.game_state->drug_table[drug_idx][j][d].qty <= 0) {
        window_main.game_state->drug_table[drug_idx][j][d].available = false;
      }
      window_input.window->close();
      update_all_ui(*window_main.game_state);
    }
  });

  window_input.window->show();
  window_input.window->raise();
  window_input.window->activateWindow();
}

void window_main_button_sell_clicked_cb() {
  if (!window_main.game_state || !window_main.treeview_pocket) return;
  auto *item = window_main.treeview_pocket->currentItem();
  if (!item) {
    if (window_main.treeview_pocket->topLevelItemCount() > 0) {
      item = window_main.treeview_pocket->topLevelItem(0);
      window_main.treeview_pocket->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;
  int owned_qty = window_main.game_state->player_qty[drug_idx];
  if (owned_qty <= 0) return;

  int j = window_main.game_state->location;
  int d = window_main.game_state->day;
  int price = window_main.game_state->drug_table[drug_idx][j][d].price;
  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Selling {}", name);
  std::string message = std::format("{} is currently being bought for ${} per unit. You have {} to sell.",
                                    name, money_string(price), owned_qty);

  create_window_input(title.c_str(), message.c_str(), "How many to sell?");
  window_input.spinbutton_value->setRange(1, owned_qty);
  window_input.spinbutton_value->setValue(owned_qty);

  QObject::connect(window_input.button_ok, &QPushButton::clicked, [drug_idx, price, owned_qty]() {
    int count = window_input.spinbutton_value->value();
    if (count > 0 && count <= owned_qty && window_main.game_state) {
      int gain = count * price;
      window_main.game_state->cash += gain;
      window_main.game_state->pocket -= count;
      window_main.game_state->player_qty[drug_idx] -= count;
      if (window_main.game_state->player_qty[drug_idx] == 0) {
        window_main.game_state->player_price[drug_idx] = 0;
      }
      window_input.window->close();
      update_all_ui(*window_main.game_state);
    }
  });

  window_input.window->show();
  window_input.window->raise();
  window_input.window->activateWindow();
}

void window_main_button_dump_clicked_cb() {
  if (!window_main.game_state || !window_main.treeview_pocket) return;
  auto *item = window_main.treeview_pocket->currentItem();
  if (!item) {
    if (window_main.treeview_pocket->topLevelItemCount() > 0) {
      item = window_main.treeview_pocket->topLevelItem(0);
      window_main.treeview_pocket->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;
  int owned_qty = window_main.game_state->player_qty[drug_idx];
  if (owned_qty <= 0) return;

  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Dumping {}", name);
  std::string message = std::format("You have {} {} in your pocket to dump.", owned_qty, name);

  create_window_input(title.c_str(), message.c_str(), "How many to dump?");
  window_input.spinbutton_value->setRange(1, owned_qty);
  window_input.spinbutton_value->setValue(owned_qty);

  QObject::connect(window_input.button_ok, &QPushButton::clicked, [drug_idx, owned_qty]() {
    int count = window_input.spinbutton_value->value();
    if (count > 0 && count <= owned_qty && window_main.game_state) {
      window_main.game_state->pocket -= count;
      window_main.game_state->player_qty[drug_idx] -= count;
      if (window_main.game_state->player_qty[drug_idx] == 0) {
        window_main.game_state->player_price[drug_idx] = 0;
      }
      window_input.window->close();
      update_all_ui(*window_main.game_state);
    }
  });

  window_input.window->show();
  window_input.window->raise();
  window_input.window->activateWindow();
}

// Places menu

void menuitem_places_finances_activate_cb() {
  create_window_finance();
  if (!window_main.game_state) return;

  auto update_finance_labels = []() {
    if (!window_main.game_state) return;
    window_finance.label_cash->setText(QString::fromStdString(money_string(window_main.game_state->cash)));
    window_finance.label_bank->setText(QString::fromStdString(money_string(window_main.game_state->bank)));
    window_finance.label_debt->setText(QString::fromStdString(money_string(window_main.game_state->debt)));
  };

  update_finance_labels();

  QObject::connect(window_finance.button_doit, &QPushButton::clicked, [update_finance_labels]() {
    if (!window_main.game_state) return;
    int amount = window_finance.spinbutton_amount->value();

    if (window_finance.radiobutton_depositsome->isChecked()) {
      amount = std::min(amount, window_main.game_state->cash);
      window_main.game_state->cash -= amount;
      window_main.game_state->bank += amount;
    } else if (window_finance.radiobutton_depositall->isChecked()) {
      window_main.game_state->bank += window_main.game_state->cash;
      window_main.game_state->cash = 0;
    } else if (window_finance.radiobutton_depositallbut->isChecked()) {
      if (window_main.game_state->cash > amount) {
        int deposit = window_main.game_state->cash - amount;
        window_main.game_state->cash = amount;
        window_main.game_state->bank += deposit;
      }
    } else if (window_finance.radiobutton_withdrawsome->isChecked()) {
      amount = std::min(amount, window_main.game_state->bank);
      window_main.game_state->bank -= amount;
      window_main.game_state->cash += amount;
    } else if (window_finance.radiobutton_withdrawall->isChecked()) {
      window_main.game_state->cash += window_main.game_state->bank;
      window_main.game_state->bank = 0;
    } else if (window_finance.radiobutton_withdrawallbut->isChecked()) {
      if (window_main.game_state->bank > amount) {
        int withdraw = window_main.game_state->bank - amount;
        window_main.game_state->bank = amount;
        window_main.game_state->cash += withdraw;
      }
    }

    update_finance_labels();
    update_all_ui(*window_main.game_state);
  });

  QObject::connect(window_finance.button_repay, &QPushButton::clicked, [update_finance_labels]() {
    if (!window_main.game_state) return;
    int amount = std::min(window_main.game_state->cash, window_main.game_state->debt);
    window_main.game_state->cash -= amount;
    window_main.game_state->debt -= amount;
    update_finance_labels();
    update_all_ui(*window_main.game_state);
  });

  QObject::connect(window_finance.button_borrow, &QPushButton::clicked, [update_finance_labels]() {
    if (!window_main.game_state) return;
    int amount = 2000;
    window_main.game_state->cash += amount;
    window_main.game_state->debt += amount;
    update_finance_labels();
    update_all_ui(*window_main.game_state);
  });

  window_finance.window->show();
  window_finance.window->raise();
  window_finance.window->activateWindow();
}

void menuitem_places_shopping_activate_cb() {
  create_window_shopping();
  if (!window_main.game_state) return;

  window_shopping.label_cash->setText(QString::fromStdString(money_string(window_main.game_state->cash)));

  window_shopping.treeview_store->clear();
  for (int i = 0; i < WEAPON_NUM; ++i) {
    QTreeWidgetItem *item = new QTreeWidgetItem(window_shopping.treeview_store);
    item->setText(COLUMN_STORE_NAME, QString::number(i + 1));
    item->setText(COLUMN_STORE_TYPE, "Weapon");
    item->setText(COLUMN_STORE_PRICE, QString::fromStdString(money_string(weapon_info[i].price)));
  }

  window_shopping.window->show();
  window_shopping.window->raise();
  window_shopping.window->activateWindow();
}

void menuitem_places_hospital_activate_cb() {
  create_window_hospital();
  if (!window_main.game_state) return;

  int current_health = window_main.game_state->health;
  window_hospital.label_cash->setText(QString::fromStdString(money_string(window_main.game_state->cash)));
  window_hospital.progressbar_health->setValue(current_health);
  window_hospital.scalebutton_health->setRange(current_health, 100);
  window_hospital.scalebutton_health->setValue(current_health);
  window_hospital.label_cost->setText("0");

  QObject::connect(window_hospital.scalebutton_health, &QSlider::valueChanged, [](int val) {
    if (!window_main.game_state) return;
    int needed = std::max(0, val - window_main.game_state->health);
    int cost = needed * 50;
    window_hospital.progressbar_health->setValue(val);
    window_hospital.label_cost->setText(QString::fromStdString(money_string(cost)));
  });

  QObject::connect(window_hospital.button_ok, &QPushButton::clicked, []() {
    if (!window_main.game_state) return;
    int target_health = window_hospital.scalebutton_health->value();
    int needed = std::max(0, target_health - window_main.game_state->health);
    int cost = needed * 50;
    if (window_main.game_state->cash >= cost) {
      window_main.game_state->cash -= cost;
      window_main.game_state->health = target_health;
      window_hospital.window->close();
      update_all_ui(*window_main.game_state);
    }
  });

  window_hospital.window->show();
  window_hospital.window->raise();
  window_hospital.window->activateWindow();
}

void menuitem_places_vault_activate_cb() {
  create_window_vault();
  if (!window_main.game_state) return;

  auto update_vault_lists = []() {
    if (!window_main.game_state) return;
    window_vault.treeview_pocket->clear();
    window_vault.treeview_vault->clear();

    for (int i = 0; i < DRUG_NUM; ++i) {
      if (window_main.game_state->player_qty[i] > 0) {
        QTreeWidgetItem *item = new QTreeWidgetItem(window_vault.treeview_pocket);
        item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
        item->setText(1, QString::number(window_main.game_state->player_qty[i]));
        item->setText(2, QString::fromStdString(money_string(window_main.game_state->player_price[i])));
        item->setData(0, Qt::UserRole, i);
      }
      if (window_main.game_state->vault_qty[i] > 0) {
        QTreeWidgetItem *item = new QTreeWidgetItem(window_vault.treeview_vault);
        item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
        item->setText(1, QString::number(window_main.game_state->vault_qty[i]));
        item->setText(2, QString::fromStdString(money_string(window_main.game_state->player_price[i])));
        item->setData(0, Qt::UserRole, i);
      }
    }
  };

  update_vault_lists();

  QObject::connect(window_vault.button_intovault, &QPushButton::clicked, [update_vault_lists]() {
    if (!window_main.game_state) return;
    auto *item = window_vault.treeview_pocket->currentItem();
    if (!item) return;
    int drug_idx = item->data(0, Qt::UserRole).toInt();
    if (drug_idx >= 0 && drug_idx < DRUG_NUM && window_main.game_state->player_qty[drug_idx] > 0) {
      window_main.game_state->player_qty[drug_idx] -= 1;
      window_main.game_state->pocket -= 1;
      window_main.game_state->vault_qty[drug_idx] += 1;
      update_vault_lists();
      update_all_ui(*window_main.game_state);
    }
  });

  QObject::connect(window_vault.button_fromvault, &QPushButton::clicked, [update_vault_lists]() {
    if (!window_main.game_state) return;
    auto *item = window_vault.treeview_vault->currentItem();
    if (!item) return;
    int drug_idx = item->data(0, Qt::UserRole).toInt();
    if (drug_idx >= 0 && drug_idx < DRUG_NUM && window_main.game_state->vault_qty[drug_idx] > 0) {
      if (window_main.game_state->pocket < window_main.game_state->pocket_capacity) {
        window_main.game_state->vault_qty[drug_idx] -= 1;
        window_main.game_state->player_qty[drug_idx] += 1;
        window_main.game_state->pocket += 1;
        update_vault_lists();
        update_all_ui(*window_main.game_state);
      }
    }
  });

  QObject::connect(window_vault.button_ok, &QPushButton::clicked, window_vault.window, &QDialog::close);

  window_vault.window->show();
  window_vault.window->raise();
  window_vault.window->activateWindow();
}

void menuitem_places_shipping_activate_cb() {
  QMessageBox::information(window_main.window, "Shipping", "Shipping is currently not available in this region.");
}

// Info menu

void menuitem_info_vaults_activate_cb() {
  if (!window_main.game_state) return;
  std::string info = "Vault Contents:\n";
  bool empty = true;
  for (int i = 0; i < DRUG_NUM; ++i) {
    if (window_main.game_state->vault_qty[i] > 0) {
      empty = false;
      info += std::format("  {}: {}\n", drug_name(drug_info[i].id), window_main.game_state->vault_qty[i]);
    }
  }
  if (empty) info += "  (Vault is empty)";
  QMessageBox::information(window_main.window, "Vaults", QString::fromStdString(info));
}

void menuitem_info_world_drug_prices_activate_cb() {
  if (!window_main.game_state) return;

  create_window_world_drug_prices();
  fill_treeview_city_list(window_world_drug_prices.treeview_city,
                          window_main.game_state, 0);

  QObject::connect(
      window_world_drug_prices.treeview_drug, &QTreeWidget::itemClicked,
      [&](QTreeWidgetItem* item) {
        if (!window_main.game_state) return;
        int drug_idx = item->data(0, Qt::UserRole).toInt();
        fill_treeview_city_list(window_world_drug_prices.treeview_city,
                                window_main.game_state, drug_idx);
      });

  window_world_drug_prices.window->show();
  window_world_drug_prices.window->raise();
  window_world_drug_prices.window->activateWindow();
}

void menuitem_info_world_cities_activate_cb() {
  if (!window_main.game_state) return;

  create_window_world_cities();
  fill_treeview_drug_list(window_world_cities.treeview_drug,
                          window_main.game_state, 0);

  QObject::connect(window_world_cities.treeview_city, &QTreeWidget::itemClicked, [](QTreeWidgetItem* item) {
    if (!window_main.game_state) return;
    int city_idx = item->data(0, Qt::UserRole).toInt();
    fill_treeview_drug_list(window_world_cities.treeview_drug,
                            window_main.game_state, city_idx);
  });

  window_world_cities.window->show();
  window_world_cities.window->raise();
  window_world_cities.window->activateWindow();
}

void menuitem_info_shipment_status_activate_cb() {
  QMessageBox::information(window_main.window, "Shipment Status", "No active shipments.");
}

void menuitem_info_history_activate_cb() {
  QMessageBox::information(window_main.window, "History", "No history available.");
}

// Main window buttons

void window_main_button_stayhere_clicked_cb() {
  if (!window_main.game_state) return;
  window_main.game_state->stay_here();
  update_all_ui(*window_main.game_state);
  if (window_main.game_state->day >= DAY_NUM - 1) {
    int score = window_main.game_state->cash + window_main.game_state->bank - window_main.game_state->debt;
    if (score < 0) score = 0;
    std::string msg = std::format("That's it, the game is over! You have a final score of ${}", money_string(score));
    QMessageBox::information(window_main.window, "Game Over", QString::fromStdString(msg));
  }
}

void window_main_button_flyaway_clicked_cb() {
  if (!window_main.game_state) return;
  QDialog *dlg = new QDialog(window_main.window);
  dlg->setWindowTitle("Fly Away");
  dlg->setModal(true);
  QVBoxLayout *layout = new QVBoxLayout(dlg);
  layout->addWidget(new QLabel("Select destination city (Ticket: $200):", dlg));

  QListWidget *city_list = new QListWidget(dlg);
  for (int i = 0; i < CITY_NUM; ++i) {
    std::string text = std::format("{}, {}", city_name(city_info[i].id), country_name(city_info[i].country));
    if (i == window_main.game_state->location) {
      text += " (Current)";
    }
    city_list->addItem(QString::fromStdString(text));
  }
  city_list->setCurrentRow((window_main.game_state->location + 1) % CITY_NUM);
  layout->addWidget(city_list);

  QHBoxLayout *btn_box = new QHBoxLayout();
  QPushButton *btn_fly = new QPushButton("Fly", dlg);
  QPushButton *btn_cancel = new QPushButton("Cancel", dlg);
  btn_box->addStretch();
  btn_box->addWidget(btn_fly);
  btn_box->addWidget(btn_cancel);
  layout->addLayout(btn_box);

  QObject::connect(btn_cancel, &QPushButton::clicked, dlg, &QDialog::close);
  QObject::connect(btn_fly, &QPushButton::clicked, [dlg, city_list]() {
    int dest = city_list->currentRow();
    if (dest >= 0 && dest < CITY_NUM && window_main.game_state) {
      if (dest != window_main.game_state->location) {
        if (window_main.game_state->cash >= 200) {
          window_main.game_state->cash -= 200;
        }
        window_main.game_state->location = dest;
        window_main.game_state->stay_here();
      }
      dlg->close();
      update_all_ui(*window_main.game_state);
    }
  });

  dlg->show();
}

void window_main_button_about_clicked_cb() {
  std::string info = std::format("{}\n{}\n\nAuthor: {}\nEmail: {}\nVersion: {}",
                                 kProgramName, kProgramDescription, kProgramAuthorName, kProgramAuthorEmail, kProgramVersion);
  QMessageBox::about(window_main.window, "About Drux Lord", QString::fromStdString(info));
}

void window_main_button_docs_clicked_cb() {
  QMessageBox::information(window_main.window, "Documentation",
                           QString::fromUtf8("Welcome to Drux Lord!\n\n"
                                             "You assume the role of a drug dealer starting with $2,000 in your pocket and a $1,000 loan from Buddles.\n"
                                             "It is your goal to turn that investment into as much wealth as you can in 30 days.\n\n"
                                             "Buy drugs from the market, sell them for a profit, deposit money in the bank to earn interest, "
                                             "and repay your loan shark before the interest gets out of hand!"));
}

void window_main_button_highscores_clicked_cb() {
  QMessageBox::information(window_main.window, "High Scores",
                           QString::fromUtf8("High Scores:\n"
                                             "1. Drug Lord - $10,000,000\n"
                                             "2. Master Dealer - $5,000,000\n"
                                             "3. Distributor - $1,000,000\n"
                                             "4. Dealer - $100,000\n"
                                             "5. Wannabe - $2,000"));
}

void window_main_button_newgamequit_clicked_cb() {
  if (window_main.game_state) {
    window_main.game_state->newgame();
    update_all_ui(*window_main.game_state);
  }
}