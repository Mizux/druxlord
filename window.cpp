#include "window.h"

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>
#include <QTextEdit>
#include <QTreeWidget>
#include <QWidget>
#include <array>
#include <format>
#include <random>
#include <string>

#include "config.h"
#include "druxlord.h"
#include "window-cb.h"

inline constexpr int COLUMN_NAME_WIDTH = 120;
inline constexpr int COLUMN_QTY_WIDTH = 50;
inline constexpr int COLUMN_PRICE_WIDTH = 70;

// Static treeview helper functions
static QTreeWidget* create_treeview_drug(bool with_status) {
  QTreeWidget* treeview = new QTreeWidget();
  treeview->setRootIsDecorated(false);
  treeview->setUniformRowHeights(true);
  treeview->header()->setStretchLastSection(false);

  if (with_status) {
    treeview->setColumnCount(4);
    treeview->setHeaderLabels({"", "Name", "Qty", "Price"});
    treeview->setColumnWidth(COLUMN_STATUS, 24);
    treeview->setColumnWidth(COLUMN_NAME, COLUMN_NAME_WIDTH);
    treeview->setColumnWidth(COLUMN_QTY, COLUMN_QTY_WIDTH);
    treeview->setColumnWidth(COLUMN_PRICE, COLUMN_PRICE_WIDTH);
    treeview->headerItem()->setTextAlignment(COLUMN_QTY,
                                             Qt::AlignRight | Qt::AlignVCenter);
    treeview->headerItem()->setTextAlignment(COLUMN_PRICE,
                                             Qt::AlignRight | Qt::AlignVCenter);
  } else {
    treeview->setColumnCount(3);
    treeview->setHeaderLabels({"Name", "Qty", "Price"});
    treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
    treeview->setColumnWidth(1, COLUMN_QTY_WIDTH);
    treeview->setColumnWidth(2, COLUMN_PRICE_WIDTH);
    treeview->headerItem()->setTextAlignment(1,
                                             Qt::AlignRight | Qt::AlignVCenter);
    treeview->headerItem()->setTextAlignment(2,
                                             Qt::AlignRight | Qt::AlignVCenter);
  }

  return treeview;
}

static QTreeWidget* create_treeview_drug_names() {
  QTreeWidget* treeview = new QTreeWidget();
  treeview->setRootIsDecorated(false);
  treeview->setUniformRowHeights(true);
  treeview->header()->setStretchLastSection(false);
  treeview->setColumnCount(1);
  treeview->setHeaderLabels({"Name"});
  treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
  return treeview;
}

static QTreeWidget* create_treeview_city_list() {
  QTreeWidget* treeview = new QTreeWidget();
  treeview->setRootIsDecorated(false);
  treeview->setUniformRowHeights(true);
  treeview->header()->setStretchLastSection(false);
  treeview->setColumnCount(3);
  treeview->setHeaderLabels({"City", "Qty", "Price"});
  treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
  treeview->setColumnWidth(1, COLUMN_QTY_WIDTH);
  treeview->setColumnWidth(2, COLUMN_PRICE_WIDTH);
  return treeview;
}

static QTreeWidget* create_treeview_city_names() {
  QTreeWidget* treeview = new QTreeWidget();
  treeview->setRootIsDecorated(false);
  treeview->setUniformRowHeights(true);
  treeview->header()->setStretchLastSection(false);
  treeview->setColumnCount(1);
  treeview->setHeaderLabels({"City Name"});
  treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
  return treeview;
}

static QTreeWidget* create_treeview_drug_list() {
  QTreeWidget* treeview = new QTreeWidget();
  treeview->setRootIsDecorated(false);
  treeview->setUniformRowHeights(true);
  treeview->header()->setStretchLastSection(false);
  treeview->setColumnCount(3);
  treeview->setHeaderLabels({"Drug", "Qty", "Price"});
  treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
  treeview->setColumnWidth(1, COLUMN_QTY_WIDTH);
  treeview->setColumnWidth(2, COLUMN_PRICE_WIDTH);
  return treeview;
}

// MainWindow Implementation

MainWindow::MainWindow(QWidget* parent) : QWidget(parent), _gameState() {
  _setupWidget();
  updateAllUi();
}

MainWindow::MainWindow(GameState game_state, QWidget* parent)
    : QWidget(parent), _gameState(std::move(game_state)) {
  _setupWidget();
  updateAllUi();
}

void MainWindow::updateAllUi() {
  _setLabelLocation(_gameState.location);
  _setLabelHealth(_gameState.health);
  _setLabelDay(_gameState.day + 1);
  _setLabelRank(_gameState.rank);
  _setLabelCash(_gameState.cash);
  _setLabelBank(_gameState.bank);
  _setLabelDebt(_gameState.debt);
  _setLabelPocket(_gameState.pocket, _gameState.pocket_capacity);
  _fillTreeviewMarket();
  _fillTreeviewPocket();
  if (_textview_information) {
    std::string news =
        _gameState.get_market_news(_gameState.location, _gameState.day);
    _textview_information->setText(QString::fromStdString(news));
  }
}

void MainWindow::_fillTreeviewMarket() {
  if (!_treeview_market) return;
  _treeview_market->clear();
  int loc = _gameState.location;
  int day = _gameState.day;

  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    if (_gameState.drug_table[drug_idx][loc][day].available) {
      std::string price_str =
          money_string(_gameState.drug_table[drug_idx][loc][day].price);
      QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_market);
      std::string name_str = drug_name(drug_info[drug_idx].id);
      if (_gameState.drug_table[drug_idx][loc][day].event_flag > 0) {
        item->setText(COLUMN_STATUS, QString::fromUtf8("\u25B2"));
      } else if (_gameState.drug_table[drug_idx][loc][day].event_flag < 0) {
        item->setText(COLUMN_STATUS, QString::fromUtf8("\u25BC"));
      }
      item->setText(COLUMN_NAME, QString::fromStdString(name_str));
      item->setText(
          COLUMN_QTY,
          QString::number(_gameState.drug_table[drug_idx][loc][day].qty));
      item->setText(COLUMN_PRICE, QString::fromStdString(price_str));
      item->setData(COLUMN_NAME, Qt::UserRole, drug_idx);
      item->setTextAlignment(COLUMN_QTY, Qt::AlignRight | Qt::AlignVCenter);
      item->setTextAlignment(COLUMN_PRICE, Qt::AlignRight | Qt::AlignVCenter);
    }
  }
}

void MainWindow::_fillTreeviewPocket() {
  if (!_treeview_pocket) return;
  _treeview_pocket->clear();

  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    if (_gameState.player_qty[drug_idx] > 0) {
      QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_pocket);
      std::string name_str = drug_name(drug_info[drug_idx].id);
      item->setText(0, QString::fromStdString(name_str));
      item->setText(1, QString::number(_gameState.player_qty[drug_idx]));
      item->setText(2, QString::fromStdString(
                           money_string(_gameState.player_price[drug_idx])));
      item->setData(0, Qt::UserRole, drug_idx);
      item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
      item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
    }
  }
}

void MainWindow::_setLabelPocket(int npocket, int capacity) {
  auto str = std::format("You pants pocket ({}/{})", npocket, capacity);
  if (_label_pocket) {
    _label_pocket->setText(QString::fromStdString(str));
  }
  if (_group_pocket) {
    _group_pocket->setTitle(QString::fromStdString(str));
  }
}

void MainWindow::_setLabelHealth(int health) {
  if (_progressbar_health) {
    _progressbar_health->setValue(health);
  }
}

void MainWindow::_setLabelLocation(int location) {
  std::string loc_str;
  if (location >= 0 && location < CITY_NUM) {
    loc_str = std::format("{}, {}", city_name(city_info[location].id),
                          country_name(city_info[location].country));
  } else {
    loc_str = "Austin, USA";
  }
  auto markup = std::format("<span><b>{}</b></span>", loc_str);
  if (_label_location) {
    _label_location->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setLabelDay(int day) {
  auto markup = std::format("<span><b>{}/30</b></span>", day);
  if (_label_day) {
    _label_day->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setLabelRank(int rank) {
  constexpr std::array<const char*, RANK_NUM> rank_str = {
      "wannabe",         "small time operator", "dealer",
      "big time dealer", "distributor",         "drug lord"};
  const char* r = (rank >= 0 && rank < RANK_NUM) ? rank_str[rank] : "wannabe";
  auto markup = std::format("<span><b>{}</b></span>", r);
  if (_label_rank) {
    _label_rank->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setLabelCash(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  if (_label_cash) {
    _label_cash->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setLabelBank(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  if (_label_bank) {
    _label_bank->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setLabelDebt(int value) {
  std::string markup;
  if (value > 0) {
    markup =
        std::format("<span style=\"color:#FF0000;\"><b>{}</b></span>", value);
  } else {
    markup = std::format("<span><b>{}</b></span>", value);
  }
  if (_label_debt) {
    _label_debt->setText(QString::fromStdString(markup));
  }
}

void MainWindow::_setupWidget() {
  setWindowTitle(QString::fromUtf8(kProgramName.data(), kProgramName.size()));

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox* frame_info = new QGroupBox("Information", this);
  QVBoxLayout* vbox_info = new QVBoxLayout(frame_info);
  vbox_info->setContentsMargins(5, 5, 5, 5);
  _textview_information = new QTextEdit(frame_info);
  _textview_information->setReadOnly(true);
  _textview_information->setFixedHeight(120);
  vbox_info->addWidget(_textview_information);
  vbox_main->addWidget(frame_info);

  QHBoxLayout* hbox_down = new QHBoxLayout();
  hbox_down->setSpacing(8);
  vbox_main->addLayout(hbox_down);

  QGroupBox* frame_market = new QGroupBox("The Market", this);
  QVBoxLayout* vbox_market = new QVBoxLayout(frame_market);
  vbox_market->setContentsMargins(5, 5, 5, 5);
  _treeview_market = create_treeview_drug(true);
  _treeview_market->setMinimumSize(270, 240);
  vbox_market->addWidget(_treeview_market);
  hbox_down->addWidget(frame_market);

  QVBoxLayout* vbox_middle = new QVBoxLayout();
  vbox_middle->setSpacing(5);
  hbox_down->addLayout(vbox_middle);

  QGroupBox* frame_action = new QGroupBox("Action", this);
  QVBoxLayout* box_action = new QVBoxLayout(frame_action);
  box_action->setContentsMargins(5, 5, 5, 5);
  box_action->setSpacing(3);

  _button_buy = new QPushButton(QString::fromUtf8("&Buy \u2192"), frame_action);
  connect(_button_buy, &QPushButton::clicked, this, &MainWindow::slotBuy);
  box_action->addWidget(_button_buy);

  _button_sell =
      new QPushButton(QString::fromUtf8("\u2190 &Sell"), frame_action);
  connect(_button_sell, &QPushButton::clicked, this, &MainWindow::slotSell);
  box_action->addWidget(_button_sell);

  _button_dump = new QPushButton("&Dump", frame_action);
  connect(_button_dump, &QPushButton::clicked, this, &MainWindow::slotDump);
  box_action->addWidget(_button_dump);

  _button_places = new QPushButton("Places...", frame_action);
  _createPlacesMenu(_button_places);
  box_action->addWidget(_button_places);

  _button_info = new QPushButton("Info...", frame_action);
  _createInfoMenu(_button_info);
  box_action->addWidget(_button_info);

  vbox_middle->addWidget(frame_action);

  QGroupBox* frame_tomorrow = new QGroupBox("Tomorrow", this);
  QVBoxLayout* box_tomorrow = new QVBoxLayout(frame_tomorrow);
  box_tomorrow->setContentsMargins(5, 5, 5, 5);
  box_tomorrow->setSpacing(3);

  _button_stayhere = new QPushButton("Stay Here", frame_tomorrow);
  connect(_button_stayhere, &QPushButton::clicked, this,
          &MainWindow::slotStayHere);
  box_tomorrow->addWidget(_button_stayhere);

  _button_flyaway = new QPushButton("Fly Away", frame_tomorrow);
  connect(_button_flyaway, &QPushButton::clicked, this,
          &MainWindow::slotFlyAway);
  box_tomorrow->addWidget(_button_flyaway);

  vbox_middle->addWidget(frame_tomorrow);

  QGroupBox* frame_game = new QGroupBox("Game", this);
  QVBoxLayout* box_game = new QVBoxLayout(frame_game);
  box_game->setContentsMargins(5, 5, 5, 5);
  box_game->setSpacing(3);

  _checkbutton_sound = new QCheckBox("Sou&nd", frame_game);
  box_game->addWidget(_checkbutton_sound);

  _button_about = new QPushButton("&About", frame_game);
  connect(_button_about, &QPushButton::clicked, this, &MainWindow::slotAbout);
  box_game->addWidget(_button_about);

  _button_docs = new QPushButton("Docs", frame_game);
  connect(_button_docs, &QPushButton::clicked, this, &MainWindow::slotDocs);
  box_game->addWidget(_button_docs);

  _button_highscores = new QPushButton("High Scores", frame_game);
  connect(_button_highscores, &QPushButton::clicked, this,
          &MainWindow::slotHighscores);
  box_game->addWidget(_button_highscores);

  _button_newgamequit = new QPushButton("New &Game", frame_game);
  connect(_button_newgamequit, &QPushButton::clicked, this,
          &MainWindow::slotNewGameQuit);
  box_game->addWidget(_button_newgamequit);

  vbox_middle->addWidget(frame_game);
  vbox_middle->addStretch();

  QVBoxLayout* vbox_right = new QVBoxLayout();
  vbox_right->setSpacing(5);
  hbox_down->addLayout(vbox_right);

  _group_pocket =
      new QGroupBox(QString::fromStdString(std::format(
                        "You pants pocket ({}/{})", _gameState.pocket,
                        _gameState.pocket_capacity)),
                    this);
  QVBoxLayout* vbox_pocket = new QVBoxLayout(_group_pocket);
  vbox_pocket->setContentsMargins(5, 5, 5, 5);
  _label_pocket = new QLabel(_group_pocket);
  _label_pocket->hide();
  _setLabelPocket(_gameState.pocket, _gameState.pocket_capacity);

  _treeview_pocket = create_treeview_drug(false);
  _treeview_pocket->setMinimumSize(250, 180);
  vbox_pocket->addWidget(_treeview_pocket);
  vbox_right->addWidget(_group_pocket);

  QGroupBox* frame_status = new QGroupBox("Status", this);
  QVBoxLayout* vbox_status = new QVBoxLayout(frame_status);
  vbox_status->setContentsMargins(5, 5, 5, 5);
  vbox_status->setSpacing(5);

  QHBoxLayout* box_loc = new QHBoxLayout();
  box_loc->setSpacing(10);
  QLabel* label_loc_title = new QLabel("Location:", frame_status);
  box_loc->addWidget(label_loc_title);
  _label_location = new QLabel(frame_status);
  _setLabelLocation(_gameState.location);
  box_loc->addWidget(_label_location);
  box_loc->addStretch();
  vbox_status->addLayout(box_loc);

  QHBoxLayout* box_health = new QHBoxLayout();
  box_health->setSpacing(10);
  QLabel* label_health_title = new QLabel("Health:", frame_status);
  box_health->addWidget(label_health_title);
  _progressbar_health = new QProgressBar(frame_status);
  _progressbar_health->setRange(0, 100);
  _progressbar_health->setValue(_gameState.health);
  _progressbar_health->setTextVisible(false);
  box_health->addWidget(_progressbar_health);
  vbox_status->addLayout(box_health);

  QHBoxLayout* box_day_rank = new QHBoxLayout();
  box_day_rank->setSpacing(30);

  QHBoxLayout* box_day = new QHBoxLayout();
  box_day->setSpacing(10);
  QLabel* label_day_title = new QLabel("Day:", frame_status);
  box_day->addWidget(label_day_title);
  _label_day = new QLabel(frame_status);
  _setLabelDay(_gameState.day);
  box_day->addWidget(_label_day);
  box_day_rank->addLayout(box_day);

  QHBoxLayout* box_rank = new QHBoxLayout();
  box_rank->setSpacing(10);
  QLabel* label_rank_title = new QLabel("Rank:", frame_status);
  box_rank->addWidget(label_rank_title);
  _label_rank = new QLabel(frame_status);
  _setLabelRank(_gameState.rank);
  box_rank->addWidget(_label_rank);
  box_day_rank->addLayout(box_rank);
  box_day_rank->addStretch();
  vbox_status->addLayout(box_day_rank);

  QHBoxLayout* box_money_status = new QHBoxLayout();
  box_money_status->setSpacing(0);

  QGridLayout* grid_money = new QGridLayout();
  grid_money->setHorizontalSpacing(50);
  grid_money->setVerticalSpacing(5);

  QLabel* label_cash_title = new QLabel("Cash:", frame_status);
  grid_money->addWidget(label_cash_title, 0, 0, Qt::AlignLeft);
  _label_cash = new QLabel(frame_status);
  _setLabelCash(_gameState.cash);
  grid_money->addWidget(_label_cash, 0, 1, Qt::AlignRight);

  QLabel* label_bank_title = new QLabel("Bank:", frame_status);
  grid_money->addWidget(label_bank_title, 1, 0, Qt::AlignLeft);
  _label_bank = new QLabel(frame_status);
  _setLabelBank(_gameState.bank);
  grid_money->addWidget(_label_bank, 1, 1, Qt::AlignRight);

  QLabel* label_debt_title = new QLabel("Debt:", frame_status);
  grid_money->addWidget(label_debt_title, 2, 0, Qt::AlignLeft);
  _label_debt = new QLabel(frame_status);
  _setLabelDebt(_gameState.debt);
  grid_money->addWidget(_label_debt, 2, 1, Qt::AlignRight);

  box_money_status->addLayout(grid_money);

  _drawingarea_status = new QWidget(frame_status);
  box_money_status->addWidget(_drawingarea_status);
  vbox_status->addLayout(box_money_status);

  vbox_right->addWidget(frame_status);

  _shortcut_quit = new QShortcut(QKeySequence::Quit, this);
  connect(_shortcut_quit, &QShortcut::activated, this, &QWidget::close);

  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

QMenu* MainWindow::_createPlacesMenu(QPushButton* button) {
  QMenu* menu = new QMenu(button);
  menu->addAction("Finances...", this, &MainWindow::slotPlacesFinances);
  menu->addAction("Shopping...", this, &MainWindow::slotPlacesShopping);
  menu->addAction("Hospital...", this, &MainWindow::slotPlacesHospital);
  menu->addAction("Vault...", this, &MainWindow::slotPlacesVault);
  menu->addAction("Shipping...", this, &MainWindow::slotPlacesShipping);
  button->setMenu(menu);
  return menu;
}

QMenu* MainWindow::_createInfoMenu(QPushButton* button) {
  QMenu* menu = new QMenu(button);
  menu->addAction("Vaults...", this, &MainWindow::slotInfoVaults);
  menu->addAction("World Drug Prices...", this,
                  &MainWindow::slotInfoWorldDrugPrices);
  menu->addAction("World Cities...", this, &MainWindow::slotInfoWorldCities);
  menu->addAction("Shipment Status...", this,
                  &MainWindow::slotInfoShipmentStatus);
  menu->addAction("History...", this, &MainWindow::slotInfoHistory);
  button->setMenu(menu);
  return menu;
}

// MainWindow control methods
void MainWindow::showFinance() { menuitem_places_finances_activate_cb(*this); }
void MainWindow::showShopping() { menuitem_places_shopping_activate_cb(*this); }
void MainWindow::showHospital() { menuitem_places_hospital_activate_cb(*this); }
void MainWindow::showVault() { menuitem_places_vault_activate_cb(*this); }
void MainWindow::showWorldDrugPrices() {
  menuitem_info_world_drug_prices_activate_cb(*this);
}
void MainWindow::showWorldCities() {
  menuitem_info_world_cities_activate_cb(*this);
}
void MainWindow::showShipping() { menuitem_places_shipping_activate_cb(*this); }
void MainWindow::showVaultsInfo() { menuitem_info_vaults_activate_cb(*this); }
void MainWindow::showShipmentStatus() {
  menuitem_info_shipment_status_activate_cb(*this);
}
void MainWindow::showHistory() { menuitem_info_history_activate_cb(*this); }
void MainWindow::showFlyAway() { window_main_button_flyaway_clicked_cb(*this); }
void MainWindow::showAbout() { window_main_button_about_clicked_cb(*this); }
void MainWindow::showDocs() { window_main_button_docs_clicked_cb(*this); }
void MainWindow::showHighscores() {
  window_main_button_highscores_clicked_cb(*this);
}
void MainWindow::buyDrug() { window_main_button_buy_clicked_cb(*this); }
void MainWindow::sellDrug() { window_main_button_sell_clicked_cb(*this); }
void MainWindow::dumpDrug() { window_main_button_dump_clicked_cb(*this); }
void MainWindow::stayHere() { window_main_button_stayhere_clicked_cb(*this); }
void MainWindow::newGame() { window_main_button_newgamequit_clicked_cb(*this); }

// MainWindow slots
void MainWindow::slotBuy() { buyDrug(); }
void MainWindow::slotSell() { sellDrug(); }
void MainWindow::slotDump() { dumpDrug(); }
void MainWindow::slotPlacesFinances() { showFinance(); }
void MainWindow::slotPlacesShopping() { showShopping(); }
void MainWindow::slotPlacesHospital() { showHospital(); }
void MainWindow::slotPlacesVault() { showVault(); }
void MainWindow::slotPlacesShipping() { showShipping(); }
void MainWindow::slotInfoVaults() { showVaultsInfo(); }
void MainWindow::slotInfoWorldDrugPrices() { showWorldDrugPrices(); }
void MainWindow::slotInfoWorldCities() { showWorldCities(); }
void MainWindow::slotInfoShipmentStatus() { showShipmentStatus(); }
void MainWindow::slotInfoHistory() { showHistory(); }
void MainWindow::slotStayHere() { stayHere(); }
void MainWindow::slotFlyAway() { showFlyAway(); }
void MainWindow::slotAbout() { showAbout(); }
void MainWindow::slotDocs() { showDocs(); }
void MainWindow::slotHighscores() { showHighscores(); }
void MainWindow::slotNewGameQuit() { newGame(); }

// WindowFinance Implementation

WindowFinance::WindowFinance(GameState& gameState, QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowFinance::updateFinanceLabels() {
  if (_label_cash) {
    _label_cash->setText(QString::fromStdString(money_string(_gameState.cash)));
  }
  if (_label_bank) {
    _label_bank->setText(QString::fromStdString(money_string(_gameState.bank)));
  }
  if (_label_debt) {
    _label_debt->setText(QString::fromStdString(money_string(_gameState.debt)));
  }
}

void WindowFinance::onDoItClicked() {
  int amount = _spinbutton_amount->value();

  if (_radiobutton_depositsome->isChecked()) {
    amount = std::min(amount, _gameState.cash);
    _gameState.cash -= amount;
    _gameState.bank += amount;
  } else if (_radiobutton_depositall->isChecked()) {
    _gameState.bank += _gameState.cash;
    _gameState.cash = 0;
  } else if (_radiobutton_depositallbut->isChecked()) {
    if (_gameState.cash > amount) {
      int deposit = _gameState.cash - amount;
      _gameState.cash = amount;
      _gameState.bank += deposit;
    }
  } else if (_radiobutton_withdrawsome->isChecked()) {
    amount = std::min(amount, _gameState.bank);
    _gameState.bank -= amount;
    _gameState.cash += amount;
  } else if (_radiobutton_withdrawall->isChecked()) {
    _gameState.cash += _gameState.bank;
    _gameState.bank = 0;
  } else if (_radiobutton_withdrawallbut->isChecked()) {
    if (_gameState.bank > amount) {
      int withdraw = _gameState.bank - amount;
      _gameState.bank = amount;
      _gameState.cash += withdraw;
    }
  }

  updateFinanceLabels();
  emit stateChanged();
}

void WindowFinance::onBorrowClicked() {
  int amount = 2000;
  _gameState.cash += amount;
  _gameState.debt += amount;
  updateFinanceLabels();
  emit stateChanged();
}

void WindowFinance::onRepayClicked() {
  int amount = std::min(_gameState.cash, _gameState.debt);
  _gameState.cash -= amount;
  _gameState.debt -= amount;
  updateFinanceLabels();
  emit stateChanged();
}

void WindowFinance::_setupWidget() {
  setWindowTitle("Finance");
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox* frame_bank = new QGroupBox("Bank", this);
  QVBoxLayout* vbox_bank = new QVBoxLayout(frame_bank);
  vbox_bank->setContentsMargins(5, 5, 5, 5);
  vbox_bank->setSpacing(3);

  QHBoxLayout* hbox_cash_bank = new QHBoxLayout();
  hbox_cash_bank->setSpacing(130);

  QHBoxLayout* hbox_cash = new QHBoxLayout();
  hbox_cash->setSpacing(20);
  hbox_cash->addWidget(new QLabel("Cash:", frame_bank));
  _label_cash = new QLabel("0", frame_bank);
  hbox_cash->addWidget(_label_cash);
  hbox_cash_bank->addLayout(hbox_cash);

  QHBoxLayout* hbox_bank_val = new QHBoxLayout();
  hbox_bank_val->setSpacing(20);
  hbox_bank_val->addWidget(new QLabel("In Bank:", frame_bank));
  _label_bank = new QLabel("0", frame_bank);
  hbox_bank_val->addWidget(_label_bank);
  hbox_cash_bank->addLayout(hbox_bank_val);
  hbox_cash_bank->addStretch();
  vbox_bank->addLayout(hbox_cash_bank);

  QGridLayout* grid_radios = new QGridLayout();
  grid_radios->setHorizontalSpacing(3);
  grid_radios->setVerticalSpacing(3);

  QButtonGroup* radio_group = new QButtonGroup(frame_bank);
  _radiobutton_depositsome = new QRadioButton("Deposit some", frame_bank);
  _radiobutton_depositall = new QRadioButton("Deposit all", frame_bank);
  _radiobutton_depositallbut = new QRadioButton("Deposit all but", frame_bank);
  _radiobutton_withdrawsome = new QRadioButton("Withdraw some", frame_bank);
  _radiobutton_withdrawall = new QRadioButton("Withdraw all", frame_bank);
  _radiobutton_withdrawallbut =
      new QRadioButton("Withdraw all but", frame_bank);

  radio_group->addButton(_radiobutton_depositsome);
  radio_group->addButton(_radiobutton_depositall);
  radio_group->addButton(_radiobutton_depositallbut);
  radio_group->addButton(_radiobutton_withdrawsome);
  radio_group->addButton(_radiobutton_withdrawall);
  radio_group->addButton(_radiobutton_withdrawallbut);
  _radiobutton_depositsome->setChecked(true);

  grid_radios->addWidget(_radiobutton_depositsome, 0, 0);
  grid_radios->addWidget(_radiobutton_depositall, 0, 1);
  grid_radios->addWidget(_radiobutton_depositallbut, 0, 2);
  grid_radios->addWidget(_radiobutton_withdrawsome, 1, 0);
  grid_radios->addWidget(_radiobutton_withdrawall, 1, 1);
  grid_radios->addWidget(_radiobutton_withdrawallbut, 1, 2);
  vbox_bank->addLayout(grid_radios);

  QHBoxLayout* hbox_amount = new QHBoxLayout();
  hbox_amount->setSpacing(10);
  hbox_amount->addWidget(new QLabel("Amount", frame_bank));
  _spinbutton_amount = new QSpinBox(frame_bank);
  _spinbutton_amount->setRange(1, 1000000000);
  _spinbutton_amount->setValue(1);
  hbox_amount->addWidget(_spinbutton_amount);
  _button_doit = new QPushButton("Do it!", frame_bank);
  hbox_amount->addWidget(_button_doit);
  hbox_amount->addStretch();
  vbox_bank->addLayout(hbox_amount);

  vbox_main->addWidget(frame_bank);

  QGroupBox* frame_loans = new QGroupBox("Loans", this);
  QVBoxLayout* vbox_loans = new QVBoxLayout(frame_loans);
  vbox_loans->setContentsMargins(5, 5, 5, 5);
  vbox_loans->setSpacing(5);

  _treeview_loan = new QTreeWidget(frame_loans);
  _treeview_loan->setRootIsDecorated(false);
  _treeview_loan->setColumnCount(6);
  _treeview_loan->setHeaderLabels(
      {"Name", "Pays", "Rate", "Days", "Debt", "Days Left"});
  for (int col = 0; col < 6; ++col) {
    _treeview_loan->setColumnWidth(col, 70);
    _treeview_loan->headerItem()->setTextAlignment(
        col, Qt::AlignRight | Qt::AlignVCenter);
  }
  _treeview_loan->setFixedHeight(170);
  vbox_loans->addWidget(_treeview_loan);

  QHBoxLayout* hbox_debt_actions = new QHBoxLayout();
  hbox_debt_actions->setSpacing(0);

  QHBoxLayout* hbox_debt = new QHBoxLayout();
  hbox_debt->setSpacing(20);
  hbox_debt->addWidget(new QLabel("Debt:", frame_loans));
  _label_debt = new QLabel("0", frame_loans);
  hbox_debt->addWidget(_label_debt);
  hbox_debt_actions->addLayout(hbox_debt);

  hbox_debt_actions->addStretch();

  QHBoxLayout* hbox_borrow_repay = new QHBoxLayout();
  hbox_borrow_repay->setSpacing(3);
  _button_borrow = new QPushButton("Borrow", frame_loans);
  hbox_borrow_repay->addWidget(_button_borrow);
  _button_repay = new QPushButton("Repay", frame_loans);
  hbox_borrow_repay->addWidget(_button_repay);
  hbox_debt_actions->addLayout(hbox_borrow_repay);

  vbox_loans->addLayout(hbox_debt_actions);
  vbox_main->addWidget(frame_loans);

  QHBoxLayout* hbox_done = new QHBoxLayout();
  hbox_done->setSpacing(5);
  hbox_done->addStretch();
  _button_done = new QPushButton("Done", this);
  connect(_button_done, &QPushButton::clicked, this, &QDialog::close);
  hbox_done->addWidget(_button_done);
  vbox_main->addLayout(hbox_done);

  connect(_button_doit, &QPushButton::clicked, this,
          &WindowFinance::onDoItClicked);
  connect(_button_borrow, &QPushButton::clicked, this,
          &WindowFinance::onBorrowClicked);
  connect(_button_repay, &QPushButton::clicked, this,
          &WindowFinance::onRepayClicked);

  updateFinanceLabels();
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowShopping Implementation

WindowShopping::WindowShopping(GameState& gameState, QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowShopping::updateShopping() {
  if (_label_cash) {
    _label_cash->setText(QString::fromStdString(money_string(_gameState.cash)));
  }
  if (_treeview_store) {
    _treeview_store->clear();
    for (int i = 0; i < SHOP_ITEM_NUM; ++i) {
      if (!shop_items[i].name.empty()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_store);
        item->setText(COLUMN_STORE_NAME,
                      QString::fromStdString(shop_items[i].name));
        QString type_str =
            shop_items[i].armor > 0
                ? "Armor"
                : (shop_items[i].hit_chance > 0 ? "Weapon" : "Item");
        item->setText(COLUMN_STORE_TYPE, type_str);
        item->setText(
            COLUMN_STORE_PRICE,
            QString::fromStdString(money_string(shop_items[i].weapon_price)));
        item->setData(0, Qt::UserRole, i);
        item->setData(0, Qt::UserRole + 1, false);
      }
      if (!shop_items[i].ammo_name.empty()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_store);
        item->setText(COLUMN_STORE_NAME,
                      QString::fromStdString(shop_items[i].ammo_name));
        item->setText(COLUMN_STORE_TYPE, "Ammo");
        item->setText(
            COLUMN_STORE_PRICE,
            QString::fromStdString(money_string(shop_items[i].ammo_price)));
        item->setData(0, Qt::UserRole, i);
        item->setData(0, Qt::UserRole + 1, true);
      }
    }
  }
  if (_treeview_inventory) {
    _treeview_inventory->clear();
    for (int i = 0; i < SHOP_ITEM_NUM; ++i) {
      if (_gameState.weapon_qty[i] > 0) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_inventory);
        item->setText(COLUMN_INVENTORY_NAME,
                      QString::fromStdString(shop_items[i].name));
        QString type_str =
            shop_items[i].armor > 0
                ? "Armor"
                : (shop_items[i].hit_chance > 0 ? "Weapon" : "Item");
        item->setText(COLUMN_INVENTORY_TYPE, type_str);
        item->setText(COLUMN_INVENTORY_QTY,
                      QString::number(_gameState.weapon_qty[i]));
        item->setText(
            COLUMN_INVENTORY_SELLFOR,
            QString::fromStdString(money_string(shop_items[i].weapon_price)));
        item->setData(0, Qt::UserRole, i);
        item->setData(0, Qt::UserRole + 1, false);
      }
      if (_gameState.ammo_qty[i] > 0) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_inventory);
        item->setText(COLUMN_INVENTORY_NAME,
                      QString::fromStdString(shop_items[i].ammo_name));
        item->setText(COLUMN_INVENTORY_TYPE, "Ammo");
        item->setText(COLUMN_INVENTORY_QTY,
                      QString::number(_gameState.ammo_qty[i]));
        item->setText(
            COLUMN_INVENTORY_SELLFOR,
            QString::fromStdString(money_string(shop_items[i].ammo_price)));
        item->setData(0, Qt::UserRole, i);
        item->setData(0, Qt::UserRole + 1, true);
      }
    }
  }
  onStoreItemSelectionChanged();
  onInventoryItemSelectionChanged();
}

void WindowShopping::onStoreItemSelectionChanged() {
  QList<QTreeWidgetItem*> sel = _treeview_store->selectedItems();
  if (sel.isEmpty()) {
    _button_buy->setEnabled(false);
    return;
  }
  int idx = sel.first()->data(0, Qt::UserRole).toInt();
  bool is_ammo = sel.first()->data(0, Qt::UserRole + 1).toBool();
  if (is_ammo) {
    int price = shop_items[idx].ammo_price;
    int cur_ammo = _gameState.ammo_qty[idx];
    int max_ammo = shop_items[idx].ammo_limit;
    _button_buy->setEnabled(_gameState.cash >= price && cur_ammo < max_ammo);
  } else {
    int price = shop_items[idx].weapon_price;
    int cur_qty = _gameState.weapon_qty[idx];
    int max_qty = shop_items[idx].weapon_limit;
    _button_buy->setEnabled(_gameState.cash >= price && cur_qty < max_qty);
  }
}

void WindowShopping::onInventoryItemSelectionChanged() {
  QList<QTreeWidgetItem*> sel = _treeview_inventory->selectedItems();
  _button_sell->setEnabled(!sel.isEmpty());
}

void WindowShopping::onBuyClicked() {
  QList<QTreeWidgetItem*> sel = _treeview_store->selectedItems();
  if (sel.isEmpty()) return;
  int idx = sel.first()->data(0, Qt::UserRole).toInt();
  bool is_ammo = sel.first()->data(0, Qt::UserRole + 1).toBool();

  if (is_ammo) {
    int price = shop_items[idx].ammo_price;
    int cur_ammo = _gameState.ammo_qty[idx];
    int max_ammo = shop_items[idx].ammo_limit;
    int max_buy = std::min(max_ammo - cur_ammo, _gameState.cash / price);
    if (max_buy <= 0) {
      QMessageBox::information(this, "Shopping",
                               "You cannot buy any more of that!");
      return;
    }
    int qty = 1;
    if (max_buy > 1) {
      bool ok = false;
      qty = QInputDialog::getInt(
          this, "Buy Ammo",
          QString("How many %1 do you want to buy?")
              .arg(QString::fromStdString(shop_items[idx].ammo_plural)),
          1, 1, max_buy, 1, &ok);
      if (!ok || qty <= 0) return;
    }
    _gameState.cash -= qty * price;
    _gameState.ammo_qty[idx] += qty;
  } else {
    int price = shop_items[idx].weapon_price;
    int cur_qty = _gameState.weapon_qty[idx];
    int max_qty = shop_items[idx].weapon_limit;
    int max_buy = std::min(max_qty - cur_qty, _gameState.cash / price);
    if (max_buy <= 0) {
      QMessageBox::information(this, "Shopping",
                               "You cannot buy any more of that!");
      return;
    }
    int qty = 1;
    if (max_buy > 1) {
      bool ok = false;
      qty = QInputDialog::getInt(
          this, "Buy Item",
          QString("How many %1 do you want to buy?")
              .arg(QString::fromStdString(shop_items[idx].plural)),
          1, 1, max_buy, 1, &ok);
      if (!ok || qty <= 0) return;
    }
    _gameState.cash -= qty * price;
    _gameState.weapon_qty[idx] += qty;
  }
  updateShopping();
  emit stateChanged();
}

void WindowShopping::onSellClicked() {
  QList<QTreeWidgetItem*> sel = _treeview_inventory->selectedItems();
  if (sel.isEmpty()) return;
  int idx = sel.first()->data(0, Qt::UserRole).toInt();
  bool is_ammo = sel.first()->data(0, Qt::UserRole + 1).toBool();

  if (is_ammo) {
    int price = shop_items[idx].ammo_price;
    int owned = _gameState.ammo_qty[idx];
    if (owned <= 0) return;
    int qty = 1;
    if (owned > 1) {
      bool ok = false;
      qty = QInputDialog::getInt(
          this, "Sell Ammo",
          QString("How many %1 do you want to sell?")
              .arg(QString::fromStdString(shop_items[idx].ammo_plural)),
          1, 1, owned, 1, &ok);
      if (!ok || qty <= 0) return;
    }
    _gameState.cash += qty * price;
    _gameState.ammo_qty[idx] -= qty;
  } else {
    int price = shop_items[idx].weapon_price;
    int owned = _gameState.weapon_qty[idx];
    if (owned <= 0) return;
    int qty = 1;
    if (owned > 1) {
      bool ok = false;
      qty = QInputDialog::getInt(
          this, "Sell Item",
          QString("How many %1 do you want to sell?")
              .arg(QString::fromStdString(shop_items[idx].plural)),
          1, 1, owned, 1, &ok);
      if (!ok || qty <= 0) return;
    }
    _gameState.cash += qty * price;
    _gameState.weapon_qty[idx] -= qty;
  }
  updateShopping();
  emit stateChanged();
}

void WindowShopping::_setupWidget() {
  setWindowTitle("Shopping");
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox* frame_store = new QGroupBox("Store's Inventory", this);
  QVBoxLayout* vbox_store = new QVBoxLayout(frame_store);
  vbox_store->setContentsMargins(5, 5, 5, 5);
  vbox_store->setSpacing(5);

  _treeview_store = new QTreeWidget(frame_store);
  _treeview_store->setRootIsDecorated(false);
  _treeview_store->setColumnCount(3);
  _treeview_store->setHeaderLabels({"Name", "Type", "Price"});
  _treeview_store->setColumnWidth(COLUMN_STORE_NAME, 160);
  _treeview_store->setColumnWidth(COLUMN_STORE_TYPE, 80);
  _treeview_store->setColumnWidth(COLUMN_STORE_PRICE, 70);
  _treeview_store->headerItem()->setTextAlignment(
      COLUMN_STORE_NAME, Qt::AlignLeft | Qt::AlignVCenter);
  _treeview_store->headerItem()->setTextAlignment(
      COLUMN_STORE_TYPE, Qt::AlignCenter | Qt::AlignVCenter);
  _treeview_store->headerItem()->setTextAlignment(
      COLUMN_STORE_PRICE, Qt::AlignRight | Qt::AlignVCenter);
  _treeview_store->setFixedHeight(220);
  vbox_store->addWidget(_treeview_store);

  QHBoxLayout* hbox_buy = new QHBoxLayout();
  hbox_buy->addStretch();
  _button_buy = new QPushButton("   Buy   ", frame_store);
  _button_buy->setEnabled(false);
  hbox_buy->addWidget(_button_buy);
  vbox_store->addLayout(hbox_buy);

  vbox_main->addWidget(frame_store);

  QGroupBox* frame_inv = new QGroupBox("Your Inventory", this);
  QVBoxLayout* vbox_inv = new QVBoxLayout(frame_inv);
  vbox_inv->setContentsMargins(5, 5, 5, 5);
  vbox_inv->setSpacing(5);

  _treeview_inventory = new QTreeWidget(frame_inv);
  _treeview_inventory->setRootIsDecorated(false);
  _treeview_inventory->setColumnCount(4);
  _treeview_inventory->setHeaderLabels({"Name", "Type", "Qty", "Price"});
  _treeview_inventory->setColumnWidth(COLUMN_INVENTORY_NAME, 160);
  _treeview_inventory->setColumnWidth(COLUMN_INVENTORY_TYPE, 80);
  _treeview_inventory->setColumnWidth(COLUMN_INVENTORY_QTY, 50);
  _treeview_inventory->setColumnWidth(COLUMN_INVENTORY_SELLFOR, 70);
  _treeview_inventory->headerItem()->setTextAlignment(
      COLUMN_INVENTORY_NAME, Qt::AlignLeft | Qt::AlignVCenter);
  _treeview_inventory->headerItem()->setTextAlignment(
      COLUMN_INVENTORY_TYPE, Qt::AlignCenter | Qt::AlignVCenter);
  _treeview_inventory->headerItem()->setTextAlignment(
      COLUMN_INVENTORY_QTY, Qt::AlignRight | Qt::AlignVCenter);
  _treeview_inventory->headerItem()->setTextAlignment(
      COLUMN_INVENTORY_SELLFOR, Qt::AlignRight | Qt::AlignVCenter);
  _treeview_inventory->setFixedHeight(150);
  vbox_inv->addWidget(_treeview_inventory);

  QHBoxLayout* hbox_sell_cash = new QHBoxLayout();
  hbox_sell_cash->setSpacing(0);

  QHBoxLayout* hbox_cash = new QHBoxLayout();
  hbox_cash->setSpacing(20);
  hbox_cash->addWidget(new QLabel("Cash:", frame_inv));
  _label_cash = new QLabel("0", frame_inv);
  hbox_cash->addWidget(_label_cash);
  hbox_sell_cash->addLayout(hbox_cash);

  hbox_sell_cash->addStretch();

  _button_sell = new QPushButton("   Sell   ", frame_inv);
  _button_sell->setEnabled(false);
  hbox_sell_cash->addWidget(_button_sell);
  vbox_inv->addLayout(hbox_sell_cash);

  vbox_main->addWidget(frame_inv);

  QHBoxLayout* hbox_done = new QHBoxLayout();
  hbox_done->setSpacing(5);
  hbox_done->addStretch();
  _button_done = new QPushButton("Done", this);
  connect(_button_done, &QPushButton::clicked, this, &QDialog::close);
  hbox_done->addWidget(_button_done);
  vbox_main->addLayout(hbox_done);

  connect(_treeview_store, &QTreeWidget::itemSelectionChanged, this,
          &WindowShopping::onStoreItemSelectionChanged);
  connect(_treeview_inventory, &QTreeWidget::itemSelectionChanged, this,
          &WindowShopping::onInventoryItemSelectionChanged);
  connect(_button_buy, &QPushButton::clicked, this,
          &WindowShopping::onBuyClicked);
  connect(_button_sell, &QPushButton::clicked, this,
          &WindowShopping::onSellClicked);

  updateShopping();
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowHospital Implementation

WindowHospital::WindowHospital(GameState& gameState, QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowHospital::onSliderValueChanged(int val) {
  int needed = std::max(0, val - _gameState.health);
  int cost = needed * 50;
  if (_progressbar_health) {
    _progressbar_health->setValue(val);
  }
  if (_label_cost) {
    _label_cost->setText(QString::fromStdString(money_string(cost)));
  }
}

void WindowHospital::onOkClicked() {
  int target_health = _scalebutton_health->value();
  int needed = std::max(0, target_health - _gameState.health);
  int cost = needed * 50;
  if (_gameState.cash >= cost) {
    _gameState.cash -= cost;
    _gameState.health = target_health;
    emit stateChanged();
    close();
  }
}

void WindowHospital::_setupWidget() {
  setWindowTitle("Hospital");
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout* hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox* frame_icon = new QGroupBox("", this);
  QHBoxLayout* hbox_icon = new QHBoxLayout(frame_icon);
  hbox_icon->setContentsMargins(20, 20, 20, 20);
  QLabel* image_icon = new QLabel(frame_icon);
  image_icon->setPixmap(
      style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(48, 48));
  hbox_icon->addWidget(image_icon);
  hbox_top->addWidget(frame_icon);

  QGroupBox* frame_slider =
      new QGroupBox("Move the pointer to desired health", this);
  frame_slider->setFixedWidth(400);
  QVBoxLayout* vbox_slider = new QVBoxLayout(frame_slider);
  vbox_slider->setContentsMargins(5, 5, 5, 5);
  vbox_slider->setSpacing(10);

  QVBoxLayout* vbox_bars = new QVBoxLayout();
  vbox_bars->setSpacing(3);
  _progressbar_health = new QProgressBar(frame_slider);
  _progressbar_health->setRange(0, 100);
  _progressbar_health->setValue(_gameState.health);
  _progressbar_health->setTextVisible(false);
  vbox_bars->addWidget(_progressbar_health);

  _scalebutton_health = new QSlider(Qt::Horizontal, frame_slider);
  _scalebutton_health->setRange(_gameState.health, 100);
  _scalebutton_health->setValue(_gameState.health);
  vbox_bars->addWidget(_scalebutton_health);
  vbox_slider->addLayout(vbox_bars);

  QGridLayout* grid_labels = new QGridLayout();
  grid_labels->setHorizontalSpacing(10);
  grid_labels->setVerticalSpacing(3);

  QLabel* label_cash_title = new QLabel("Cash:", frame_slider);
  grid_labels->addWidget(label_cash_title, 0, 0, Qt::AlignRight);
  _label_cash = new QLabel(
      QString::fromStdString(money_string(_gameState.cash)), frame_slider);
  grid_labels->addWidget(_label_cash, 0, 1, Qt::AlignLeft);

  QLabel* label_cost_title = new QLabel("Cost for treatment:", frame_slider);
  grid_labels->addWidget(label_cost_title, 1, 0, Qt::AlignRight);
  _label_cost = new QLabel("0", frame_slider);
  grid_labels->addWidget(_label_cost, 1, 1, Qt::AlignLeft);
  vbox_slider->addLayout(grid_labels);

  hbox_top->addWidget(frame_slider);
  vbox_main->addLayout(hbox_top);

  QHBoxLayout* hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(3);
  hbox_buttons->addStretch();

  _button_ok = new QPushButton("&OK", this);
  hbox_buttons->addWidget(_button_ok);

  _button_cancel = new QPushButton("&Cancel", this);
  connect(_button_cancel, &QPushButton::clicked, this, &QDialog::close);
  hbox_buttons->addWidget(_button_cancel);

  vbox_main->addLayout(hbox_buttons);

  connect(_scalebutton_health, &QSlider::valueChanged, this,
          &WindowHospital::onSliderValueChanged);
  connect(_button_ok, &QPushButton::clicked, this,
          &WindowHospital::onOkClicked);

  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowVault Implementation

WindowVault::WindowVault(GameState& gameState, QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowVault::updateVaultLists() {
  if (_frame_pocket) {
    _frame_pocket->setTitle(QString::fromStdString(
        std::format("You pants pocket ({}/{})", _gameState.pocket,
                    _gameState.pocket_capacity)));
  }
  if (_treeview_pocket) {
    _treeview_pocket->clear();
    for (int i = 0; i < DRUG_NUM; ++i) {
      if (_gameState.player_qty[i] > 0) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_pocket);
        item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
        item->setText(1, QString::number(_gameState.player_qty[i]));
        item->setText(2, QString::fromStdString(
                             money_string(_gameState.player_price[i])));
        item->setData(0, Qt::UserRole, i);
      }
    }
  }
  if (_treeview_vault) {
    _treeview_vault->clear();
    for (int i = 0; i < DRUG_NUM; ++i) {
      if (_gameState.vault_qty[i] > 0) {
        QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_vault);
        item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
        item->setText(1, QString::number(_gameState.vault_qty[i]));
        item->setText(2, QString::fromStdString(
                             money_string(_gameState.player_price[i])));
        item->setData(0, Qt::UserRole, i);
      }
    }
  }
}

void WindowVault::onIntoVaultClicked() {
  if (!_treeview_pocket) return;
  auto* item = _treeview_pocket->currentItem();
  if (!item) return;
  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx >= 0 && drug_idx < DRUG_NUM &&
      _gameState.player_qty[drug_idx] > 0) {
    _gameState.player_qty[drug_idx] -= 1;
    _gameState.pocket -= 1;
    _gameState.vault_qty[drug_idx] += 1;
    updateVaultLists();
    emit stateChanged();
  }
}

void WindowVault::onFromVaultClicked() {
  if (!_treeview_vault) return;
  auto* item = _treeview_vault->currentItem();
  if (!item) return;
  int drug_idx = item->data(0, Qt::UserRole).toInt();
  if (drug_idx >= 0 && drug_idx < DRUG_NUM &&
      _gameState.vault_qty[drug_idx] > 0) {
    if (_gameState.pocket < _gameState.pocket_capacity) {
      _gameState.vault_qty[drug_idx] -= 1;
      _gameState.player_qty[drug_idx] += 1;
      _gameState.pocket += 1;
      updateVaultLists();
      emit stateChanged();
    }
  }
}

void WindowVault::_setupWidget() {
  setWindowTitle("The Vault");
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout* hbox_middle = new QHBoxLayout();
  hbox_middle->setSpacing(5);

  _frame_pocket =
      new QGroupBox(QString::fromStdString(std::format(
                        "You pants pocket ({}/{})", _gameState.pocket,
                        _gameState.pocket_capacity)),
                    this);
  QVBoxLayout* vbox_pocket = new QVBoxLayout(_frame_pocket);
  vbox_pocket->setContentsMargins(5, 5, 5, 5);
  _treeview_pocket = create_treeview_drug(false);
  _treeview_pocket->setFixedHeight(200);
  vbox_pocket->addWidget(_treeview_pocket);
  hbox_middle->addWidget(_frame_pocket);

  QGroupBox* frame_move = new QGroupBox("Move", this);
  QVBoxLayout* vbox_move = new QVBoxLayout(frame_move);
  vbox_move->setContentsMargins(5, 5, 5, 5);
  vbox_move->setSpacing(3);
  _button_intovault =
      new QPushButton(QString::fromUtf8("Into Vault \u2192"), frame_move);
  vbox_move->addWidget(_button_intovault);
  _button_fromvault =
      new QPushButton(QString::fromUtf8("\u2190 From Vault"), frame_move);
  vbox_move->addWidget(_button_fromvault);
  vbox_move->addStretch();
  hbox_middle->addWidget(frame_move);

  QGroupBox* frame_vault = new QGroupBox("In the Vault", this);
  QVBoxLayout* vbox_vault = new QVBoxLayout(frame_vault);
  vbox_vault->setContentsMargins(5, 5, 5, 5);
  _treeview_vault = create_treeview_drug(false);
  _treeview_vault->setFixedHeight(210);
  vbox_vault->addWidget(_treeview_vault);
  hbox_middle->addWidget(frame_vault);

  vbox_main->addLayout(hbox_middle);

  QHBoxLayout* hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(3);
  hbox_buttons->addStretch();

  _button_ok = new QPushButton("&OK", this);
  hbox_buttons->addWidget(_button_ok);

  _button_cancel = new QPushButton("&Cancel", this);
  connect(_button_cancel, &QPushButton::clicked, this, &QDialog::close);
  hbox_buttons->addWidget(_button_cancel);

  vbox_main->addLayout(hbox_buttons);

  connect(_button_intovault, &QPushButton::clicked, this,
          &WindowVault::onIntoVaultClicked);
  connect(_button_fromvault, &QPushButton::clicked, this,
          &WindowVault::onFromVaultClicked);
  connect(_button_ok, &QPushButton::clicked, this, &QDialog::close);

  updateVaultLists();
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowWorldDrugPrices Implementation

WindowWorldDrugPrices::WindowWorldDrugPrices(const GameState& gameState,
                                             QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowWorldDrugPrices::fillCityList(int drug_idx) {
  if (!_treeview_city) return;
  _treeview_city->clear();
  int day = _gameState.day;
  for (int city_idx = 0; city_idx < CITY_NUM; ++city_idx) {
    QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_city);
    std::string text = std::format("{}, {}", city_name(city_info[city_idx].id),
                                   country_name(city_info[city_idx].country));
    item->setText(0, QString::fromStdString(text));
    item->setText(
        1, QString::number(_gameState.drug_table[drug_idx][city_idx][day].qty));
    item->setText(2,
                  QString::fromStdString(money_string(
                      _gameState.drug_table[drug_idx][city_idx][day].price)));
    item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
  }
}

void WindowWorldDrugPrices::onDrugItemClicked(QTreeWidgetItem* item,
                                              int column) {
  (void)column;
  if (!item) return;
  int drug_idx = item->data(0, Qt::UserRole).toInt();
  fillCityList(drug_idx);
}

void WindowWorldDrugPrices::_setupWidget() {
  setWindowTitle(QString::fromUtf8("World Drug Prices"));
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout* hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox* frame_drug = new QGroupBox("Drug", this);
  QVBoxLayout* vbox_drug = new QVBoxLayout(frame_drug);
  vbox_drug->setContentsMargins(5, 5, 5, 5);
  _treeview_drug = create_treeview_drug_names();
  _treeview_drug->setFixedHeight(210);
  _treeview_drug->clear();
  for (int i = 0; i < DRUG_NUM; ++i) {
    QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_drug);
    item->setData(0, Qt::UserRole, i);
    item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
  }
  vbox_drug->addWidget(_treeview_drug);
  hbox_top->addWidget(frame_drug);

  QGroupBox* frame_city = new QGroupBox("City List", this);
  QVBoxLayout* vbox_city = new QVBoxLayout(frame_city);
  vbox_city->setContentsMargins(5, 5, 5, 5);
  _treeview_city = create_treeview_city_list();
  _treeview_city->setFixedHeight(210);
  vbox_city->addWidget(_treeview_city);
  hbox_top->addWidget(frame_city);

  vbox_main->addLayout(hbox_top);

  QHBoxLayout* hbox_bottom = new QHBoxLayout();
  hbox_bottom->setSpacing(3);
  hbox_bottom->addStretch();

  _button_close = new QPushButton("&Close", this);
  connect(_button_close, &QPushButton::clicked, this, &QDialog::close);
  hbox_bottom->addWidget(_button_close);

  vbox_main->addLayout(hbox_bottom);

  connect(_treeview_drug, &QTreeWidget::itemClicked, this,
          &WindowWorldDrugPrices::onDrugItemClicked);

  fillCityList(0);
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowWorldCities Implementation

WindowWorldCities::WindowWorldCities(const GameState& gameState,
                                     QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowWorldCities::fillDrugList(int city_idx) {
  if (!_treeview_drug) return;
  _treeview_drug->clear();
  int day = _gameState.day;
  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_drug);
    std::string name_str = drug_name(drug_info[drug_idx].id);
    item->setText(0, QString::fromStdString(name_str));
    item->setText(
        1, QString::number(_gameState.drug_table[drug_idx][city_idx][day].qty));
    item->setText(2,
                  QString::fromStdString(money_string(
                      _gameState.drug_table[drug_idx][city_idx][day].price)));
    item->setData(0, Qt::UserRole, drug_idx);
    item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
  }
}

void WindowWorldCities::onCityItemClicked(QTreeWidgetItem* item, int column) {
  (void)column;
  if (!item) return;
  int city_idx = item->data(0, Qt::UserRole).toInt();
  fillDrugList(city_idx);
}

void WindowWorldCities::_setupWidget() {
  setWindowTitle(QString::fromUtf8("World Cities"));
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout* hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox* frame_city = new QGroupBox("City", this);
  QVBoxLayout* vbox_city = new QVBoxLayout(frame_city);
  vbox_city->setContentsMargins(5, 5, 5, 5);
  _treeview_city = create_treeview_city_names();
  _treeview_city->setFixedHeight(210);
  _treeview_city->clear();
  for (int i = 0; i < CITY_NUM; ++i) {
    QTreeWidgetItem* item = new QTreeWidgetItem(_treeview_city);
    item->setData(0, Qt::UserRole, i);
    std::string text = std::format("{}, {}", city_name(city_info[i].id),
                                   country_name(city_info[i].country));
    if (i == _gameState.location) {
      text += " (Current)";
    }
    item->setText(0, QString::fromStdString(text));
  }
  vbox_city->addWidget(_treeview_city);
  hbox_top->addWidget(frame_city);

  QGroupBox* frame_drug = new QGroupBox("Drug List", this);
  QVBoxLayout* vbox_drug = new QVBoxLayout(frame_drug);
  vbox_drug->setContentsMargins(5, 5, 5, 5);
  _treeview_drug = create_treeview_drug_list();
  _treeview_drug->setFixedHeight(210);
  vbox_drug->addWidget(_treeview_drug);
  hbox_top->addWidget(frame_drug);

  vbox_main->addLayout(hbox_top);

  QHBoxLayout* hbox_bottom = new QHBoxLayout();
  hbox_bottom->setSpacing(3);
  hbox_bottom->addStretch();

  _button_close = new QPushButton("&Close", this);
  connect(_button_close, &QPushButton::clicked, this, &QDialog::close);
  hbox_bottom->addWidget(_button_close);

  vbox_main->addLayout(hbox_bottom);

  connect(_treeview_city, &QTreeWidget::itemClicked, this,
          &WindowWorldCities::onCityItemClicked);

  fillDrugList(_gameState.location);
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowInput Implementation

WindowInput::WindowInput(QWidget* parent) : QDialog(parent) {
  _setupWidget("", "", "");
}

WindowInput::WindowInput(const QString& title, const QString& message,
                         const QString& question, QWidget* parent)
    : QDialog(parent) {
  _setupWidget(title, message, question);
}

void WindowInput::setMessage(const QString& message) {
  if (_label_msg) _label_msg->setText(message);
}

void WindowInput::setQuestion(const QString& question) {
  if (_label_question) _label_question->setText(question);
}

void WindowInput::setRange(int min, int max) {
  if (_spinbutton_value) _spinbutton_value->setRange(min, max);
}

void WindowInput::setValue(int val) {
  if (_spinbutton_value) _spinbutton_value->setValue(val);
}

int WindowInput::value() const {
  return _spinbutton_value ? _spinbutton_value->value() : 0;
}

void WindowInput::_setupWidget(const QString& title, const QString& message,
                               const QString& question) {
  setWindowTitle(title);
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox* frame_msg = new QGroupBox("Message", this);
  QHBoxLayout* hbox_msg = new QHBoxLayout(frame_msg);
  hbox_msg->setContentsMargins(5, 5, 5, 5);
  _label_msg = new QLabel(message, frame_msg);
  hbox_msg->addWidget(_label_msg);
  vbox_main->addWidget(frame_msg);

  QGroupBox* frame_input = new QGroupBox("", this);
  QHBoxLayout* hbox_input = new QHBoxLayout(frame_input);
  hbox_input->setContentsMargins(5, 5, 5, 5);
  hbox_input->setSpacing(5);

  _label_question = new QLabel(question, frame_input);
  _label_question->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  _label_question->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Preferred);
  hbox_input->addWidget(_label_question);

  _spinbutton_value = new QSpinBox(frame_input);
  _spinbutton_value->setRange(1, 1000000);
  _spinbutton_value->setValue(1);
  hbox_input->addWidget(_spinbutton_value);
  vbox_main->addWidget(frame_input);

  QHBoxLayout* hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(5);
  hbox_buttons->addStretch();

  _button_ok = new QPushButton("&OK", this);
  hbox_buttons->addWidget(_button_ok);

  _button_cancel = new QPushButton("&Cancel", this);
  connect(_button_cancel, &QPushButton::clicked, this, &QDialog::reject);
  hbox_buttons->addWidget(_button_cancel);

  vbox_main->addLayout(hbox_buttons);

  connect(_button_ok, &QPushButton::clicked, this, &QDialog::accept);

  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

// WindowFlyAway Implementation

WindowFlyAway::WindowFlyAway(GameState& gameState, QWidget* parent)
    : QDialog(parent), _gameState(gameState) {
  _setupWidget();
}

void WindowFlyAway::_setupWidget() {
  setWindowTitle("Fly Away");
  setModal(true);

  QVBoxLayout* vbox_main = new QVBoxLayout(this);
  vbox_main->setContentsMargins(10, 10, 10, 10);
  vbox_main->setSpacing(8);

  QHBoxLayout* hbox_header = new QHBoxLayout();
  QLabel* label_dest = new QLabel("Select destination city:", this);
  hbox_header->addWidget(label_dest);
  hbox_header->addStretch();
  _label_cash = new QLabel(QString::fromStdString(std::format(
                               "Cash: ${}", money_string(_gameState.cash))),
                           this);
  hbox_header->addWidget(_label_cash);
  vbox_main->addLayout(hbox_header);

  _city_list = new QListWidget(this);
  _populateCityList();
  vbox_main->addWidget(_city_list);

  QHBoxLayout* btn_box = new QHBoxLayout();
  btn_box->addStretch();
  _button_fly = new QPushButton("Fly", this);
  _button_cancel = new QPushButton("Cancel", this);
  btn_box->addWidget(_button_fly);
  btn_box->addWidget(_button_cancel);
  vbox_main->addLayout(btn_box);

  connect(_city_list, &QListWidget::itemSelectionChanged, this,
          &WindowFlyAway::onSelectionChanged);
  connect(_city_list, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem* item) {
            if (item && (item->flags() & Qt::ItemIsEnabled)) {
              onFlyClicked();
            }
          });
  connect(_button_cancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(_button_fly, &QPushButton::clicked, this,
          &WindowFlyAway::onFlyClicked);

  onSelectionChanged();
}

void WindowFlyAway::_populateCityList() {
  _city_list->clear();
  int first_valid = -1;

  for (int i = 0; i < CITY_NUM; ++i) {
    int cost = flight_cost(_gameState.location, i);
    bool is_current = (i == _gameState.location);
    bool can_afford = (_gameState.cash >= cost);

    std::string text;
    if (is_current) {
      text = std::format("{}, {} (Current)", city_name(city_info[i].id),
                         country_name(city_info[i].country));
    } else {
      text =
          std::format("{}, {} (${})", city_name(city_info[i].id),
                      country_name(city_info[i].country), money_string(cost));
    }

    QListWidgetItem* item =
        new QListWidgetItem(QString::fromStdString(text), _city_list);
    item->setData(Qt::UserRole, i);
    item->setData(Qt::UserRole + 1, cost);

    if (is_current) {
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled &
                     ~Qt::ItemIsSelectable);
      item->setToolTip("You are already here");
    } else if (!can_afford) {
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled &
                     ~Qt::ItemIsSelectable);
      item->setToolTip("Too expensive: insufficient funds");
    } else {
      if (first_valid == -1) {
        first_valid = i;
      }
    }
  }

  if (first_valid >= 0) {
    _city_list->setCurrentRow(first_valid);
  }
}

void WindowFlyAway::onSelectionChanged() {
  QListWidgetItem* item = _city_list->currentItem();
  if (!item || !(item->flags() & Qt::ItemIsEnabled)) {
    _button_fly->setEnabled(false);
    return;
  }
  int dest = item->data(Qt::UserRole).toInt();
  int cost = item->data(Qt::UserRole + 1).toInt();
  _button_fly->setEnabled(dest != _gameState.location &&
                          _gameState.cash >= cost);
}

void WindowFlyAway::onFlyClicked() {
  QListWidgetItem* item = _city_list->currentItem();
  if (!item || !(item->flags() & Qt::ItemIsEnabled)) return;
  int dest = item->data(Qt::UserRole).toInt();
  int cost = item->data(Qt::UserRole + 1).toInt();
  if (dest >= 0 && dest < CITY_NUM && dest != _gameState.location) {
    if (_gameState.cash >= cost) {
      _gameState.cash -= cost;
      _gameState.location = dest;

      // Drug-sniffing dogs check at airport
      static std::random_device rd;
      static std::mt19937 gen(rd());
      if (_gameState.total_drugs() > 0 && (gen() % 3 == 0)) {
        int needed = std::max(1, _gameState.total_drugs() / 10);
        if (_gameState.no_scent_cans() >= needed) {
          auto ans = QMessageBox::question(
              this, "The drug sniffing dogs smell something!",
              QString("To make your drugs undetectable to the drug-sniffing "
                      "dogs, you will need to use %1 %2 of No-Scent. You have "
                      "%3 %4.\nDo you wish to use them?")
                  .arg(needed)
                  .arg(needed == 1 ? "can" : "cans")
                  .arg(_gameState.no_scent_cans())
                  .arg(_gameState.no_scent_cans() == 1 ? "can" : "cans"),
              QMessageBox::Yes | QMessageBox::No);
          if (ans == QMessageBox::Yes) {
            _gameState.weapon_qty[11] -= needed;
          } else {
            QMessageBox::warning(this, "Airport Security",
                                 "The drug sniffing dogs detected your drugs! "
                                 "Airport security is moving in!");
            WindowCombat dlg(_gameState, 9, 2 + (_gameState.rank + 1) * 2,
                             this);
            dlg.exec();
          }
        } else {
          QMessageBox::warning(
              this, "The drug sniffing dogs smell something!",
              QString("To make your drugs undetectable to the drug-sniffing "
                      "dogs, you will need to use %1 %2 of No-Scent. You have "
                      "%3 %4.\nAirport security is moving in!")
                  .arg(needed)
                  .arg(needed == 1 ? "can" : "cans")
                  .arg(_gameState.no_scent_cans())
                  .arg(_gameState.no_scent_cans() == 1 ? "can" : "cans"));
          WindowCombat dlg(_gameState, 9, 2 + (_gameState.rank + 1) * 2, this);
          dlg.exec();
        }
      }

      _gameState.stay_here();
      emit stateChanged();
      accept();
    }
  }
}

// WindowCombat Implementation

WindowCombat::WindowCombat(GameState& gameState, int enemy_idx, int enemy_count,
                           QWidget* parent)
    : QDialog(parent),
      _gameState(gameState),
      _enemyIdx(enemy_idx),
      _enemyCount(enemy_count) {
  _setupWidget();
  _updateStatus();
}

void WindowCombat::_setupWidget() {
  setWindowTitle(QString("Fighting %1")
                     .arg(QString::fromStdString(enemy_info[_enemyIdx].name)));
  setModal(true);
  setFixedSize(500, 400);

  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setSpacing(8);

  _labelHeader = new QLabel(
      QString("You have encountered %1! There %2 of them!")
          .arg(QString::fromStdString(enemy_info[_enemyIdx].name))
          .arg(_enemyCount == 1 ? "is only one"
                                : QString("are %1").arg(_enemyCount)),
      this);
  QFont font = _labelHeader->font();
  font.setBold(true);
  font.setPointSize(font.pointSize() + 1);
  _labelHeader->setFont(font);
  main_layout->addWidget(_labelHeader);

  // Status group
  QGroupBox* status_group = new QGroupBox("Status", this);
  QGridLayout* grid = new QGridLayout(status_group);
  grid->setContentsMargins(5, 5, 5, 5);

  grid->addWidget(new QLabel("Enemy:", status_group), 0, 0);
  _labelEnemy = new QLabel(status_group);
  grid->addWidget(_labelEnemy, 0, 1);

  grid->addWidget(new QLabel("Health:", status_group), 0, 2);
  _labelHealth = new QLabel(status_group);
  grid->addWidget(_labelHealth, 0, 3);

  grid->addWidget(new QLabel("Cash:", status_group), 1, 0);
  _labelCash = new QLabel(status_group);
  grid->addWidget(_labelCash, 1, 1);

  grid->addWidget(new QLabel("Weapon:", status_group), 1, 2);
  _labelWeapon = new QLabel(status_group);
  grid->addWidget(_labelWeapon, 1, 3);

  grid->addWidget(new QLabel("Ammo:", status_group), 2, 0);
  _labelAmmo = new QLabel(status_group);
  grid->addWidget(_labelAmmo, 2, 1);

  grid->addWidget(new QLabel("Armor:", status_group), 2, 2);
  _labelArmor = new QLabel(status_group);
  grid->addWidget(_labelArmor, 2, 3);

  main_layout->addWidget(status_group);

  _textLog = new QTextEdit(this);
  _textLog->setReadOnly(true);
  _textLog->setFixedHeight(150);
  main_layout->addWidget(_textLog);

  QHBoxLayout* button_layout = new QHBoxLayout();
  _buttonFight = new QPushButton("Fight", this);
  _buttonFlee = new QPushButton("Flee", this);
  _buttonBribe = new QPushButton("Bribe", this);
  _buttonSurrender = new QPushButton("Surrender", this);

  connect(_buttonFight, &QPushButton::clicked, this,
          &WindowCombat::onFightClicked);
  connect(_buttonFlee, &QPushButton::clicked, this,
          &WindowCombat::onFleeClicked);
  connect(_buttonBribe, &QPushButton::clicked, this,
          &WindowCombat::onBribeClicked);
  connect(_buttonSurrender, &QPushButton::clicked, this,
          &WindowCombat::onSurrenderClicked);

  button_layout->addWidget(_buttonFight);
  button_layout->addWidget(_buttonFlee);
  button_layout->addWidget(_buttonBribe);
  button_layout->addWidget(_buttonSurrender);
  main_layout->addLayout(button_layout);

  _textLog->append(
      QString("You face %1 %2! Choose your action.")
          .arg(_enemyCount)
          .arg(QString::fromStdString(enemy_info[_enemyIdx].name)));
}

void WindowCombat::_updateStatus() {
  _labelEnemy->setText(
      QString("%1 (%2 remain)")
          .arg(QString::fromStdString(enemy_info[_enemyIdx].name))
          .arg(_enemyCount));
  _labelHealth->setText(QString("%1%").arg(_gameState.health));
  _labelCash->setText(QString::fromStdString(money_string(_gameState.cash)));
  _labelArmor->setText(QString::number(_gameState.total_armor()));

  // Find best available weapon with ammo (or knife)
  int best_weapon = -1;
  for (int i = 8; i >= 0; --i) {
    if (i == 5 || i == 6) {
      if (_gameState.ammo_qty[i] > 0) {
        best_weapon = i;
        break;
      }
    } else if (_gameState.weapon_qty[i] > 0) {
      if (shop_items[i].ammo_limit == 0 || _gameState.ammo_qty[i] > 0) {
        best_weapon = i;
        break;
      }
    }
  }

  if (best_weapon >= 0) {
    if (best_weapon == 5 || best_weapon == 6) {
      _labelWeapon->setText(
          QString::fromStdString(shop_items[best_weapon].ammo_name));
      _labelAmmo->setText(QString("%1/%2")
                              .arg(_gameState.ammo_qty[best_weapon])
                              .arg(shop_items[best_weapon].ammo_limit));
    } else {
      _labelWeapon->setText(
          QString::fromStdString(shop_items[best_weapon].name));
      if (shop_items[best_weapon].ammo_limit > 0) {
        _labelAmmo->setText(QString("%1/%2")
                                .arg(_gameState.ammo_qty[best_weapon])
                                .arg(shop_items[best_weapon].ammo_limit));
      } else {
        _labelAmmo->setText("N/A");
      }
    }
  } else {
    _labelWeapon->setText("Fists");
    _labelAmmo->setText("N/A");
  }

  _buttonBribe->setEnabled(enemy_info[_enemyIdx].can_bribe &&
                           _gameState.cash > 0 && _enemyCount > 0);
  _buttonSurrender->setEnabled(enemy_info[_enemyIdx].surrender_accept > 0 &&
                               _enemyCount > 0);
}

void WindowCombat::onFightClicked() {
  if (_enemyCount <= 0 || _gameState.health <= 0) return;

  int best_weapon = -1;
  for (int i = 8; i >= 0; --i) {
    if (i == 5 || i == 6) {
      if (_gameState.ammo_qty[i] > 0) {
        best_weapon = i;
        break;
      }
    } else if (_gameState.weapon_qty[i] > 0) {
      if (shop_items[i].ammo_limit == 0 || _gameState.ammo_qty[i] > 0) {
        best_weapon = i;
        break;
      }
    }
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (best_weapon >= 0) {
    std::string w_name = (best_weapon == 5 || best_weapon == 6)
                             ? shop_items[best_weapon].ammo_name
                             : shop_items[best_weapon].name;
    if (shop_items[best_weapon].ammo_limit > 0) {
      _gameState.ammo_qty[best_weapon]--;
    }
    int hit_chance = shop_items[best_weapon].hit_chance;
    if (static_cast<int>(gen() % 100) < hit_chance) {
      int killed = std::min(_enemyCount, shop_items[best_weapon].damage);
      _enemyCount -= killed;
      if (_enemyCount <= 0) {
        _textLog->append(
            QString(
                "You use your %1! You killed the remaining %2! They are all "
                "dead!")
                .arg(QString::fromStdString(w_name))
                .arg(killed));
        int bounty =
            (static_cast<int>(gen() % 400) + 100) * (_gameState.rank + 1);
        _gameState.cash += bounty;
        _textLog->append(
            QString("You search the bodies and find $%1!")
                .arg(QString::fromStdString(money_string(bounty))));
        _buttonFight->setEnabled(false);
        _buttonFlee->setEnabled(false);
        _buttonBribe->setEnabled(false);
        _buttonSurrender->setText("Done");
        disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
        connect(_buttonSurrender, &QPushButton::clicked, this,
                &QDialog::accept);
        _updateStatus();
        emit stateChanged();
        return;
      } else {
        _textLog->append(
            QString("You use your %1! You killed %2 of them! Only %3 %4.")
                .arg(QString::fromStdString(w_name))
                .arg(killed)
                .arg(_enemyCount)
                .arg(_enemyCount == 1 ? "remains" : "remain"));
      }
    } else {
      _textLog->append(QString("You use your %1... You miss!")
                           .arg(QString::fromStdString(w_name)));
    }
  } else {
    _textLog->append("You attack with your bare hands... You miss!");
  }

  _enemyAttack();
  _updateStatus();
  emit stateChanged();
}

void WindowCombat::_enemyAttack() {
  if (_enemyCount <= 0) return;

  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (_enemyCount <= 2 && (gen() % 100 < 25)) {
    _textLog->append("They decide to flee!");
    _enemyCount = 0;
    _buttonFight->setEnabled(false);
    _buttonFlee->setEnabled(false);
    _buttonBribe->setEnabled(false);
    _buttonSurrender->setText("Done");
    disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
    connect(_buttonSurrender, &QPushButton::clicked, this, &QDialog::accept);
    return;
  }

  int acc = enemy_info[_enemyIdx].accuracy;
  if (static_cast<int>(gen() % 100) < acc) {
    int raw_dmg = 5 + static_cast<int>(gen() % 12);
    int armor = _gameState.total_armor();
    int dmg = std::max(1, raw_dmg - armor);
    _gameState.health -= dmg;
    _textLog->append(
        QString("They attack you! You have been hit for %1 %2 of damage!")
            .arg(dmg)
            .arg(dmg == 1 ? "point" : "points"));
    if (_gameState.health <= 0) {
      _gameState.health = 0;
      _textLog->append("YOU ARE DEAD!");
      _buttonFight->setEnabled(false);
      _buttonFlee->setEnabled(false);
      _buttonBribe->setEnabled(false);
      _buttonSurrender->setText("Close");
      disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
      connect(_buttonSurrender, &QPushButton::clicked, this, &QDialog::reject);
    }
  } else {
    _textLog->append("They attack you but miss!");
  }
}

void WindowCombat::onFleeClicked() {
  if (_enemyCount <= 0 || _gameState.health <= 0) return;

  static std::random_device rd;
  static std::mt19937 gen(rd());

  int chance = 35 + enemy_info[_enemyIdx].flee_chance;
  if (static_cast<int>(gen() % 100) < chance) {
    _textLog->append("You manage to lose them in the streets!");
    _buttonFight->setEnabled(false);
    _buttonFlee->setEnabled(false);
    _buttonBribe->setEnabled(false);
    _buttonSurrender->setText("Done");
    disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
    connect(_buttonSurrender, &QPushButton::clicked, this, &QDialog::accept);
  } else {
    _textLog->append("You are unable to shake them.");
    _enemyAttack();
  }
  _updateStatus();
  emit stateChanged();
}

void WindowCombat::onBribeClicked() {
  if (_enemyCount <= 0 || _gameState.health <= 0) return;
  if (_gameState.cash <= 0) return;

  bool ok = false;
  int offer = QInputDialog::getInt(
      this, "Bribe",
      QString(
          "You have $%1 to bribe them with.\nHow much do you want to offer?")
          .arg(QString::fromStdString(money_string(_gameState.cash))),
      std::min(_gameState.cash, 100), 1, _gameState.cash, 50, &ok);
  if (!ok || offer <= 0) return;

  static std::random_device rd;
  static std::mt19937 gen(rd());

  int needed = (enemy_info[_enemyIdx].scale * 80) + (_enemyCount * 30);
  if (offer >= needed || (gen() % 100 < 50)) {
    _gameState.cash -= offer;
    _textLog->append("They accept your bribe!");
    _buttonFight->setEnabled(false);
    _buttonFlee->setEnabled(false);
    _buttonBribe->setEnabled(false);
    _buttonSurrender->setText("Done");
    disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
    connect(_buttonSurrender, &QPushButton::clicked, this, &QDialog::accept);
  } else {
    _textLog->append(
        "They laugh at your attempt! Probably not the best time to be stingy.");
    _enemyAttack();
  }
  _updateStatus();
  emit stateChanged();
}

void WindowCombat::onSurrenderClicked() {
  if (_enemyCount <= 0 || _gameState.health <= 0) return;

  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (static_cast<int>(gen() % 100) < enemy_info[_enemyIdx].surrender_accept) {
    bool is_cops =
        (_enemyIdx <= 1 || _enemyIdx == 4 || _enemyIdx == 8 || _enemyIdx == 9);
    if (is_cops) {
      if (_gameState.total_drugs() > 0) {
        for (int i = 0; i < DRUG_NUM; ++i) _gameState.player_qty[i] = 0;
        _gameState.pocket = 0;
        _textLog->append(
            "They accept your surrender. They seize all your drugs! They "
            "arrest you and after a few hours in jail, you are let go.");
      } else {
        _textLog->append(
            "They accept your surrender. They search you and find you are "
            "clean! A brief apology is mumbled.");
      }
    } else {
      _gameState.cash = 0;
      for (int i = 0; i < DRUG_NUM; ++i) _gameState.player_qty[i] = 0;
      _gameState.pocket = 0;
      _textLog->append(
          "They accept your surrender but take everything! Hope you have money "
          "in the bank!");
    }
    _buttonFight->setEnabled(false);
    _buttonFlee->setEnabled(false);
    _buttonBribe->setEnabled(false);
    _buttonSurrender->setText("Done");
    disconnect(_buttonSurrender, nullptr, nullptr, nullptr);
    connect(_buttonSurrender, &QPushButton::clicked, this, &QDialog::accept);
  } else {
    _textLog->append("Your surrender is not accepted!");
    _enemyAttack();
  }
  _updateStatus();
  emit stateChanged();
}
