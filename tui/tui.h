#pragma once

#include <string>
#include <vector>

#include "druxlord.h"

class TuiApp {
 public:
  static constexpr int ITEM_CASH = 0;
  static constexpr int ITEM_DEBT = 1;
  static constexpr int ITEM_HEALTH = 2;
  static constexpr int ITEM_FIRST_DRUG = 3;
  static constexpr int TOTAL_ITEMS = 3 + static_cast<int>(drug_info.size());

  TuiApp();
  ~TuiApp();

  TuiApp(const TuiApp&) = delete;
  TuiApp& operator=(const TuiApp&) = delete;

  int run();

 private:
  enum class FocusPane { Market, Pocket };

  // Rendering helpers
  void drawDashboard();
  void drawTopBar(int cols);
  void drawMarketPane(int y, int x, int h, int w);
  void drawPocketPane(int y, int x, int h, int w);
  void drawStatusPane(int y, int x, int h, int w);
  void drawGraphPane(int y, int x, int h, int w);
  void drawNewsPane(int y, int x, int h, int w);
  void drawBottomKeyBar(int y, int cols);

  // Braille chart helper
  void renderBrailleChart(int y, int x, int h, int w, int item_idx,
                          int city_idx, bool show_axes);

  // Actions & Modals (1:1 parity with Qt6 MainWindow & Dialogs)
  void actionBuyDrug();
  void actionSellDrug();
  void actionDumpDrug();
  void actionStayHere();
  void actionFlyAway();
  void actionNewGame();

  void showFinancesDialog();
  void showShoppingDialog();
  void showHospitalDialog();
  void showVaultDialog();
  void showVaultsInfoDialog();
  void showShippingDialog();
  void showShipmentStatusDialog();
  void showWorldDrugPricesDialog();
  void showWorldCitiesDialog();
  void showHistoryDialog(int initial_item, int initial_city);
  void showCombatDialog(int enemy_idx, int enemy_count);
  void showPlacesMenu();
  void showInfoMenu();
  void showAboutDialog();
  void showDocsDialog();
  void showHighscoresDialog();
  void showHelpDialog();

  // Generic UI Dialog primitives
  void showMessageModal(const std::string& title, const std::string& message,
                        int border_color = 1);
  bool showConfirmModal(const std::string& title, const std::string& message);
  int showInputSpinModal(const std::string& title, const std::string& message,
                         const std::string& question, int min_val, int max_val,
                         int initial_val, int unit_price = 0);

  void syncSelectionAndChart();
  void checkPostTurnState();

  std::vector<int> availableMarketDrugs() const;
  std::vector<int> ownedPocketDrugs() const;
  static std::string itemName(int item_idx);

  GameState _gameState;
  FocusPane _focus = FocusPane::Market;
  int _marketCursor = 0;
  int _pocketCursor = 0;
  int _chartItemIdx = ITEM_CASH;
  int _chartCityIdx = 0;
  bool _soundEnabled = false;
  bool _running = true;
};
