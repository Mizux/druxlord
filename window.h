#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDialog>
#include <QGroupBox>
#include <QKeySequence>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>
#include <QTreeWidget>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

#include "druxlord.h"

enum { COLUMN_STATUS, COLUMN_NAME, COLUMN_QTY, COLUMN_PRICE };

enum {
  COLUMN_LOAN_NAME,
  COLUMN_LOAN_PAY,
  COLUMN_LOAN_RATE,
  COLUMN_LOAN_DAY,
  COLUMN_LOAN_DEBT,
  COLUMN_LOAN_DAYLEFT
};

enum { COLUMN_STORE_NAME, COLUMN_STORE_TYPE, COLUMN_STORE_PRICE };

enum {
  COLUMN_INVENTORY_NAME,
  COLUMN_INVENTORY_TYPE,
  COLUMN_INVENTORY_QTY,
  COLUMN_INVENTORY_SELLFOR
};

class HistoryChartView : public QChartView {
  Q_OBJECT

 public:
  static constexpr int ITEM_CASH = 0;
  static constexpr int ITEM_DEBT = 1;
  static constexpr int ITEM_HEALTH = 2;
  static constexpr int ITEM_FIRST_DRUG = 3;
  static constexpr int TOTAL_ITEMS = 3 + static_cast<int>(drug_info.size());

  explicit HistoryChartView(bool compact = true, QWidget* parent = nullptr);
  virtual ~HistoryChartView() = default;

  HistoryChartView(const HistoryChartView&) = delete;
  HistoryChartView& operator=(const HistoryChartView&) = delete;

  int itemIndex() const { return _item_idx; }
  void setItemIndex(int idx);

  int cityIndex() const { return _city_idx; }
  void setCityIndex(int idx);

  void updateChart(const GameState& gameState, int city_idx = -1);

  static QString itemName(int item_idx);

 signals:
  void itemChanged(int item_idx);
  void zoomRequested(int item_idx, int city_idx);

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void contextMenuEvent(QContextMenuEvent* event) override;

 private:
  void _setupChart();

  bool _compact = true;
  int _item_idx = ITEM_CASH;
  int _city_idx = 0;

  QChart* _chart = nullptr;
  QValueAxis* _axis_x = nullptr;
  QValueAxis* _axis_y = nullptr;
  QLineSeries* _series_min = nullptr;
  QLineSeries* _series_max = nullptr;
  QLineSeries* _series_avg = nullptr;
  QLineSeries* _series_data = nullptr;
  QScatterSeries* _series_points = nullptr;
  QScatterSeries* _series_traded = nullptr;
};

class MainWindow : public QWidget {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  explicit MainWindow(GameState game_state, QWidget* parent = nullptr);
  virtual ~MainWindow() = default;

  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

  GameState& gameState() { return _gameState; }
  const GameState& gameState() const { return _gameState; }

  void updateAllUi();

  QTreeWidget* treeviewMarket() const { return _treeview_market; }
  QTreeWidget* treeviewPocket() const { return _treeview_pocket; }
  QTextEdit* textviewInformation() const { return _textview_information; }
  HistoryChartView* statusChartView() const { return _drawingarea_status; }

  // Controlling other views
  void showFinance();
  void showShopping();
  void showHospital();
  void showVault();
  void showWorldDrugPrices();
  void showWorldCities();
  void showShipping();
  void showVaultsInfo();
  void showShipmentStatus();
  void showHistory();
  void showHistory(int item_idx, int city_idx);
  void showFlyAway();
  void showAbout();
  void showDocs();
  void showHighscores();
  void buyDrug();
  void sellDrug();
  void dumpDrug();
  void stayHere();
  void newGame();

 signals:
  void stateUpdated();

 public slots:
  void slotBuy();
  void slotSell();
  void slotDump();
  void slotPlacesFinances();
  void slotPlacesShopping();
  void slotPlacesHospital();
  void slotPlacesVault();
  void slotPlacesShipping();
  void slotInfoVaults();
  void slotInfoWorldDrugPrices();
  void slotInfoWorldCities();
  void slotInfoShipmentStatus();
  void slotInfoHistory();
  void slotStayHere();
  void slotFlyAway();
  void slotAbout();
  void slotDocs();
  void slotHighscores();
  void slotNewGameQuit();
  void onMarketItemSelectionChanged();
  void onPocketItemSelectionChanged();
  void onStatusZoomRequested(int item_idx, int city_idx);

 private:
  void _setupWidget();
  QMenu* _createPlacesMenu(QPushButton* button);
  QMenu* _createInfoMenu(QPushButton* button);

  void _fillTreeviewMarket();
  void _fillTreeviewPocket();

  void _setLabelPocket(int npocket, int capacity = 10);
  void _setLabelLocation(int location);
  void _setLabelHealth(int health);
  void _setLabelDay(int day);
  void _setLabelRank(int rank);
  void _setLabelCash(int value);
  void _setLabelBank(int value);
  void _setLabelDebt(int value);

  GameState _gameState;

  QTextEdit* _textview_information = nullptr;
  QTreeWidget* _treeview_market = nullptr;
  QTreeWidget* _treeview_pocket = nullptr;
  QPushButton* _button_buy = nullptr;
  QPushButton* _button_sell = nullptr;
  QPushButton* _button_dump = nullptr;
  QPushButton* _button_places = nullptr;
  QPushButton* _button_info = nullptr;
  QPushButton* _button_stayhere = nullptr;
  QPushButton* _button_flyaway = nullptr;
  QPushButton* _button_about = nullptr;
  QPushButton* _button_docs = nullptr;
  QPushButton* _button_highscores = nullptr;
  QPushButton* _button_newgamequit = nullptr;
  QCheckBox* _checkbutton_sound = nullptr;
  QLabel* _label_pocket = nullptr;
  QGroupBox* _group_pocket = nullptr;
  QLabel* _label_location = nullptr;
  QLabel* _label_day = nullptr;
  QLabel* _label_rank = nullptr;
  QLabel* _label_cash = nullptr;
  QLabel* _label_bank = nullptr;
  QLabel* _label_debt = nullptr;
  QProgressBar* _progressbar_health = nullptr;
  HistoryChartView* _drawingarea_status = nullptr;
  QShortcut* _shortcut_quit = nullptr;
};

class WindowFinance : public QDialog {
  Q_OBJECT

 public:
  explicit WindowFinance(GameState& gameState, QWidget* parent = nullptr);
  virtual ~WindowFinance() = default;

  WindowFinance(const WindowFinance&) = delete;
  WindowFinance& operator=(const WindowFinance&) = delete;

  void updateFinanceLabels();

 signals:
  void stateChanged();

 public slots:
  void onDoItClicked();
  void onBorrowClicked();
  void onRepayClicked();

 private:
  void _setupWidget();

  GameState& _gameState;

  QRadioButton* _radiobutton_depositsome = nullptr;
  QRadioButton* _radiobutton_depositall = nullptr;
  QRadioButton* _radiobutton_depositallbut = nullptr;
  QRadioButton* _radiobutton_withdrawsome = nullptr;
  QRadioButton* _radiobutton_withdrawall = nullptr;
  QRadioButton* _radiobutton_withdrawallbut = nullptr;
  QSpinBox* _spinbutton_amount = nullptr;
  QPushButton* _button_doit = nullptr;
  QTreeWidget* _treeview_loan = nullptr;
  QLabel* _label_cash = nullptr;
  QLabel* _label_bank = nullptr;
  QLabel* _label_debt = nullptr;
  QPushButton* _button_borrow = nullptr;
  QPushButton* _button_repay = nullptr;
  QPushButton* _button_done = nullptr;
};

using FinanceDialog = WindowFinance;

class WindowShopping : public QDialog {
  Q_OBJECT

 public:
  explicit WindowShopping(GameState& gameState, QWidget* parent = nullptr);
  virtual ~WindowShopping() = default;

  WindowShopping(const WindowShopping&) = delete;
  WindowShopping& operator=(const WindowShopping&) = delete;

  void updateShopping();

 signals:
  void stateChanged();

 private slots:
  void onBuyClicked();
  void onSellClicked();
  void onStoreItemSelectionChanged();
  void onInventoryItemSelectionChanged();

 private:
  void _setupWidget();

  GameState& _gameState;

  QTreeWidget* _treeview_store = nullptr;
  QTreeWidget* _treeview_inventory = nullptr;
  QPushButton* _button_buy = nullptr;
  QPushButton* _button_sell = nullptr;
  QPushButton* _button_done = nullptr;
  QLabel* _label_cash = nullptr;
};

using ShoppingDialog = WindowShopping;

class WindowHospital : public QDialog {
  Q_OBJECT

 public:
  explicit WindowHospital(GameState& gameState, QWidget* parent = nullptr);
  virtual ~WindowHospital() = default;

  WindowHospital(const WindowHospital&) = delete;
  WindowHospital& operator=(const WindowHospital&) = delete;

 signals:
  void stateChanged();

 public slots:
  void onSliderValueChanged(int val);
  void onOkClicked();

 private:
  void _setupWidget();

  GameState& _gameState;

  QProgressBar* _progressbar_health = nullptr;
  QSlider* _scalebutton_health = nullptr;
  QLabel* _label_cash = nullptr;
  QLabel* _label_cost = nullptr;
  QPushButton* _button_ok = nullptr;
  QPushButton* _button_cancel = nullptr;
};

using HospitalDialog = WindowHospital;

class WindowVault : public QDialog {
  Q_OBJECT

 public:
  explicit WindowVault(GameState& gameState, QWidget* parent = nullptr);
  virtual ~WindowVault() = default;

  WindowVault(const WindowVault&) = delete;
  WindowVault& operator=(const WindowVault&) = delete;

  void updateVaultLists();

 signals:
  void stateChanged();

 public slots:
  void onIntoVaultClicked();
  void onFromVaultClicked();

 private:
  void _setupWidget();

  GameState& _gameState;

  QGroupBox* _frame_pocket = nullptr;
  QTreeWidget* _treeview_pocket = nullptr;
  QTreeWidget* _treeview_vault = nullptr;
  QPushButton* _button_intovault = nullptr;
  QPushButton* _button_fromvault = nullptr;
  QPushButton* _button_ok = nullptr;
  QPushButton* _button_cancel = nullptr;
};

using VaultDialog = WindowVault;

class WindowWorldDrugPrices : public QDialog {
  Q_OBJECT

 public:
  explicit WindowWorldDrugPrices(const GameState& gameState,
                                 QWidget* parent = nullptr);
  virtual ~WindowWorldDrugPrices() = default;

  WindowWorldDrugPrices(const WindowWorldDrugPrices&) = delete;
  WindowWorldDrugPrices& operator=(const WindowWorldDrugPrices&) = delete;

  void fillCityList(int drug_idx);

 public slots:
  void onDrugItemClicked(QTreeWidgetItem* item, int column);
  void onCityItemClicked(QTreeWidgetItem* item, int column);

 private:
  void _setupWidget();
  void _updateChart();

  const GameState& _gameState;
  int _selectedDrug = 0;
  int _selectedCity = 0;

  QTreeWidget* _treeview_drug = nullptr;
  QTreeWidget* _treeview_city = nullptr;
  HistoryChartView* _chart_view = nullptr;
  QPushButton* _button_close = nullptr;
};

using WorldDrugPricesDialog = WindowWorldDrugPrices;

class WindowWorldCities : public QDialog {
  Q_OBJECT

 public:
  explicit WindowWorldCities(const GameState& gameState,
                             QWidget* parent = nullptr);
  virtual ~WindowWorldCities() = default;

  WindowWorldCities(const WindowWorldCities&) = delete;
  WindowWorldCities& operator=(const WindowWorldCities&) = delete;

  void fillDrugList(int city_idx);

 public slots:
  void onCityItemClicked(QTreeWidgetItem* item, int column);
  void onDrugItemClicked(QTreeWidgetItem* item, int column);

 private:
  void _setupWidget();
  void _updateChart();

  const GameState& _gameState;
  int _selectedCity = 0;
  int _selectedDrug = 0;

  QTreeWidget* _treeview_city = nullptr;
  QTreeWidget* _treeview_drug = nullptr;
  HistoryChartView* _chart_view = nullptr;
  QPushButton* _button_close = nullptr;
};

using WorldCitiesDialog = WindowWorldCities;

class WindowHistory : public QDialog {
  Q_OBJECT

 public:
  explicit WindowHistory(const GameState& gameState, int initial_item = 0,
                         int initial_city = -1, QWidget* parent = nullptr);
  virtual ~WindowHistory() = default;

  WindowHistory(const WindowHistory&) = delete;
  WindowHistory& operator=(const WindowHistory&) = delete;

 public slots:
  void onCityChanged(int index);
  void onItemChanged(int index);

 private:
  void _setupWidget(int initial_item, int initial_city);
  void _refreshChart();

  const GameState& _gameState;

  QComboBox* _combo_city = nullptr;
  QComboBox* _combo_item = nullptr;
  HistoryChartView* _chart_view = nullptr;
  QLabel* _label_legend = nullptr;
  QPushButton* _button_close = nullptr;
};

using HistoryDialog = WindowHistory;

class WindowInput : public QDialog {
  Q_OBJECT

 public:
  explicit WindowInput(QWidget* parent = nullptr);
  WindowInput(const QString& title, const QString& message,
              const QString& question, QWidget* parent = nullptr);
  virtual ~WindowInput() = default;

  WindowInput(const WindowInput&) = delete;
  WindowInput& operator=(const WindowInput&) = delete;

  void setMessage(const QString& message);
  void setQuestion(const QString& question);
  void setRange(int min, int max);
  void setValue(int val);
  int value() const;

  QSpinBox* spinbox() const { return _spinbutton_value; }
  QPushButton* buttonOk() const { return _button_ok; }
  QPushButton* buttonCancel() const { return _button_cancel; }

 private:
  void _setupWidget(const QString& title, const QString& message,
                    const QString& question);

  QLabel* _label_msg = nullptr;
  QLabel* _label_question = nullptr;
  QSpinBox* _spinbutton_value = nullptr;
  QPushButton* _button_ok = nullptr;
  QPushButton* _button_cancel = nullptr;
};

using InputDialog = WindowInput;

class WindowFlyAway : public QDialog {
  Q_OBJECT

 public:
  explicit WindowFlyAway(GameState& gameState, QWidget* parent = nullptr);
  virtual ~WindowFlyAway() = default;

  WindowFlyAway(const WindowFlyAway&) = delete;
  WindowFlyAway& operator=(const WindowFlyAway&) = delete;

  QListWidget* cityList() const { return _city_list; }
  QPushButton* buttonFly() const { return _button_fly; }
  QPushButton* buttonCancel() const { return _button_cancel; }

 signals:
  void stateChanged();

 private slots:
  void onSelectionChanged();
  void onFlyClicked();

 private:
  void _setupWidget();
  void _populateCityList();

  GameState& _gameState;
  QLabel* _label_cash = nullptr;
  QListWidget* _city_list = nullptr;
  QPushButton* _button_fly = nullptr;
  QPushButton* _button_cancel = nullptr;
};

using FlyAwayDialog = WindowFlyAway;

class WindowCombat : public QDialog {
  Q_OBJECT

 public:
  explicit WindowCombat(GameState& gameState, int enemy_idx, int enemy_count,
                        QWidget* parent = nullptr);
  virtual ~WindowCombat() = default;

  WindowCombat(const WindowCombat&) = delete;
  WindowCombat& operator=(const WindowCombat&) = delete;

 signals:
  void stateChanged();

 private slots:
  void onFightClicked();
  void onFleeClicked();
  void onBribeClicked();
  void onSurrenderClicked();

 private:
  void _setupWidget();
  void _updateStatus();
  void _enemyAttack();

  GameState& _gameState;
  int _enemyIdx;
  int _enemyCount;

  QLabel* _labelHeader = nullptr;
  QLabel* _labelEnemy = nullptr;
  QLabel* _labelHealth = nullptr;
  QLabel* _labelCash = nullptr;
  QLabel* _labelWeapon = nullptr;
  QLabel* _labelAmmo = nullptr;
  QLabel* _labelArmor = nullptr;
  QTextEdit* _textLog = nullptr;

  QPushButton* _buttonFight = nullptr;
  QPushButton* _buttonFlee = nullptr;
  QPushButton* _buttonBribe = nullptr;
  QPushButton* _buttonSurrender = nullptr;
};

using CombatDialog = WindowCombat;
