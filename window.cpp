#include "window.h"

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QKeySequence>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>
#include <QTextEdit>
#include <QTreeWidget>
#include <QWidget>

#include <array>
#include <format>
#include <string>

#include "config.h"
#include "druxlord.h"
#include "window-cb.h"

inline constexpr int COLUMN_NAME_WIDTH = 120;
inline constexpr int COLUMN_QTY_WIDTH = 50;
inline constexpr int COLUMN_PRICE_WIDTH = 70;

WindowMain window_main{};

WindowFinance window_finance{};
WindowShopping window_shopping{};
WindowHospital window_hospital{};
WindowVault window_vault{};

WindowWorldDrugPrices window_world_drug_prices{};
WindowWorldCities window_world_cities{};

WindowInput window_input{};

static QGroupBox *group_pocket = nullptr;

void update_all_ui(const GameState &game_state) {
  set_label_location(game_state.location);
  set_label_health(game_state.health);
  set_label_day(game_state.day + 1);
  set_label_rank(game_state.rank);
  set_label_cash(game_state.cash);
  set_label_bank(game_state.bank);
  set_label_debt(game_state.debt);
  set_label_frame_pocket(game_state.pocket, game_state.pocket_capacity);
  if (window_main.treeview_market) {
    fill_treeview_market(window_main.treeview_market, game_state);
  }
  if (window_main.treeview_pocket) {
    fill_treeview_pocket(window_main.treeview_pocket, game_state);
  }
  if (window_main.textview_information) {
    std::string news = game_state.get_market_news(game_state.location, game_state.day);
    window_main.textview_information->setText(QString::fromStdString(news));
  }
}

void fill_treeview_market(QTreeWidget* treeview, const GameState& game_state) {
  if (!treeview) return;
  treeview->clear();
  int loc = game_state.location;
  int day = game_state.day;

  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    if (game_state.drug_table[drug_idx][loc][day].available) {
      std::string price_str =
          money_string(game_state.drug_table[drug_idx][loc][day].price);
      QTreeWidgetItem* item = new QTreeWidgetItem(treeview);
      std::string name_str = drug_name(drug_info[drug_idx].id);
      if (game_state.drug_table[drug_idx][loc][day].event_flag > 0) {
        item->setText(COLUMN_STATUS, QString::fromUtf8("\u25B2"));
      } else if (game_state.drug_table[drug_idx][loc][day].event_flag < 0) {
        item->setText(COLUMN_STATUS, QString::fromUtf8("\u25BC"));
      }
      item->setText(COLUMN_NAME, QString::fromStdString(name_str));
      item->setText(
          COLUMN_QTY,
          QString::number(game_state.drug_table[drug_idx][loc][day].qty));
      item->setText(COLUMN_PRICE, QString::fromStdString(price_str));
      item->setData(COLUMN_NAME, Qt::UserRole, drug_idx);
      item->setTextAlignment(COLUMN_QTY, Qt::AlignRight | Qt::AlignVCenter);
      item->setTextAlignment(COLUMN_PRICE, Qt::AlignRight | Qt::AlignVCenter);
    }
  }
}

void fill_treeview_pocket(QTreeWidget* treeview, const GameState& game_state) {
  if (!treeview) return;
  treeview->clear();

  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    if (game_state.player_qty[drug_idx] > 0) {
      QTreeWidgetItem* item = new QTreeWidgetItem(treeview);
      std::string name_str = drug_name(drug_info[drug_idx].id);
      item->setText(0, QString::fromStdString(name_str));
      item->setText(1, QString::number(game_state.player_qty[drug_idx]));
      item->setText(2, QString::fromStdString(
                           money_string(game_state.player_price[drug_idx])));
      item->setData(0, Qt::UserRole, drug_idx);
      item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
      item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
    }
  }
}

void fill_treeview_city_list(QTreeWidget* treeview, GameState* game_state,
                             int drug_idx) {
  if (!treeview || !game_state) return;
  treeview->clear();
  int day = game_state->day;
  for (int city_idx = 0; city_idx < CITY_NUM; ++city_idx) {
    QTreeWidgetItem* item = new QTreeWidgetItem(treeview);
    std::string text = std::format("{}, {}", city_name(city_info[city_idx].id),
                                   country_name(city_info[city_idx].country));
    item->setText(0, QString::fromStdString(text));
    item->setText(1, QString::number(
                         game_state->drug_table[drug_idx][city_idx][day].qty));
    item->setText(2,
                  QString::fromStdString(money_string(
                      game_state->drug_table[drug_idx][city_idx][day].price)));
    item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
  }
}

void fill_treeview_drug_list(QTreeWidget* treeview, GameState* game_state,
                             int city_idx) {
  if (!treeview || !game_state) return;
  treeview->clear();
  int day = game_state->day;
  for (int drug_idx = 0; drug_idx < DRUG_NUM; ++drug_idx) {
    QTreeWidgetItem* item = new QTreeWidgetItem(treeview);
    std::string name_str = drug_name(drug_info[drug_idx].id);
    item->setText(0, QString::fromStdString(name_str));
    item->setText(1, QString::number(
                         game_state->drug_table[drug_idx][city_idx][day].qty));
    item->setText(2,
                  QString::fromStdString(money_string(
                      game_state->drug_table[drug_idx][city_idx][day].price)));
    item->setData(0, Qt::UserRole, drug_idx);
    item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
  }
}

static QTreeWidget* create_treeview_drug(bool with_status) {
  QTreeWidget *treeview = new QTreeWidget();
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
    treeview->headerItem()->setTextAlignment(COLUMN_QTY, Qt::AlignRight | Qt::AlignVCenter);
    treeview->headerItem()->setTextAlignment(COLUMN_PRICE, Qt::AlignRight | Qt::AlignVCenter);
  } else {
    treeview->setColumnCount(3);
    treeview->setHeaderLabels({"Name", "Qty", "Price"});
    treeview->setColumnWidth(0, COLUMN_NAME_WIDTH);
    treeview->setColumnWidth(1, COLUMN_QTY_WIDTH);
    treeview->setColumnWidth(2, COLUMN_PRICE_WIDTH);
    treeview->headerItem()->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    treeview->headerItem()->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
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
  treeview->setHeaderLabels({"Name"});
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

void set_label_frame_pocket(int npocket, int capacity) {
  auto str = std::format("You pants pocket ({}/{})", npocket, capacity);
  if (window_main.label_pocket) {
    window_main.label_pocket->setText(QString::fromStdString(str));
  }
  if (group_pocket) {
    group_pocket->setTitle(QString::fromStdString(str));
  }
}

void set_label_health(int health) {
  if (window_main.progressbar_health) {
    window_main.progressbar_health->setValue(health);
  }
}

void set_label_location(int location) {
  std::string loc_str;
  if (location >= 0 && location < CITY_NUM) {
    loc_str = std::format("{}, {}", city_name(city_info[location].id), country_name(city_info[location].country));
  } else {
    loc_str = "Austin, USA";
  }
  auto markup = std::format("<span><b>{}</b></span>", loc_str);
  if (window_main.label_location) {
    window_main.label_location->setText(QString::fromStdString(markup));
  }
}

void set_label_day(int day) {
  auto markup = std::format("<span><b>{}/30</b></span>", day);
  if (window_main.label_day) {
    window_main.label_day->setText(QString::fromStdString(markup));
  }
}

void set_label_rank(int rank) {
  constexpr std::array<const char*, RANK_NUM> rank_str = {
    "wannabe",
    "small time operator",
    "dealer",
    "big time dealer",
    "distributor",
    "drug lord"
  };
  const char *r = (rank >= 0 && rank < RANK_NUM) ? rank_str[rank] : "wannabe";
  auto markup = std::format("<span><b>{}</b></span>", r);
  if (window_main.label_rank) {
    window_main.label_rank->setText(QString::fromStdString(markup));
  }
}

void set_label_cash(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  if (window_main.label_cash) {
    window_main.label_cash->setText(QString::fromStdString(markup));
  }
}

void set_label_bank(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  if (window_main.label_bank) {
    window_main.label_bank->setText(QString::fromStdString(markup));
  }
}

void set_label_debt(int value) {
  std::string markup;
  if (value > 0) {
    markup = std::format("<span style=\"color:#FF0000;\"><b>{}</b></span>", value);
  } else {
    markup = std::format("<span><b>{}</b></span>", value);
  }
  if (window_main.label_debt) {
    window_main.label_debt->setText(QString::fromStdString(markup));
  }
}

void create_window_main(GameState &game_state) {
  window_main.game_state = &game_state;
  window_main.window = new QWidget();
  window_main.window->setWindowTitle(QString::fromUtf8(kProgramName.data(), kProgramName.size()));

  QVBoxLayout *vbox_main = new QVBoxLayout(window_main.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox *frame_info = new QGroupBox("Information", window_main.window);
  QVBoxLayout *vbox_info = new QVBoxLayout(frame_info);
  vbox_info->setContentsMargins(5, 5, 5, 5);
  window_main.textview_information = new QTextEdit(frame_info);
  window_main.textview_information->setReadOnly(true);
  window_main.textview_information->setFixedHeight(120);
  vbox_info->addWidget(window_main.textview_information);
  vbox_main->addWidget(frame_info);

  QHBoxLayout *hbox_down = new QHBoxLayout();
  hbox_down->setSpacing(8);
  vbox_main->addLayout(hbox_down);

  QGroupBox *frame_market = new QGroupBox("The Market", window_main.window);
  QVBoxLayout *vbox_market = new QVBoxLayout(frame_market);
  vbox_market->setContentsMargins(5, 5, 5, 5);
  window_main.treeview_market = create_treeview_drug(true);
  window_main.treeview_market->setMinimumSize(270, 240);
  vbox_market->addWidget(window_main.treeview_market);
  hbox_down->addWidget(frame_market);

  QVBoxLayout *vbox_middle = new QVBoxLayout();
  vbox_middle->setSpacing(5);
  hbox_down->addLayout(vbox_middle);

  QGroupBox *frame_action = new QGroupBox("Action", window_main.window);
  QVBoxLayout *box_action = new QVBoxLayout(frame_action);
  box_action->setContentsMargins(5, 5, 5, 5);
  box_action->setSpacing(3);

  window_main.button_buy = new QPushButton(QString::fromUtf8("&Buy \u2192"), frame_action);
  QObject::connect(window_main.button_buy, &QPushButton::clicked, window_main_button_buy_clicked_cb);
  box_action->addWidget(window_main.button_buy);

  window_main.button_sell = new QPushButton(QString::fromUtf8("\u2190 &Sell"), frame_action);
  QObject::connect(window_main.button_sell, &QPushButton::clicked, window_main_button_sell_clicked_cb);
  box_action->addWidget(window_main.button_sell);

  window_main.button_dump = new QPushButton("&Dump", frame_action);
  QObject::connect(window_main.button_dump, &QPushButton::clicked, window_main_button_dump_clicked_cb);
  box_action->addWidget(window_main.button_dump);

  window_main.button_places = new QPushButton("Places...", frame_action);
  create_places_menu(window_main.button_places);
  box_action->addWidget(window_main.button_places);

  window_main.button_info = new QPushButton("Info...", frame_action);
  create_info_menu(window_main.button_info);
  box_action->addWidget(window_main.button_info);

  vbox_middle->addWidget(frame_action);

  QGroupBox *frame_tomorrow = new QGroupBox("Tomorrow", window_main.window);
  QVBoxLayout *box_tomorrow = new QVBoxLayout(frame_tomorrow);
  box_tomorrow->setContentsMargins(5, 5, 5, 5);
  box_tomorrow->setSpacing(3);

  window_main.button_stayhere = new QPushButton("Stay Here", frame_tomorrow);
  QObject::connect(window_main.button_stayhere, &QPushButton::clicked, window_main_button_stayhere_clicked_cb);
  box_tomorrow->addWidget(window_main.button_stayhere);

  window_main.button_flyaway = new QPushButton("Fly Away", frame_tomorrow);
  QObject::connect(window_main.button_flyaway, &QPushButton::clicked, window_main_button_flyaway_clicked_cb);
  box_tomorrow->addWidget(window_main.button_flyaway);

  vbox_middle->addWidget(frame_tomorrow);

  QGroupBox *frame_game = new QGroupBox("Game", window_main.window);
  QVBoxLayout *box_game = new QVBoxLayout(frame_game);
  box_game->setContentsMargins(5, 5, 5, 5);
  box_game->setSpacing(3);

  window_main.checkbutton_sound = new QCheckBox("Sou&nd", frame_game);
  box_game->addWidget(window_main.checkbutton_sound);

  window_main.button_about = new QPushButton("&About", frame_game);
  QObject::connect(window_main.button_about, &QPushButton::clicked, window_main_button_about_clicked_cb);
  box_game->addWidget(window_main.button_about);

  window_main.button_docs = new QPushButton("Docs", frame_game);
  QObject::connect(window_main.button_docs, &QPushButton::clicked, window_main_button_docs_clicked_cb);
  box_game->addWidget(window_main.button_docs);

  window_main.button_highscores = new QPushButton("High Scores", frame_game);
  QObject::connect(window_main.button_highscores, &QPushButton::clicked, window_main_button_highscores_clicked_cb);
  box_game->addWidget(window_main.button_highscores);

  window_main.button_newgamequit = new QPushButton("New &Game", frame_game);
  QObject::connect(window_main.button_newgamequit, &QPushButton::clicked, window_main_button_newgamequit_clicked_cb);
  box_game->addWidget(window_main.button_newgamequit);

  vbox_middle->addWidget(frame_game);
  vbox_middle->addStretch();

  QVBoxLayout *vbox_right = new QVBoxLayout();
  vbox_right->setSpacing(5);
  hbox_down->addLayout(vbox_right);

  group_pocket = new QGroupBox(QString::fromStdString(std::format("You pants pocket ({}/{})", game_state.pocket, game_state.pocket_capacity)), window_main.window);
  QVBoxLayout *vbox_pocket = new QVBoxLayout(group_pocket);
  vbox_pocket->setContentsMargins(5, 5, 5, 5);
  window_main.label_pocket = new QLabel(group_pocket);
  window_main.label_pocket->hide();
  set_label_frame_pocket(game_state.pocket, game_state.pocket_capacity);

  window_main.treeview_pocket = create_treeview_drug(false);
  window_main.treeview_pocket->setMinimumSize(250, 180);
  vbox_pocket->addWidget(window_main.treeview_pocket);
  vbox_right->addWidget(group_pocket);

  QGroupBox *frame_status = new QGroupBox("Status", window_main.window);
  QVBoxLayout *vbox_status = new QVBoxLayout(frame_status);
  vbox_status->setContentsMargins(5, 5, 5, 5);
  vbox_status->setSpacing(5);

  QHBoxLayout *box_loc = new QHBoxLayout();
  box_loc->setSpacing(10);
  QLabel *label_loc_title = new QLabel("Location:", frame_status);
  box_loc->addWidget(label_loc_title);
  window_main.label_location = new QLabel(frame_status);
  set_label_location(game_state.location);
  box_loc->addWidget(window_main.label_location);
  box_loc->addStretch();
  vbox_status->addLayout(box_loc);

  QHBoxLayout *box_health = new QHBoxLayout();
  box_health->setSpacing(10);
  QLabel *label_health_title = new QLabel("Health:", frame_status);
  box_health->addWidget(label_health_title);
  window_main.progressbar_health = new QProgressBar(frame_status);
  window_main.progressbar_health->setRange(0, 100);
  window_main.progressbar_health->setValue(game_state.health);
  window_main.progressbar_health->setTextVisible(false);
  box_health->addWidget(window_main.progressbar_health);
  vbox_status->addLayout(box_health);

  QHBoxLayout *box_day_rank = new QHBoxLayout();
  box_day_rank->setSpacing(30);

  QHBoxLayout *box_day = new QHBoxLayout();
  box_day->setSpacing(10);
  QLabel *label_day_title = new QLabel("Day:", frame_status);
  box_day->addWidget(label_day_title);
  window_main.label_day = new QLabel(frame_status);
  set_label_day(game_state.day);
  box_day->addWidget(window_main.label_day);
  box_day_rank->addLayout(box_day);

  QHBoxLayout *box_rank = new QHBoxLayout();
  box_rank->setSpacing(10);
  QLabel *label_rank_title = new QLabel("Rank:", frame_status);
  box_rank->addWidget(label_rank_title);
  window_main.label_rank = new QLabel(frame_status);
  set_label_rank(game_state.rank);
  box_rank->addWidget(window_main.label_rank);
  box_day_rank->addLayout(box_rank);
  box_day_rank->addStretch();
  vbox_status->addLayout(box_day_rank);

  QHBoxLayout *box_money_status = new QHBoxLayout();
  box_money_status->setSpacing(0);

  QGridLayout *grid_money = new QGridLayout();
  grid_money->setHorizontalSpacing(50);
  grid_money->setVerticalSpacing(5);

  QLabel *label_cash_title = new QLabel("Cash:", frame_status);
  grid_money->addWidget(label_cash_title, 0, 0, Qt::AlignLeft);
  window_main.label_cash = new QLabel(frame_status);
  set_label_cash(game_state.cash);
  grid_money->addWidget(window_main.label_cash, 0, 1, Qt::AlignRight);

  QLabel *label_bank_title = new QLabel("Bank:", frame_status);
  grid_money->addWidget(label_bank_title, 1, 0, Qt::AlignLeft);
  window_main.label_bank = new QLabel(frame_status);
  set_label_bank(game_state.bank);
  grid_money->addWidget(window_main.label_bank, 1, 1, Qt::AlignRight);

  QLabel *label_debt_title = new QLabel("Debt:", frame_status);
  grid_money->addWidget(label_debt_title, 2, 0, Qt::AlignLeft);
  window_main.label_debt = new QLabel(frame_status);
  set_label_debt(game_state.debt);
  grid_money->addWidget(window_main.label_debt, 2, 1, Qt::AlignRight);

  box_money_status->addLayout(grid_money);

  window_main.drawingarea_status = new QWidget(frame_status);
  box_money_status->addWidget(window_main.drawingarea_status);
  vbox_status->addLayout(box_money_status);

  vbox_right->addWidget(frame_status);

  window_main.shortcut_quit = new QShortcut(QKeySequence::Quit, window_main.window);
  QObject::connect(window_main.shortcut_quit, &QShortcut::activated, window_main.window, &QWidget::close);

  update_all_ui(game_state);

  window_main.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_finance() {
  if (window_finance.window) {
    delete window_finance.window;
  }
  window_finance.window = new QDialog(window_main.window);
  window_finance.window->setWindowTitle("Finance");
  window_finance.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_finance.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox *frame_bank = new QGroupBox("Bank", window_finance.window);
  QVBoxLayout *vbox_bank = new QVBoxLayout(frame_bank);
  vbox_bank->setContentsMargins(5, 5, 5, 5);
  vbox_bank->setSpacing(3);

  QHBoxLayout *hbox_cash_bank = new QHBoxLayout();
  hbox_cash_bank->setSpacing(130);

  QHBoxLayout *hbox_cash = new QHBoxLayout();
  hbox_cash->setSpacing(20);
  hbox_cash->addWidget(new QLabel("Cash:", frame_bank));
  window_finance.label_cash = new QLabel("0", frame_bank);
  hbox_cash->addWidget(window_finance.label_cash);
  hbox_cash_bank->addLayout(hbox_cash);

  QHBoxLayout *hbox_bank_val = new QHBoxLayout();
  hbox_bank_val->setSpacing(20);
  hbox_bank_val->addWidget(new QLabel("In Bank:", frame_bank));
  window_finance.label_bank = new QLabel("0", frame_bank);
  hbox_bank_val->addWidget(window_finance.label_bank);
  hbox_cash_bank->addLayout(hbox_bank_val);
  hbox_cash_bank->addStretch();
  vbox_bank->addLayout(hbox_cash_bank);

  QGridLayout *grid_radios = new QGridLayout();
  grid_radios->setHorizontalSpacing(3);
  grid_radios->setVerticalSpacing(3);

  QButtonGroup *radio_group = new QButtonGroup(frame_bank);
  window_finance.radiobutton_depositsome = new QRadioButton("Deposit some", frame_bank);
  window_finance.radiobutton_depositall = new QRadioButton("Deposit all", frame_bank);
  window_finance.radiobutton_depositallbut = new QRadioButton("Deposit all but", frame_bank);
  window_finance.radiobutton_withdrawsome = new QRadioButton("Withdraw some", frame_bank);
  window_finance.radiobutton_withdrawall = new QRadioButton("Withdraw all", frame_bank);
  window_finance.radiobutton_withdrawallbut = new QRadioButton("Withdraw all but", frame_bank);

  radio_group->addButton(window_finance.radiobutton_depositsome);
  radio_group->addButton(window_finance.radiobutton_depositall);
  radio_group->addButton(window_finance.radiobutton_depositallbut);
  radio_group->addButton(window_finance.radiobutton_withdrawsome);
  radio_group->addButton(window_finance.radiobutton_withdrawall);
  radio_group->addButton(window_finance.radiobutton_withdrawallbut);
  window_finance.radiobutton_depositsome->setChecked(true);

  grid_radios->addWidget(window_finance.radiobutton_depositsome, 0, 0);
  grid_radios->addWidget(window_finance.radiobutton_depositall, 0, 1);
  grid_radios->addWidget(window_finance.radiobutton_depositallbut, 0, 2);
  grid_radios->addWidget(window_finance.radiobutton_withdrawsome, 1, 0);
  grid_radios->addWidget(window_finance.radiobutton_withdrawall, 1, 1);
  grid_radios->addWidget(window_finance.radiobutton_withdrawallbut, 1, 2);
  vbox_bank->addLayout(grid_radios);

  QHBoxLayout *hbox_amount = new QHBoxLayout();
  hbox_amount->setSpacing(10);
  hbox_amount->addWidget(new QLabel("Amount", frame_bank));
  window_finance.spinbutton_amount = new QSpinBox(frame_bank);
  window_finance.spinbutton_amount->setRange(1, 1000000000);
  window_finance.spinbutton_amount->setValue(1);
  hbox_amount->addWidget(window_finance.spinbutton_amount);
  window_finance.button_doit = new QPushButton("Do it!", frame_bank);
  hbox_amount->addWidget(window_finance.button_doit);
  hbox_amount->addStretch();
  vbox_bank->addLayout(hbox_amount);

  vbox_main->addWidget(frame_bank);

  QGroupBox *frame_loans = new QGroupBox("Loans", window_finance.window);
  QVBoxLayout *vbox_loans = new QVBoxLayout(frame_loans);
  vbox_loans->setContentsMargins(5, 5, 5, 5);
  vbox_loans->setSpacing(5);

  window_finance.treeview_loan = new QTreeWidget(frame_loans);
  window_finance.treeview_loan->setRootIsDecorated(false);
  window_finance.treeview_loan->setColumnCount(6);
  window_finance.treeview_loan->setHeaderLabels({"Name", "Pays", "Rate", "Days", "Debt", "Days Left"});
  for (int col = 0; col < 6; ++col) {
    window_finance.treeview_loan->setColumnWidth(col, 70);
    window_finance.treeview_loan->headerItem()->setTextAlignment(col, Qt::AlignRight | Qt::AlignVCenter);
  }
  window_finance.treeview_loan->setFixedHeight(170);
  vbox_loans->addWidget(window_finance.treeview_loan);

  QHBoxLayout *hbox_debt_actions = new QHBoxLayout();
  hbox_debt_actions->setSpacing(0);

  QHBoxLayout *hbox_debt = new QHBoxLayout();
  hbox_debt->setSpacing(20);
  hbox_debt->addWidget(new QLabel("Debt:", frame_loans));
  window_finance.label_debt = new QLabel("0", frame_loans);
  hbox_debt->addWidget(window_finance.label_debt);
  hbox_debt_actions->addLayout(hbox_debt);

  hbox_debt_actions->addStretch();

  QHBoxLayout *hbox_borrow_repay = new QHBoxLayout();
  hbox_borrow_repay->setSpacing(3);
  window_finance.button_borrow = new QPushButton("Borrow", frame_loans);
  hbox_borrow_repay->addWidget(window_finance.button_borrow);
  window_finance.button_repay = new QPushButton("Repay", frame_loans);
  hbox_borrow_repay->addWidget(window_finance.button_repay);
  hbox_debt_actions->addLayout(hbox_borrow_repay);

  vbox_loans->addLayout(hbox_debt_actions);
  vbox_main->addWidget(frame_loans);

  QHBoxLayout *hbox_done = new QHBoxLayout();
  hbox_done->setSpacing(5);
  hbox_done->addStretch();
  window_finance.button_done = new QPushButton("Done", window_finance.window);
  QObject::connect(window_finance.button_done, &QPushButton::clicked, window_finance.window, &QDialog::close);
  hbox_done->addWidget(window_finance.button_done);
  vbox_main->addLayout(hbox_done);

  window_finance.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_shopping() {
  if (window_shopping.window) {
    delete window_shopping.window;
  }
  window_shopping.window = new QDialog(window_main.window);
  window_shopping.window->setWindowTitle("Shopping");
  window_shopping.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_shopping.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox *frame_store = new QGroupBox("Store's Inventory", window_shopping.window);
  QVBoxLayout *vbox_store = new QVBoxLayout(frame_store);
  vbox_store->setContentsMargins(5, 5, 5, 5);
  vbox_store->setSpacing(5);

  window_shopping.treeview_store = new QTreeWidget(frame_store);
  window_shopping.treeview_store->setRootIsDecorated(false);
  window_shopping.treeview_store->setColumnCount(3);
  window_shopping.treeview_store->setHeaderLabels({"Name", "Type", "Price"});
  window_shopping.treeview_store->setColumnWidth(COLUMN_STORE_NAME, 140);
  window_shopping.treeview_store->setColumnWidth(COLUMN_STORE_TYPE, 100);
  window_shopping.treeview_store->setColumnWidth(COLUMN_STORE_PRICE, 70);
  window_shopping.treeview_store->headerItem()->setTextAlignment(COLUMN_STORE_NAME, Qt::AlignRight | Qt::AlignVCenter);
  window_shopping.treeview_store->headerItem()->setTextAlignment(COLUMN_STORE_TYPE, Qt::AlignRight | Qt::AlignVCenter);
  window_shopping.treeview_store->headerItem()->setTextAlignment(COLUMN_STORE_PRICE, Qt::AlignRight | Qt::AlignVCenter);
  window_shopping.treeview_store->setFixedHeight(220);
  vbox_store->addWidget(window_shopping.treeview_store);

  QHBoxLayout *hbox_buy = new QHBoxLayout();
  hbox_buy->addStretch();
  window_shopping.button_buy = new QPushButton("   Buy   ", frame_store);
  hbox_buy->addWidget(window_shopping.button_buy);
  vbox_store->addLayout(hbox_buy);

  vbox_main->addWidget(frame_store);

  QGroupBox *frame_inv = new QGroupBox("Your Inventory", window_shopping.window);
  QVBoxLayout *vbox_inv = new QVBoxLayout(frame_inv);
  vbox_inv->setContentsMargins(5, 5, 5, 5);
  vbox_inv->setSpacing(5);

  window_shopping.treeview_inventory = new QTreeWidget(frame_inv);
  window_shopping.treeview_inventory->setRootIsDecorated(false);
  window_shopping.treeview_inventory->setColumnCount(4);
  window_shopping.treeview_inventory->setHeaderLabels({"Name", "Type", "Qty", "Price"});
  window_shopping.treeview_inventory->setColumnWidth(COLUMN_INVENTORY_NAME, 140);
  window_shopping.treeview_inventory->setColumnWidth(COLUMN_INVENTORY_TYPE, 100);
  window_shopping.treeview_inventory->setColumnWidth(COLUMN_INVENTORY_QTY, 70);
  window_shopping.treeview_inventory->setColumnWidth(COLUMN_INVENTORY_SELLFOR, 70);
  for (int col = 0; col < 4; ++col) {
    window_shopping.treeview_inventory->headerItem()->setTextAlignment(col, Qt::AlignRight | Qt::AlignVCenter);
  }
  window_shopping.treeview_inventory->setFixedHeight(150);
  vbox_inv->addWidget(window_shopping.treeview_inventory);

  QHBoxLayout *hbox_sell_cash = new QHBoxLayout();
  hbox_sell_cash->setSpacing(0);

  QHBoxLayout *hbox_cash = new QHBoxLayout();
  hbox_cash->setSpacing(20);
  hbox_cash->addWidget(new QLabel("Cash:", frame_inv));
  window_shopping.label_cash = new QLabel("0", frame_inv);
  hbox_cash->addWidget(window_shopping.label_cash);
  hbox_sell_cash->addLayout(hbox_cash);

  hbox_sell_cash->addStretch();

  window_shopping.button_sell = new QPushButton("   Sell   ", frame_inv);
  hbox_sell_cash->addWidget(window_shopping.button_sell);
  vbox_inv->addLayout(hbox_sell_cash);

  vbox_main->addWidget(frame_inv);

  QHBoxLayout *hbox_done = new QHBoxLayout();
  hbox_done->setSpacing(5);
  hbox_done->addStretch();
  window_shopping.button_done = new QPushButton("Done", window_shopping.window);
  QObject::connect(window_shopping.button_done, &QPushButton::clicked, window_shopping.window, &QDialog::close);
  hbox_done->addWidget(window_shopping.button_done);
  vbox_main->addLayout(hbox_done);

  window_shopping.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_hospital() {
  if (window_hospital.window) {
    delete window_hospital.window;
  }
  window_hospital.window = new QDialog(window_main.window);
  window_hospital.window->setWindowTitle("Hospital");
  window_hospital.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_hospital.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout *hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox *frame_icon = new QGroupBox("", window_hospital.window);
  QHBoxLayout *hbox_icon = new QHBoxLayout(frame_icon);
  hbox_icon->setContentsMargins(20, 20, 20, 20);
  QLabel *image_icon = new QLabel(frame_icon);
  image_icon->setPixmap(window_hospital.window->style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(48, 48));
  hbox_icon->addWidget(image_icon);
  hbox_top->addWidget(frame_icon);

  QGroupBox *frame_slider = new QGroupBox("Move the pointer to desired health", window_hospital.window);
  frame_slider->setFixedWidth(400);
  QVBoxLayout *vbox_slider = new QVBoxLayout(frame_slider);
  vbox_slider->setContentsMargins(5, 5, 5, 5);
  vbox_slider->setSpacing(10);

  QVBoxLayout *vbox_bars = new QVBoxLayout();
  vbox_bars->setSpacing(3);
  window_hospital.progressbar_health = new QProgressBar(frame_slider);
  window_hospital.progressbar_health->setRange(0, 100);
  window_hospital.progressbar_health->setValue(100);
  window_hospital.progressbar_health->setTextVisible(false);
  vbox_bars->addWidget(window_hospital.progressbar_health);

  window_hospital.scalebutton_health = new QSlider(Qt::Horizontal, frame_slider);
  window_hospital.scalebutton_health->setRange(1, 100);
  window_hospital.scalebutton_health->setValue(100);
  vbox_bars->addWidget(window_hospital.scalebutton_health);
  vbox_slider->addLayout(vbox_bars);

  QGridLayout *grid_labels = new QGridLayout();
  grid_labels->setHorizontalSpacing(10);
  grid_labels->setVerticalSpacing(3);

  QLabel *label_cash_title = new QLabel("Cash:", frame_slider);
  grid_labels->addWidget(label_cash_title, 0, 0, Qt::AlignRight);
  window_hospital.label_cash = new QLabel("0", frame_slider);
  grid_labels->addWidget(window_hospital.label_cash, 0, 1, Qt::AlignLeft);

  QLabel *label_cost_title = new QLabel("Cost for treatment:", frame_slider);
  grid_labels->addWidget(label_cost_title, 1, 0, Qt::AlignRight);
  window_hospital.label_cost = new QLabel("0", frame_slider);
  grid_labels->addWidget(window_hospital.label_cost, 1, 1, Qt::AlignLeft);
  vbox_slider->addLayout(grid_labels);

  hbox_top->addWidget(frame_slider);
  vbox_main->addLayout(hbox_top);

  QHBoxLayout *hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(3);
  hbox_buttons->addStretch();

  window_hospital.button_ok = new QPushButton("&OK", window_hospital.window);
  hbox_buttons->addWidget(window_hospital.button_ok);

  QPushButton *button_cancel = new QPushButton("&Cancel", window_hospital.window);
  QObject::connect(button_cancel, &QPushButton::clicked, window_hospital.window, &QDialog::close);
  hbox_buttons->addWidget(button_cancel);

  vbox_main->addLayout(hbox_buttons);

  window_hospital.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_vault() {
  if (window_vault.window) {
    delete window_vault.window;
  }
  window_vault.window = new QDialog(window_main.window);
  window_vault.window->setWindowTitle("The Vault");
  window_vault.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_vault.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout *hbox_middle = new QHBoxLayout();
  hbox_middle->setSpacing(5);

  QGroupBox *frame_pocket = new QGroupBox("You pants pocket (0/10)", window_vault.window);
  QVBoxLayout *vbox_pocket = new QVBoxLayout(frame_pocket);
  vbox_pocket->setContentsMargins(5, 5, 5, 5);
  window_vault.treeview_pocket = create_treeview_drug(false);
  window_vault.treeview_pocket->setFixedHeight(200);
  vbox_pocket->addWidget(window_vault.treeview_pocket);
  hbox_middle->addWidget(frame_pocket);

  QGroupBox *frame_move = new QGroupBox("Move", window_vault.window);
  QVBoxLayout *vbox_move = new QVBoxLayout(frame_move);
  vbox_move->setContentsMargins(5, 5, 5, 5);
  vbox_move->setSpacing(3);
  window_vault.button_intovault = new QPushButton(QString::fromUtf8("Into Vault \u2192"), frame_move);
  vbox_move->addWidget(window_vault.button_intovault);
  window_vault.button_fromvault = new QPushButton(QString::fromUtf8("\u2190 From Vault"), frame_move);
  vbox_move->addWidget(window_vault.button_fromvault);
  vbox_move->addStretch();
  hbox_middle->addWidget(frame_move);

  QGroupBox *frame_vault = new QGroupBox("In the Vault", window_vault.window);
  QVBoxLayout *vbox_vault = new QVBoxLayout(frame_vault);
  vbox_vault->setContentsMargins(5, 5, 5, 5);
  window_vault.treeview_vault = create_treeview_drug(false);
  window_vault.treeview_vault->setFixedHeight(210);
  vbox_vault->addWidget(window_vault.treeview_vault);
  hbox_middle->addWidget(frame_vault);

  vbox_main->addLayout(hbox_middle);

  QHBoxLayout *hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(3);
  hbox_buttons->addStretch();

  window_vault.button_ok = new QPushButton("&OK", window_vault.window);
  hbox_buttons->addWidget(window_vault.button_ok);

  QPushButton *button_cancel = new QPushButton("&Cancel", window_vault.window);
  QObject::connect(button_cancel, &QPushButton::clicked, window_vault.window, &QDialog::close);
  hbox_buttons->addWidget(button_cancel);

  vbox_main->addLayout(hbox_buttons);

  window_vault.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_world_drug_prices() {
  if (window_world_drug_prices.window) {
    delete window_world_drug_prices.window;
  }
  window_world_drug_prices.window = new QDialog(window_main.window);
  window_world_drug_prices.window->setWindowTitle(QString::fromUtf8("World Drug Prices"));
  window_world_drug_prices.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_world_drug_prices.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);
  
  QHBoxLayout *hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox *frame_drug = new QGroupBox("Drug", window_world_drug_prices.window);
  QVBoxLayout *vbox_drug = new QVBoxLayout(frame_drug);
  vbox_drug->setContentsMargins(5, 5, 5, 5);
  window_world_drug_prices.treeview_drug = create_treeview_drug_names();
  window_world_drug_prices.treeview_drug->setFixedHeight(210);
  window_world_drug_prices.treeview_drug->clear();
  for (int i = 0; i < DRUG_NUM; ++i) {
    QTreeWidgetItem* item =
        new QTreeWidgetItem(window_world_drug_prices.treeview_drug);
    item->setData(0, Qt::UserRole, i);
    item->setText(0, QString::fromStdString(drug_name(drug_info[i].id)));
  }
  vbox_drug->addWidget(window_world_drug_prices.treeview_drug);
  hbox_top->addWidget(frame_drug);

  QGroupBox *frame_city = new QGroupBox("City List", window_world_drug_prices.window);
  QVBoxLayout *vbox_city = new QVBoxLayout(frame_city);
  vbox_city->setContentsMargins(5, 5, 5, 5);
  window_world_drug_prices.treeview_city = create_treeview_city_list();
  window_world_drug_prices.treeview_city->setFixedHeight(210);
  vbox_city->addWidget(window_world_drug_prices.treeview_city);
  hbox_top->addWidget(frame_city);

  vbox_main->addLayout(hbox_top);

  QHBoxLayout *hbox_bottom = new QHBoxLayout();
  hbox_bottom->setSpacing(3);
  hbox_bottom->addStretch();

  window_world_drug_prices.button_close = new QPushButton("&Close", window_world_drug_prices.window);
  QObject::connect(window_world_drug_prices.button_close, &QPushButton::clicked, window_world_drug_prices.window, &QDialog::close);
  hbox_bottom->addWidget(window_world_drug_prices.button_close);
  
  vbox_main->addLayout(hbox_bottom);

  window_world_drug_prices.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_world_cities() {
  if (window_world_cities.window) {
    delete window_world_cities.window;
  }
  window_world_cities.window = new QDialog(window_main.window);
  window_world_cities.window->setWindowTitle(QString::fromUtf8("World Cities"));
  window_world_cities.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_world_cities.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QHBoxLayout *hbox_top = new QHBoxLayout();
  hbox_top->setSpacing(5);

  QGroupBox *frame_city = new QGroupBox("City", window_world_cities.window);
  QVBoxLayout *vbox_city = new QVBoxLayout(frame_city);
  vbox_city->setContentsMargins(5, 5, 5, 5);
  window_world_cities.treeview_city = create_treeview_city_names();
  window_world_cities.treeview_city->setFixedHeight(210);
  window_world_cities.treeview_city->clear();
  for (int i = 0; i < CITY_NUM; ++i) {
    QTreeWidgetItem* item =
        new QTreeWidgetItem(window_world_cities.treeview_city);
    item->setData(0, Qt::UserRole, i);
    std::string text = std::format("{}, {}", city_name(city_info[i].id),
                                   country_name(city_info[i].country));
    if (i == window_main.game_state->location) {
      text += " (Current)";
    }
    item->setText(0, QString::fromStdString(text));
  }
  vbox_city->addWidget(window_world_cities.treeview_city);
  hbox_top->addWidget(frame_city);

  QGroupBox *frame_drug = new QGroupBox("Drug List", window_world_cities.window);
  QVBoxLayout *vbox_drug = new QVBoxLayout(frame_drug);
  vbox_drug->setContentsMargins(5, 5, 5, 5);
  window_world_cities.treeview_drug = create_treeview_drug_list();
  window_world_cities.treeview_drug->setFixedHeight(210);
  vbox_drug->addWidget(window_world_cities.treeview_drug);
  hbox_top->addWidget(frame_drug);

  vbox_main->addLayout(hbox_top);

  QHBoxLayout *hbox_bottom = new QHBoxLayout();
  hbox_bottom->setSpacing(3);
  hbox_bottom->addStretch();

  window_world_cities.button_close = new QPushButton("&Close", window_world_cities.window);
  QObject::connect(window_world_cities.button_close, &QPushButton::clicked, window_world_cities.window, &QDialog::close);
  hbox_bottom->addWidget(window_world_cities.button_close);

  vbox_main->addLayout(hbox_bottom);

  window_world_cities.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void create_window_input(const char *title,
                         const char *message,
                         const char *question) {
  if (window_input.window) {
    delete window_input.window;
  }
  window_input.window = new QDialog(window_main.window);
  window_input.window->setWindowTitle(QString::fromUtf8(title));
  window_input.window->setModal(true);

  QVBoxLayout *vbox_main = new QVBoxLayout(window_input.window);
  vbox_main->setContentsMargins(5, 5, 5, 5);
  vbox_main->setSpacing(5);

  QGroupBox *frame_msg = new QGroupBox("Message", window_input.window);
  QHBoxLayout *hbox_msg = new QHBoxLayout(frame_msg);
  hbox_msg->setContentsMargins(5, 5, 5, 5);
  QLabel *label_msg = new QLabel(QString::fromUtf8(message), frame_msg);
  hbox_msg->addWidget(label_msg);
  vbox_main->addWidget(frame_msg);

  QGroupBox *frame_input = new QGroupBox("", window_input.window);
  QHBoxLayout *hbox_input = new QHBoxLayout(frame_input);
  hbox_input->setContentsMargins(5, 5, 5, 5);
  hbox_input->setSpacing(5);

  QLabel *label_question = new QLabel(QString::fromUtf8(question), frame_input);
  label_question->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  label_question->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  hbox_input->addWidget(label_question);

  window_input.spinbutton_value = new QSpinBox(frame_input);
  window_input.spinbutton_value->setRange(1, 1000000);
  window_input.spinbutton_value->setValue(1);
  hbox_input->addWidget(window_input.spinbutton_value);
  vbox_main->addWidget(frame_input);

  QHBoxLayout *hbox_buttons = new QHBoxLayout();
  hbox_buttons->setSpacing(5);
  hbox_buttons->addStretch();

  window_input.button_ok = new QPushButton("&OK", window_input.window);
  hbox_buttons->addWidget(window_input.button_ok);

  QPushButton *button_cancel = new QPushButton("&Cancel", window_input.window);
  QObject::connect(button_cancel, &QPushButton::clicked, window_input.window, &QDialog::close);
  hbox_buttons->addWidget(button_cancel);

  vbox_main->addLayout(hbox_buttons);

  window_input.window->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

QMenu* create_places_menu(QPushButton *button) {
  QMenu *menu = new QMenu(button);
  menu->addAction("Finances...", []() { menuitem_places_finances_activate_cb(); });
  menu->addAction("Shopping...", []() { menuitem_places_shopping_activate_cb(); });
  menu->addAction("Hospital...", []() { menuitem_places_hospital_activate_cb(); });
  menu->addAction("Vault...", []() { menuitem_places_vault_activate_cb(); });
  menu->addAction("Shipping...", []() { menuitem_places_shipping_activate_cb(); });
  button->setMenu(menu);
  return menu;
}

QMenu* create_info_menu(QPushButton *button) {
  QMenu *menu = new QMenu(button);
  menu->addAction("Vaults...", []() { menuitem_info_vaults_activate_cb(); });
  menu->addAction("World Drug Prices...", []() { menuitem_info_world_drug_prices_activate_cb(); });
  menu->addAction("World Cities...", []() { menuitem_info_world_cities_activate_cb(); });
  menu->addAction("Shipment Status...", []() { menuitem_info_shipment_status_activate_cb(); });
  menu->addAction("History...", []() { menuitem_info_history_activate_cb(); });
  button->setMenu(menu);
  return menu;
}
