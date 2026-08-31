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

void window_main_button_buy_clicked_cb(MainWindow& window) {
  auto& game_state = window.gameState();
  auto* treeview = window.treeviewMarket();
  if (!treeview) return;
  auto* item = treeview->currentItem();
  if (!item) {
    if (treeview->topLevelItemCount() > 0) {
      item = treeview->topLevelItem(0);
      treeview->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(COLUMN_NAME, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;

  int j = game_state.location;
  int d = game_state.day;
  int price = game_state.drug_table[drug_idx][j][d].price;
  int available_qty = game_state.drug_table[drug_idx][j][d].qty;
  int max_afford = (price > 0) ? (game_state.cash / price) : 0;
  int space = game_state.pocket_capacity - game_state.pocket;
  int max_buy = std::min(available_qty, std::min(max_afford, space));

  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Buying {}", name);
  std::string message = std::format(
      "{} is currently selling for ${} per unit. With your "
      "available funds, you can buy {}.",
      name, money_string(price), max_buy);

  WindowInput input_dlg(QString::fromStdString(title),
                        QString::fromStdString(message), "How many to buy?",
                        &window);
  input_dlg.setRange(1, std::max(1, max_buy));
  input_dlg.setValue(std::max(1, max_buy));
  if (max_buy <= 0) {
    input_dlg.buttonOk()->setEnabled(false);
  }

  if (input_dlg.exec() == QDialog::Accepted) {
    int count = input_dlg.value();
    if (count > 0 && count <= max_buy) {
      int cost = count * price;
      game_state.cash -= cost;
      game_state.pocket += count;
      int prev_qty = game_state.player_qty[drug_idx];
      int prev_price = game_state.player_price[drug_idx];
      game_state.player_price[drug_idx] =
          (prev_price * prev_qty + cost) / (prev_qty + count);
      game_state.player_qty[drug_idx] += count;
      game_state.drug_table[drug_idx][j][d].qty -= count;
      if (game_state.drug_table[drug_idx][j][d].qty <= 0) {
        game_state.drug_table[drug_idx][j][d].available = false;
      }
      window.updateAllUi();
    }
  }
}

void window_main_button_sell_clicked_cb(MainWindow& window) {
  auto& game_state = window.gameState();
  auto* treeview = window.treeviewPocket();
  if (!treeview) return;
  auto* item = treeview->currentItem();
  if (!item) {
    if (treeview->topLevelItemCount() > 0) {
      item = treeview->topLevelItem(0);
      treeview->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;
  int owned_qty = game_state.player_qty[drug_idx];
  if (owned_qty <= 0) return;

  int j = game_state.location;
  int d = game_state.day;
  int price = game_state.drug_table[drug_idx][j][d].price;
  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Selling {}", name);
  std::string message = std::format(
      "{} is currently being bought for ${} per unit. You have {} to sell.",
      name, money_string(price), owned_qty);

  WindowInput input_dlg(QString::fromStdString(title),
                        QString::fromStdString(message), "How many to sell?",
                        &window);
  input_dlg.setRange(1, owned_qty);
  input_dlg.setValue(owned_qty);

  if (input_dlg.exec() == QDialog::Accepted) {
    int count = input_dlg.value();
    if (count > 0 && count <= owned_qty) {
      int gain = count * price;
      game_state.cash += gain;
      game_state.pocket -= count;
      game_state.player_qty[drug_idx] -= count;
      if (game_state.player_qty[drug_idx] == 0) {
        game_state.player_price[drug_idx] = 0;
      }
      window.updateAllUi();
    }
  }
}

void window_main_button_dump_clicked_cb(MainWindow& window) {
  auto& game_state = window.gameState();
  auto* treeview = window.treeviewPocket();
  if (!treeview) return;
  auto* item = treeview->currentItem();
  if (!item) {
    if (treeview->topLevelItemCount() > 0) {
      item = treeview->topLevelItem(0);
      treeview->setCurrentItem(item);
    } else {
      return;
    }
  }

  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx < 0 || drug_idx >= DRUG_NUM) return;
  int owned_qty = game_state.player_qty[drug_idx];
  if (owned_qty <= 0) return;

  std::string name = drug_name(drug_info[drug_idx].id);
  std::string title = std::format("Dumping {}", name);
  std::string message =
      std::format("You have {} {} in your pocket to dump.", owned_qty, name);

  WindowInput input_dlg(QString::fromStdString(title),
                        QString::fromStdString(message), "How many to dump?",
                        &window);
  input_dlg.setRange(1, owned_qty);
  input_dlg.setValue(owned_qty);

  if (input_dlg.exec() == QDialog::Accepted) {
    int count = input_dlg.value();
    if (count > 0 && count <= owned_qty) {
      game_state.pocket -= count;
      game_state.player_qty[drug_idx] -= count;
      if (game_state.player_qty[drug_idx] == 0) {
        game_state.player_price[drug_idx] = 0;
      }
      window.updateAllUi();
    }
  }
}

// Places menu

void menuitem_places_finances_activate_cb(MainWindow& window) {
  WindowFinance dlg(window.gameState(), &window);
  QObject::connect(&dlg, &WindowFinance::stateChanged,
                   [&window]() { window.updateAllUi(); });
  dlg.exec();
  window.updateAllUi();
}

void menuitem_places_shopping_activate_cb(MainWindow& window) {
  WindowShopping dlg(window.gameState(), &window);
  QObject::connect(&dlg, &WindowShopping::stateChanged,
                   [&window]() { window.updateAllUi(); });
  dlg.exec();
  window.updateAllUi();
}

void menuitem_places_hospital_activate_cb(MainWindow& window) {
  WindowHospital dlg(window.gameState(), &window);
  QObject::connect(&dlg, &WindowHospital::stateChanged,
                   [&window]() { window.updateAllUi(); });
  dlg.exec();
  window.updateAllUi();
}

void menuitem_places_vault_activate_cb(MainWindow& window) {
  WindowVault dlg(window.gameState(), &window);
  QObject::connect(&dlg, &WindowVault::stateChanged,
                   [&window]() { window.updateAllUi(); });
  dlg.exec();
  window.updateAllUi();
}

void menuitem_places_shipping_activate_cb(MainWindow& window) {
  QMessageBox::information(
      &window, "Shipping",
      "Shipping is currently not available in this region.");
}

// Info menu

void menuitem_info_vaults_activate_cb(MainWindow& window) {
  const auto& game_state = window.gameState();
  std::string info = "Vault Contents:\n";
  bool empty = true;
  for (int i = 0; i < DRUG_NUM; ++i) {
    if (game_state.vault_qty[i] > 0) {
      empty = false;
      info += std::format("  {}: {}\n", drug_name(drug_info[i].id),
                          game_state.vault_qty[i]);
    }
  }
  if (empty) info += "  (Vault is empty)";
  QMessageBox::information(&window, "Vaults", QString::fromStdString(info));
}

void menuitem_info_world_drug_prices_activate_cb(MainWindow& window) {
  WindowWorldDrugPrices dlg(window.gameState(), &window);
  dlg.exec();
}

void menuitem_info_world_cities_activate_cb(MainWindow& window) {
  WindowWorldCities dlg(window.gameState(), &window);
  dlg.exec();
}

void menuitem_info_shipment_status_activate_cb(MainWindow& window) {
  QMessageBox::information(&window, "Shipment Status", "No active shipments.");
}

void menuitem_info_history_activate_cb(MainWindow& window) {
  QMessageBox::information(&window, "History", "No history available.");
}

// Main window buttons

void window_main_button_stayhere_clicked_cb(MainWindow& window) {
  auto& game_state = window.gameState();
  Encounter enc = game_state.check_random_encounter();
  game_state.stay_here();
  if (enc.type != EncounterType::None) {
    if (enc.type == EncounterType::Combat) {
      WindowCombat dlg(game_state, enc.enemy_idx, enc.enemy_count, &window);
      dlg.exec();
    } else {
      QMessageBox::information(&window, "Random Encounter",
                               QString::fromStdString(enc.message));
    }
  }
  window.updateAllUi();
  if (game_state.health <= 0) {
    QMessageBox::critical(&window, "Game Over", "YOU WERE KILLED!");
    game_state.newgame();
    window.updateAllUi();
    return;
  }
  if (game_state.day >= DAY_NUM - 1) {
    int score = game_state.cash + game_state.bank - game_state.debt;
    if (score < 0) score = 0;
    std::string msg = std::format(
        "That's it, the game is over! You have a final score of ${}",
        money_string(score));
    QMessageBox::information(&window, "Game Over", QString::fromStdString(msg));
  }
}

void window_main_button_flyaway_clicked_cb(MainWindow& window) {
  WindowFlyAway dlg(window.gameState(), &window);
  QObject::connect(&dlg, &WindowFlyAway::stateChanged,
                   [&window]() { window.updateAllUi(); });
  if (dlg.exec() == QDialog::Accepted) {
    window.updateAllUi();
    auto& game_state = window.gameState();
    if (game_state.health <= 0) {
      QMessageBox::critical(&window, "Game Over", "YOU WERE KILLED!");
      game_state.newgame();
      window.updateAllUi();
      return;
    }
    if (game_state.day >= DAY_NUM - 1) {
      int score = game_state.cash + game_state.bank - game_state.debt;
      if (score < 0) score = 0;
      std::string msg = std::format(
          "That's it, the game is over! You have a final score of ${}",
          money_string(score));
      QMessageBox::information(&window, "Game Over",
                               QString::fromStdString(msg));
    }
  }
}

void window_main_button_about_clicked_cb(MainWindow& window) {
  std::string info =
      std::format("{}\n{}\n\nAuthor: {}\nEmail: {}\nVersion: {}", kProgramName,
                  kProgramDescription, kProgramAuthorName, kProgramAuthorEmail,
                  kProgramVersion);
  QMessageBox::about(&window, "About Drux Lord", QString::fromStdString(info));
}

void window_main_button_docs_clicked_cb(MainWindow& window) {
  QMessageBox::information(
      &window, "Documentation",
      QString::fromUtf8(
          "Welcome to Drux Lord!\n\n"
          "You assume the role of a drug dealer starting with $2,000 in your "
          "pocket and a $1,000 loan from Buddles.\n"
          "It is your goal to turn that investment into as much wealth as you "
          "can in 30 days.\n\n"
          "Buy drugs from the market, sell them for a profit, deposit money in "
          "the bank to earn interest, "
          "and repay your loan shark before the interest gets out of hand!"));
}

void window_main_button_highscores_clicked_cb(MainWindow& window) {
  QMessageBox::information(&window, "High Scores",
                           QString::fromUtf8("High Scores:\n"
                                             "1. Drug Lord - $10,000,000\n"
                                             "2. Master Dealer - $5,000,000\n"
                                             "3. Distributor - $1,000,000\n"
                                             "4. Dealer - $100,000\n"
                                             "5. Wannabe - $2,000"));
}

void window_main_button_newgamequit_clicked_cb(MainWindow& window) {
  window.gameState().newgame();
  window.updateAllUi();
}