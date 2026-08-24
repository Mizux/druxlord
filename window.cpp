#include "window.h"

#include <gtk/gtk.h>
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
WindowInput window_input{};

void insert_treeview_drug(GtkTreeView *treeview) {
  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(treeview);
  int j = 0;

  gtk_list_store_clear(GTK_LIST_STORE(model));

  for (int i = 0; i < DRUG_NUM; ++i) {
    if (drug_table[i][j].available) {
      std::string price_str = money_string(drug_table[i][j].price);
      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                         COLUMN_NAME, drug_name[i],
                         COLUMN_QTY, drug_table[i][j].qty,
                         COLUMN_PRICE, price_str.c_str(), -1);
    }
  }
}

static GtkWidget* create_treeview_drug(gboolean with_status) {
  GtkWidget *treeview = gtk_tree_view_new();
  GtkListStore *store = nullptr;

  if (with_status) {
    store = gtk_list_store_new(4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
  } else {
    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_LONG);
  }
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

  if (with_status) {
    GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "", renderer, "pixbuf", COLUMN_STATUS, nullptr);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  }

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
      "Name", renderer, "text", COLUMN_NAME, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, COLUMN_NAME_WIDTH);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Qty", renderer, "text", COLUMN_QTY, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, COLUMN_QTY_WIDTH);
  gtk_tree_view_column_set_alignment(column, 0.9f);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Price", renderer, "text", COLUMN_PRICE, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, COLUMN_PRICE_WIDTH);
  gtk_tree_view_column_set_alignment(column, 0.9f);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  return treeview;
}

void set_label_frame_pocket(int npocket) {
  auto str = std::format("You pants pocket ({}/10)", npocket);
  gtk_label_set_text(GTK_LABEL(window_main.label_pocket), str.c_str());
}

void set_label_location(int location) {
  constexpr std::array location_str = {"Austin, US"};
  auto markup = std::format("<span><b>{}</b></span>", location_str[0]);
  gtk_label_set_markup(GTK_LABEL(window_main.label_location), markup.c_str());
}

void set_label_day(int day) {
  auto markup = std::format("<span><b>{}/30</b></span>", day);
  gtk_label_set_markup(GTK_LABEL(window_main.label_day), markup.c_str());
}

void set_label_rank(int rank) {
  constexpr std::array rank_str = {"wannabe"};
  auto markup = std::format("<span><b>{}</b></span>", rank_str[0]);
  gtk_label_set_markup(GTK_LABEL(window_main.label_rank), markup.c_str());
}

void set_label_cash(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  gtk_label_set_markup(GTK_LABEL(window_main.label_cash), markup.c_str());
}

void set_label_bank(int value) {
  auto markup = std::format("<span><b>{}</b></span>", value);
  gtk_label_set_markup(GTK_LABEL(window_main.label_bank), markup.c_str());
}

void set_label_debt(int value) {
  std::string markup;
  if (value > 0) {
    markup = std::format("<span foreground=\"#FF0000\"><b>{}</b></span>", value);
  } else {
    markup = std::format("<span><b>{}</b></span>", value);
  }
  gtk_label_set_markup(GTK_LABEL(window_main.label_debt), markup.c_str());
}

void create_window_main(GtkApplication *app) {
  if (app) {
    window_main.window = gtk_application_window_new(app);
  } else {
    window_main.window = gtk_window_new();
  }
  gtk_window_set_title(GTK_WINDOW(window_main.window), PROGRAM_NAME);
  gtk_window_set_resizable(GTK_WINDOW(window_main.window), FALSE);
  g_signal_connect(window_main.window, "close-request",
                   G_CALLBACK(window_main_window_close_request_cb), nullptr);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_main.window), vbox_main);

  GtkWidget *frame = gtk_frame_new("Information");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 120);
  gtk_widget_set_margin_top(scrolled_window, 5);
  gtk_widget_set_margin_bottom(scrolled_window, 5);
  gtk_widget_set_margin_start(scrolled_window, 5);
  gtk_widget_set_margin_end(scrolled_window, 5);
  gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

  window_main.textview_information = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(window_main.textview_information), FALSE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_main.textview_information);

  GtkWidget *hbox_down = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(vbox_main), hbox_down);

  frame = gtk_frame_new("The Market");
  gtk_box_append(GTK_BOX(hbox_down), frame);

  scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_margin_top(scrolled_window, 5);
  gtk_widget_set_margin_bottom(scrolled_window, 5);
  gtk_widget_set_margin_start(scrolled_window, 5);
  gtk_widget_set_margin_end(scrolled_window, 5);
  gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

  window_main.treeview_market = create_treeview_drug(TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_main.treeview_market);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(hbox_down), vbox);

  frame = gtk_frame_new("Action");
  gtk_box_append(GTK_BOX(vbox), frame);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_widget_set_margin_top(box, 5);
  gtk_widget_set_margin_bottom(box, 5);
  gtk_widget_set_margin_start(box, 5);
  gtk_widget_set_margin_end(box, 5);
  gtk_frame_set_child(GTK_FRAME(frame), box);

  window_main.button_buy = gtk_button_new_with_mnemonic("_Buy \u2192");
  gtk_box_append(GTK_BOX(box), window_main.button_buy);
  g_signal_connect(window_main.button_buy, "clicked",
                   G_CALLBACK(window_main_button_buy_clicked_cb), nullptr);

  window_main.button_sell = gtk_button_new_with_mnemonic("\u2190 _Sell");
  gtk_box_append(GTK_BOX(box), window_main.button_sell);

  window_main.button_dump = gtk_button_new_with_mnemonic("_Dump");
  gtk_box_append(GTK_BOX(box), window_main.button_dump);

  window_main.button_places = gtk_menu_button_new();
  gtk_menu_button_set_label(GTK_MENU_BUTTON(window_main.button_places), "Places...");
  gtk_menu_button_set_use_underline(GTK_MENU_BUTTON(window_main.button_places), TRUE);
  create_places_menu(window_main.button_places);
  gtk_box_append(GTK_BOX(box), window_main.button_places);

  window_main.button_info = gtk_menu_button_new();
  gtk_menu_button_set_label(GTK_MENU_BUTTON(window_main.button_info), "Info...");
  gtk_menu_button_set_use_underline(GTK_MENU_BUTTON(window_main.button_info), TRUE);
  create_info_menu(window_main.button_info);
  gtk_box_append(GTK_BOX(box), window_main.button_info);

  frame = gtk_frame_new("Tomorrow");
  gtk_box_append(GTK_BOX(vbox), frame);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_widget_set_margin_top(box, 5);
  gtk_widget_set_margin_bottom(box, 5);
  gtk_widget_set_margin_start(box, 5);
  gtk_widget_set_margin_end(box, 5);
  gtk_frame_set_child(GTK_FRAME(frame), box);

  window_main.button_stayhere = gtk_button_new_with_mnemonic("Stay Here");
  g_signal_connect(window_main.button_stayhere, "clicked",
                   G_CALLBACK(window_main_button_stayhere_clicked_cb), nullptr);
  gtk_box_append(GTK_BOX(box), window_main.button_stayhere);

  window_main.button_flyaway = gtk_button_new_with_mnemonic("Fly Away");
  gtk_box_append(GTK_BOX(box), window_main.button_flyaway);

  frame = gtk_frame_new("Game");
  gtk_box_append(GTK_BOX(vbox), frame);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_widget_set_margin_top(box, 5);
  gtk_widget_set_margin_bottom(box, 5);
  gtk_widget_set_margin_start(box, 5);
  gtk_widget_set_margin_end(box, 5);
  gtk_frame_set_child(GTK_FRAME(frame), box);

  window_main.checkbutton_sound = gtk_check_button_new_with_mnemonic("Sou_nd");
  gtk_box_append(GTK_BOX(box), window_main.checkbutton_sound);

  window_main.button_about = gtk_button_new_with_mnemonic("_About");
  gtk_box_append(GTK_BOX(box), window_main.button_about);

  window_main.button_docs = gtk_button_new_with_mnemonic("Docs");
  gtk_box_append(GTK_BOX(box), window_main.button_docs);

  window_main.button_highscores = gtk_button_new_with_mnemonic("High Scores");
  gtk_box_append(GTK_BOX(box), window_main.button_highscores);

  window_main.button_newgamequit = gtk_button_new_with_mnemonic("New _Game");
  gtk_box_append(GTK_BOX(box), window_main.button_newgamequit);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(hbox_down), vbox);

  frame = gtk_frame_new(nullptr);
  gtk_box_append(GTK_BOX(vbox), frame);

  window_main.label_pocket = gtk_label_new(nullptr);
  set_label_frame_pocket(0);
  gtk_frame_set_label_widget(GTK_FRAME(frame), window_main.label_pocket);

  scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_margin_top(scrolled_window, 5);
  gtk_widget_set_margin_bottom(scrolled_window, 5);
  gtk_widget_set_margin_start(scrolled_window, 5);
  gtk_widget_set_margin_end(scrolled_window, 5);
  gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

  window_main.treeview_pocket = create_treeview_drug(FALSE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_main.treeview_pocket);

  frame = gtk_frame_new("Status");
  gtk_box_append(GTK_BOX(vbox), frame);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_append(GTK_BOX(vbox), box);

  GtkWidget *label = gtk_label_new("Location:");
  gtk_box_append(GTK_BOX(box), label);

  window_main.label_location = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_location), TRUE);
  gtk_box_append(GTK_BOX(box), window_main.label_location);
  set_label_location(0);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_append(GTK_BOX(vbox), box);

  label = gtk_label_new("Health:");
  gtk_box_append(GTK_BOX(box), label);

  window_main.progressbar_health = gtk_progress_bar_new();
  gtk_widget_set_hexpand(window_main.progressbar_health, TRUE);
  gtk_box_append(GTK_BOX(box), window_main.progressbar_health);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
  gtk_box_append(GTK_BOX(vbox), box);

  GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_append(GTK_BOX(box), box2);

  label = gtk_label_new("Day:");
  gtk_box_append(GTK_BOX(box2), label);

  window_main.label_day = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_day), TRUE);
  set_label_day(0);
  gtk_box_append(GTK_BOX(box2), window_main.label_day);

  box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_append(GTK_BOX(box), box2);

  label = gtk_label_new("Rank:");
  gtk_box_append(GTK_BOX(box2), label);

  window_main.label_rank = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_rank), TRUE);
  set_label_rank(0);
  gtk_box_append(GTK_BOX(box2), window_main.label_rank);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(vbox), box);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 50);
  gtk_box_append(GTK_BOX(box), grid);

  label = gtk_label_new("Cash:");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

  window_main.label_cash = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_cash), TRUE);
  gtk_label_set_xalign(GTK_LABEL(window_main.label_cash), 1.0f);
  set_label_cash(1900);
  gtk_grid_attach(GTK_GRID(grid), window_main.label_cash, 1, 0, 1, 1);

  label = gtk_label_new("Bank:");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

  window_main.label_bank = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_bank), TRUE);
  gtk_label_set_xalign(GTK_LABEL(window_main.label_bank), 1.0f);
  set_label_bank(0);
  gtk_grid_attach(GTK_GRID(grid), window_main.label_bank, 1, 1, 1, 1);

  label = gtk_label_new("Debt:");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

  window_main.label_debt = gtk_label_new(nullptr);
  gtk_label_set_use_markup(GTK_LABEL(window_main.label_debt), TRUE);
  gtk_label_set_xalign(GTK_LABEL(window_main.label_debt), 1.0f);
  set_label_debt(0);
  gtk_grid_attach(GTK_GRID(grid), window_main.label_debt, 1, 2, 1, 1);

  window_main.drawingarea_status = gtk_drawing_area_new();
  gtk_box_append(GTK_BOX(box), window_main.drawingarea_status);
}

void create_window_finance() {
  window_finance.window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window_finance.window), "Finance");
  gtk_window_set_transient_for(GTK_WINDOW(window_finance.window),
                                GTK_WINDOW(window_main.window));
  gtk_window_set_modal(GTK_WINDOW(window_finance.window), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window_finance.window), FALSE);
  g_signal_connect_swapped(window_finance.window, "close-request",
                            G_CALLBACK(gtk_window_destroy), window_finance.window);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_finance.window), vbox_main);

  GtkWidget *frame = gtk_frame_new("Bank");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 130);
  gtk_box_append(GTK_BOX(vbox), hbox);

  GtkWidget *hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  GtkWidget *label = gtk_label_new("Cash:");
  gtk_box_append(GTK_BOX(hbox2), label);

  window_finance.label_cash = gtk_label_new("0");
  gtk_box_append(GTK_BOX(hbox2), window_finance.label_cash);

  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  label = gtk_label_new("In Bank:");
  gtk_box_append(GTK_BOX(hbox2), label);

  window_finance.label_bank = gtk_label_new("0");
  gtk_box_append(GTK_BOX(hbox2), window_finance.label_bank);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
  gtk_box_append(GTK_BOX(vbox), grid);

  window_finance.radiobutton_depositsome = gtk_check_button_new_with_label("Deposit some");
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_depositsome, 0, 0, 1, 1);

  window_finance.radiobutton_depositall = gtk_check_button_new_with_label("Deposit all");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(window_finance.radiobutton_depositall),
                             GTK_CHECK_BUTTON(window_finance.radiobutton_depositsome));
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_depositall, 1, 0, 1, 1);

  window_finance.radiobutton_depositallbut = gtk_check_button_new_with_label("Deposit all but");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(window_finance.radiobutton_depositallbut),
                             GTK_CHECK_BUTTON(window_finance.radiobutton_depositsome));
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_depositallbut, 2, 0, 1, 1);

  window_finance.radiobutton_withdrawsome = gtk_check_button_new_with_label("Withdraw some");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(window_finance.radiobutton_withdrawsome),
                             GTK_CHECK_BUTTON(window_finance.radiobutton_depositsome));
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_withdrawsome, 0, 1, 1, 1);

  window_finance.radiobutton_withdrawall = gtk_check_button_new_with_label("Withdraw all");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(window_finance.radiobutton_withdrawall),
                             GTK_CHECK_BUTTON(window_finance.radiobutton_depositsome));
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_withdrawall, 1, 1, 1, 1);

  window_finance.radiobutton_withdrawallbut = gtk_check_button_new_with_label("Withdraw all but");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(window_finance.radiobutton_withdrawallbut),
                             GTK_CHECK_BUTTON(window_finance.radiobutton_depositsome));
  gtk_grid_attach(GTK_GRID(grid), window_finance.radiobutton_withdrawallbut, 2, 1, 1, 1);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_append(GTK_BOX(vbox), hbox);

  label = gtk_label_new("Amount");
  gtk_box_append(GTK_BOX(hbox), label);

  window_finance.spinbutton_amount = gtk_spin_button_new_with_range(1.0, 1000000000.0, 1.0);
  gtk_box_append(GTK_BOX(hbox), window_finance.spinbutton_amount);

  window_finance.button_doit = gtk_button_new_with_label("Do it!");
  gtk_box_append(GTK_BOX(hbox), window_finance.button_doit);

  frame = gtk_frame_new("Loans");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 170);
  gtk_box_append(GTK_BOX(vbox), scrolled_window);

  GtkListStore *store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT,
                                          G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

  window_finance.treeview_loan = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_finance.treeview_loan);

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
      "Name", renderer, "text", COLUMN_LOAN_NAME, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Pays", renderer, "text", COLUMN_LOAN_PAY, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Rate", renderer, "text", COLUMN_LOAN_RATE, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Days", renderer, "text", COLUMN_LOAN_DAY, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Debt", renderer, "text", COLUMN_LOAN_DEBT, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Days Left", renderer, "text", COLUMN_LOAN_DAYLEFT, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_finance.treeview_loan), column);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(vbox), hbox);

  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  label = gtk_label_new("Debt:");
  gtk_box_append(GTK_BOX(hbox2), label);

  window_finance.label_debt = gtk_label_new("0");
  gtk_box_append(GTK_BOX(hbox2), window_finance.label_debt);

  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  window_finance.button_borrow = gtk_button_new_with_label("Borrow");
  gtk_box_append(GTK_BOX(hbox2), window_finance.button_borrow);

  window_finance.button_repay = gtk_button_new_with_label("Repay");
  gtk_box_append(GTK_BOX(hbox2), window_finance.button_repay);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_widget_set_halign(hbox, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  window_finance.button_done = gtk_button_new_with_label("Done");
  gtk_box_append(GTK_BOX(hbox), window_finance.button_done);
  g_signal_connect_swapped(window_finance.button_done, "clicked",
                            G_CALLBACK(gtk_window_destroy), window_finance.window);
}

void create_window_shopping() {
  window_shopping.window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window_shopping.window), "Shopping");
  gtk_window_set_transient_for(GTK_WINDOW(window_shopping.window),
                                GTK_WINDOW(window_main.window));
  gtk_window_set_modal(GTK_WINDOW(window_shopping.window), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window_shopping.window), FALSE);
  g_signal_connect_swapped(window_shopping.window, "close-request",
                            G_CALLBACK(gtk_window_destroy), window_shopping.window);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_shopping.window), vbox_main);

  GtkWidget *frame = gtk_frame_new("Store's Inventory");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 220);
  gtk_box_append(GTK_BOX(vbox), scrolled_window);

  GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
  window_shopping.treeview_store = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_shopping.treeview_store);

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
      "Name", renderer, "text", COLUMN_STORE_NAME, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 140);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_store), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Type", renderer, "text", COLUMN_STORE_TYPE, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_store), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Price", renderer, "text", COLUMN_STORE_PRICE, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_store), column);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(vbox), hbox);

  window_shopping.button_buy = gtk_button_new_with_label("   Buy   ");
  gtk_widget_set_halign(window_shopping.button_buy, GTK_ALIGN_END);
  gtk_widget_set_hexpand(window_shopping.button_buy, TRUE);
  gtk_box_append(GTK_BOX(hbox), window_shopping.button_buy);

  frame = gtk_frame_new("Your Inventory");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 150);
  gtk_box_append(GTK_BOX(vbox), scrolled_window);

  store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
  window_shopping.treeview_inventory = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_shopping.treeview_inventory);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Name", renderer, "text", COLUMN_INVENTORY_NAME, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 140);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_inventory), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Type", renderer, "text", COLUMN_INVENTORY_TYPE, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_inventory), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Qty", renderer, "text", COLUMN_INVENTORY_QTY, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_inventory), column);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_renderer_set_alignment(renderer, 0.9f, 0.5f);
  column = gtk_tree_view_column_new_with_attributes(
      "Price", renderer, "text", COLUMN_INVENTORY_SELLFOR, nullptr);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_min_width(column, 70);
  gtk_tree_view_append_column(GTK_TREE_VIEW(window_shopping.treeview_inventory), column);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(vbox), hbox);

  GtkWidget *hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  GtkWidget *label = gtk_label_new("Cash:");
  gtk_box_append(GTK_BOX(hbox2), label);

  window_shopping.label_cash = gtk_label_new("0");
  gtk_box_append(GTK_BOX(hbox2), window_shopping.label_cash);

  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(hbox2, TRUE);
  gtk_widget_set_halign(hbox2, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(hbox), hbox2);

  window_shopping.button_sell = gtk_button_new_with_label("   Sell   ");
  gtk_box_append(GTK_BOX(hbox2), window_shopping.button_sell);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_widget_set_halign(hbox, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  window_shopping.button_done = gtk_button_new_with_label("Done");
  gtk_box_append(GTK_BOX(hbox), window_shopping.button_done);
  g_signal_connect_swapped(window_shopping.button_done, "clicked",
                            G_CALLBACK(gtk_window_destroy), window_shopping.window);
}

void create_window_hospital() {
  window_hospital.window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window_hospital.window), "Hospital");
  gtk_window_set_transient_for(GTK_WINDOW(window_hospital.window),
                                GTK_WINDOW(window_main.window));
  gtk_window_set_modal(GTK_WINDOW(window_hospital.window), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window_hospital.window), FALSE);
  g_signal_connect_swapped(window_hospital.window, "close-request",
                            G_CALLBACK(gtk_window_destroy), window_hospital.window);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_hospital.window), vbox_main);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  GtkWidget *frame = gtk_frame_new("");
  gtk_box_append(GTK_BOX(hbox), frame);

  GtkWidget *hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_margin_top(hbox2, 20);
  gtk_widget_set_margin_bottom(hbox2, 20);
  gtk_widget_set_margin_start(hbox2, 20);
  gtk_widget_set_margin_end(hbox2, 20);
  gtk_frame_set_child(GTK_FRAME(frame), hbox2);

  GtkWidget *image = gtk_image_new_from_icon_name("help-browser");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 48);
  gtk_box_append(GTK_BOX(hbox2), image);

  frame = gtk_frame_new("Move the pointer to desired health");
  gtk_widget_set_size_request(frame, 400, -1);
  gtk_box_append(GTK_BOX(hbox), frame);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_box_append(GTK_BOX(vbox), vbox2);

  window_hospital.progressbar_health = gtk_progress_bar_new();
  gtk_box_append(GTK_BOX(vbox2), window_hospital.progressbar_health);

  window_hospital.scalebutton_health = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0, 100.0, 1.0);
  gtk_scale_set_draw_value(GTK_SCALE(window_hospital.scalebutton_health), FALSE);
  gtk_box_append(GTK_BOX(vbox2), window_hospital.scalebutton_health);

  vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_box_append(GTK_BOX(vbox), vbox2);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
  gtk_box_append(GTK_BOX(vbox2), grid);

  GtkWidget *label = gtk_label_new("Cash:");
  gtk_label_set_xalign(GTK_LABEL(label), 1.0f);
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

  window_hospital.label_cash = gtk_label_new("0");
  gtk_grid_attach(GTK_GRID(grid), window_hospital.label_cash, 1, 0, 1, 1);

  label = gtk_label_new("Cost for treatment:");
  gtk_label_set_xalign(GTK_LABEL(label), 1.0f);
  gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

  window_hospital.label_cost = gtk_label_new("0");
  gtk_grid_attach(GTK_GRID(grid), window_hospital.label_cost, 1, 1, 1, 1);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_widget_set_halign(hbox, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  window_hospital.button_ok = gtk_button_new_with_mnemonic("_OK");
  gtk_box_append(GTK_BOX(hbox), window_hospital.button_ok);

  GtkWidget *button = gtk_button_new_with_mnemonic("_Cancel");
  gtk_box_append(GTK_BOX(hbox), button);
  g_signal_connect_swapped(button, "clicked",
                            G_CALLBACK(gtk_window_destroy), window_hospital.window);
}

void create_window_vault() {
  window_vault.window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window_vault.window), "The Vault");
  gtk_window_set_transient_for(GTK_WINDOW(window_vault.window),
                                GTK_WINDOW(window_main.window));
  gtk_window_set_modal(GTK_WINDOW(window_vault.window), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window_vault.window), FALSE);
  g_signal_connect_swapped(window_vault.window, "close-request",
                            G_CALLBACK(gtk_window_destroy), window_vault.window);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_vault.window), vbox_main);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  GtkWidget *frame = gtk_frame_new("You pants pocket (0/10)");
  gtk_box_append(GTK_BOX(hbox), frame);

  GtkWidget *scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 200);
  gtk_widget_set_margin_top(scrolled_window, 5);
  gtk_widget_set_margin_bottom(scrolled_window, 5);
  gtk_widget_set_margin_start(scrolled_window, 5);
  gtk_widget_set_margin_end(scrolled_window, 5);
  gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

  window_vault.treeview_pocket = create_treeview_drug(FALSE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_vault.treeview_pocket);

  frame = gtk_frame_new("Move");
  gtk_box_append(GTK_BOX(hbox), frame);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
  gtk_widget_set_margin_top(vbox, 5);
  gtk_widget_set_margin_bottom(vbox, 5);
  gtk_widget_set_margin_start(vbox, 5);
  gtk_widget_set_margin_end(vbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), vbox);

  window_vault.button_intovault = gtk_button_new_with_label("Into Vault \u2192");
  gtk_box_append(GTK_BOX(vbox), window_vault.button_intovault);

  window_vault.button_fromvault = gtk_button_new_with_label("\u2190 From Vault");
  gtk_box_append(GTK_BOX(vbox), window_vault.button_fromvault);

  frame = gtk_frame_new("In the Vault");
  gtk_box_append(GTK_BOX(hbox), frame);

  scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
  gtk_widget_set_size_request(scrolled_window, -1, 210);
  gtk_widget_set_margin_top(scrolled_window, 5);
  gtk_widget_set_margin_bottom(scrolled_window, 5);
  gtk_widget_set_margin_start(scrolled_window, 5);
  gtk_widget_set_margin_end(scrolled_window, 5);
  gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

  window_vault.treeview_vault = create_treeview_drug(FALSE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), window_vault.treeview_vault);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_widget_set_halign(hbox, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  window_vault.button_ok = gtk_button_new_with_mnemonic("_OK");
  gtk_box_append(GTK_BOX(hbox), window_vault.button_ok);

  GtkWidget *button = gtk_button_new_with_mnemonic("_Cancel");
  gtk_box_append(GTK_BOX(hbox), button);
  g_signal_connect_swapped(button, "clicked",
                            G_CALLBACK(gtk_window_destroy), window_vault.window);
}

void create_window_input(const gchar *title,
                         const gchar *message,
                         const gchar *question) {
  window_input.window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window_input.window), title);
  gtk_window_set_transient_for(GTK_WINDOW(window_input.window),
                                GTK_WINDOW(window_main.window));
  gtk_window_set_modal(GTK_WINDOW(window_input.window), TRUE);
  gtk_window_set_resizable(GTK_WINDOW(window_input.window), FALSE);
  g_signal_connect_swapped(window_input.window, "close-request",
                            G_CALLBACK(gtk_window_destroy), window_input.window);

  GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_top(vbox_main, 5);
  gtk_widget_set_margin_bottom(vbox_main, 5);
  gtk_widget_set_margin_start(vbox_main, 5);
  gtk_widget_set_margin_end(vbox_main, 5);
  gtk_window_set_child(GTK_WINDOW(window_input.window), vbox_main);

  GtkWidget *frame = gtk_frame_new("Message");
  gtk_box_append(GTK_BOX(vbox_main), frame);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_margin_top(hbox, 5);
  gtk_widget_set_margin_bottom(hbox, 5);
  gtk_widget_set_margin_start(hbox, 5);
  gtk_widget_set_margin_end(hbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), hbox);

  GtkWidget *label = gtk_label_new(message);
  gtk_box_append(GTK_BOX(hbox), label);

  frame = gtk_frame_new(nullptr);
  gtk_box_append(GTK_BOX(vbox_main), frame);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_widget_set_margin_top(hbox, 5);
  gtk_widget_set_margin_bottom(hbox, 5);
  gtk_widget_set_margin_start(hbox, 5);
  gtk_widget_set_margin_end(hbox, 5);
  gtk_frame_set_child(GTK_FRAME(frame), hbox);

  label = gtk_label_new(question);
  gtk_label_set_xalign(GTK_LABEL(label), 1.0f);
  gtk_widget_set_hexpand(label, TRUE);
  gtk_box_append(GTK_BOX(hbox), label);

  window_input.spinbutton_value = gtk_spin_button_new_with_range(1.0, 1000000.0, 1.0);
  gtk_spin_button_set_digits(GTK_SPIN_BUTTON(window_input.spinbutton_value), 0);
  gtk_box_append(GTK_BOX(hbox), window_input.spinbutton_value);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_widget_set_halign(hbox, GTK_ALIGN_END);
  gtk_box_append(GTK_BOX(vbox_main), hbox);

  window_input.button_ok = gtk_button_new_with_mnemonic("_OK");
  gtk_box_append(GTK_BOX(hbox), window_input.button_ok);

  GtkWidget *button = gtk_button_new_with_mnemonic("_Cancel");
  gtk_box_append(GTK_BOX(hbox), button);
  g_signal_connect_swapped(button, "clicked",
                            G_CALLBACK(gtk_window_destroy), window_input.window);
}

GtkWidget* create_places_menu(GtkWidget *button) {
  GtkWidget *popover = gtk_popover_new();
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_popover_set_child(GTK_POPOVER(popover), box);

  struct MenuItemInfo {
    const char *label;
    GCallback cb;
  };

  const std::array<MenuItemInfo, 5> items = {{
    {"Finances...", G_CALLBACK(menuitem_places_finances_activate_cb)},
    {"Shopping...", G_CALLBACK(menuitem_places_shopping_activate_cb)},
    {"Hospital...", G_CALLBACK(menuitem_places_hospital_activate_cb)},
    {"Vault...", G_CALLBACK(menuitem_places_vault_activate_cb)},
    {"Shipping...", G_CALLBACK(menuitem_places_shipping_activate_cb)},
  }};

  for (const auto &item : items) {
    GtkWidget *btn = gtk_button_new_with_label(item.label);
    gtk_button_set_has_frame(GTK_BUTTON(btn), FALSE);
    g_signal_connect(btn, "clicked", item.cb, nullptr);
    g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_popover_popdown), popover);
    gtk_box_append(GTK_BOX(box), btn);
  }

  gtk_menu_button_set_popover(GTK_MENU_BUTTON(button), popover);
  return popover;
}

GtkWidget* create_info_menu(GtkWidget *button) {
  GtkWidget *popover = gtk_popover_new();
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_popover_set_child(GTK_POPOVER(popover), box);

  struct MenuItemInfo {
    const char *label;
    GCallback cb;
  };

  const std::array<MenuItemInfo, 5> items = {{
    {"Vaults...", G_CALLBACK(menuitem_info_vaults_activate_cb)},
    {"World Drug Prices...", G_CALLBACK(menuitem_info_drug_prices_activate_cb)},
    {"World Cities...", G_CALLBACK(menuitem_info_world_cities_activate_cb)},
    {"Shipment Status...", G_CALLBACK(menuitem_info_shipment_status_activate_cb)},
    {"History...", G_CALLBACK(menuitem_info_history_activate_cb)},
  }};

  for (const auto &item : items) {
    GtkWidget *btn = gtk_button_new_with_label(item.label);
    gtk_button_set_has_frame(GTK_BUTTON(btn), FALSE);
    g_signal_connect(btn, "clicked", item.cb, nullptr);
    g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_popover_popdown), popover);
    gtk_box_append(GTK_BOX(box), btn);
  }

  gtk_menu_button_set_popover(GTK_MENU_BUTTON(button), popover);
  return popover;
}
