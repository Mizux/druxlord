#pragma once

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QSlider>
#include <QMenu>
#include <QShortcut>
#include <QKeySequence>

struct WindowMain {
  QWidget *window = nullptr;
  QTextEdit *textview_information = nullptr;
  QTreeWidget *treeview_market = nullptr;
  QTreeWidget *treeview_pocket = nullptr;
  QPushButton *button_buy = nullptr;
  QPushButton *button_sell = nullptr;
  QPushButton *button_dump = nullptr;
  QPushButton *button_places = nullptr;
  QPushButton *button_info = nullptr;
  QPushButton *button_stayhere = nullptr;
  QPushButton *button_flyaway = nullptr;
  QPushButton *button_about = nullptr;
  QPushButton *button_docs = nullptr;
  QPushButton *button_highscores = nullptr;
  QPushButton *button_newgamequit = nullptr;
  QCheckBox *checkbutton_sound = nullptr;
  QLabel *label_pocket = nullptr;
  QLabel *label_location = nullptr;
  QLabel *label_day = nullptr;
  QLabel *label_rank = nullptr;
  QLabel *label_cash = nullptr;
  QLabel *label_bank = nullptr;
  QLabel *label_debt = nullptr;
  QProgressBar *progressbar_health = nullptr;
  QWidget *drawingarea_status = nullptr;
  QShortcut *shortcut_quit = nullptr;
};

struct WindowFinance {
  QDialog *window = nullptr;
  QRadioButton *radiobutton_depositsome = nullptr;
  QRadioButton *radiobutton_depositall = nullptr;
  QRadioButton *radiobutton_depositallbut = nullptr;
  QRadioButton *radiobutton_withdrawsome = nullptr;
  QRadioButton *radiobutton_withdrawall = nullptr;
  QRadioButton *radiobutton_withdrawallbut = nullptr;
  QSpinBox *spinbutton_amount = nullptr;
  QPushButton *button_doit = nullptr;
  QTreeWidget *treeview_loan = nullptr;
  QLabel *label_cash = nullptr;
  QLabel *label_bank = nullptr;
  QLabel *label_debt = nullptr;
  QPushButton *button_borrow = nullptr;
  QPushButton *button_repay = nullptr;
  QPushButton *button_done = nullptr;
};

struct WindowShopping {
  QDialog *window = nullptr;
  QTreeWidget *treeview_store = nullptr;
  QTreeWidget *treeview_inventory = nullptr;
  QPushButton *button_buy = nullptr;
  QPushButton *button_sell = nullptr;
  QPushButton *button_done = nullptr;
  QLabel *label_cash = nullptr;
};

struct WindowHospital {
  QDialog *window = nullptr;
  QProgressBar *progressbar_health = nullptr;
  QSlider *scalebutton_health = nullptr;
  QLabel *label_cash = nullptr;
  QLabel *label_cost = nullptr;
  QPushButton *button_ok = nullptr;
};

struct WindowVault {
  QDialog *window = nullptr;
  QTreeWidget *treeview_pocket = nullptr;
  QTreeWidget *treeview_vault = nullptr;
  QPushButton *button_intovault = nullptr;
  QPushButton *button_fromvault = nullptr;
  QPushButton *button_ok = nullptr;
};

struct WindowInput {
  QDialog *window = nullptr;
  QSpinBox *spinbutton_value = nullptr;
  QPushButton *button_ok = nullptr;
};

enum {
  COLUMN_STATUS,
  COLUMN_NAME,
  COLUMN_QTY,
  COLUMN_PRICE
};

enum {
  COLUMN_LOAN_NAME,
  COLUMN_LOAN_PAY,
  COLUMN_LOAN_RATE,
  COLUMN_LOAN_DAY,
  COLUMN_LOAN_DEBT,
  COLUMN_LOAN_DAYLEFT
};

enum {
  COLUMN_STORE_NAME,
  COLUMN_STORE_TYPE,
  COLUMN_STORE_PRICE
};

enum {
  COLUMN_INVENTORY_NAME,
  COLUMN_INVENTORY_TYPE,
  COLUMN_INVENTORY_QTY,
  COLUMN_INVENTORY_SELLFOR
};

extern WindowMain window_main;
extern WindowFinance window_finance;
extern WindowShopping window_shopping;
extern WindowHospital window_hospital;
extern WindowVault window_vault;
extern WindowInput window_input;

void create_window_main();
void create_window_finance();
void create_window_shopping();
void create_window_hospital();
void create_window_vault();
void create_window_input(const char *title,
                         const char *message,
                         const char *question);

void insert_treeview_drug(QTreeWidget *treeview);
void set_label_frame_pocket(int npocket);
void set_label_location(int location);
void set_label_day(int day);
void set_label_rank(int rank);
void set_label_cash(int value);
void set_label_bank(int value);
void set_label_debt(int value);
QMenu* create_places_menu(QPushButton *button);
QMenu* create_info_menu(QPushButton *button);
