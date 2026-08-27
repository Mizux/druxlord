#include "window-cb.h"
#include "window.h"
#include "druxlord.h"

void window_main_button_buy_clicked_cb() {
  create_window_input("input", "message", "how many?");
  window_input.window->show();
  window_input.window->raise();
  window_input.window->activateWindow();
}

void window_main_button_stayhere_clicked_cb() {
  if (window_main.game_state) {
    window_main.game_state->stay_here();
    set_label_day(window_main.game_state->day);
    set_label_rank(window_main.game_state->rank);
    set_label_cash(window_main.game_state->cash);
    set_label_bank(window_main.game_state->bank);
    set_label_debt(window_main.game_state->debt);
    set_label_frame_pocket(window_main.game_state->pocket,
                           window_main.game_state->pocket_capacity);
    insert_treeview_drug(window_main.treeview_market, *window_main.game_state);
    if (window_main.textview_information) {
      std::string news = window_main.game_state->get_market_news(
          window_main.game_state->location, window_main.game_state->day);
      window_main.textview_information->setText(QString::fromStdString(news));
    }
  }
}

void menuitem_places_finances_activate_cb() {
  create_window_finance();
  window_finance.window->show();
  window_finance.window->raise();
  window_finance.window->activateWindow();
}

void menuitem_places_shopping_activate_cb() {
  create_window_shopping();
  window_shopping.window->show();
  window_shopping.window->raise();
  window_shopping.window->activateWindow();
}

void menuitem_places_hospital_activate_cb() {
  create_window_hospital();
  window_hospital.window->show();
  window_hospital.window->raise();
  window_hospital.window->activateWindow();
}

void menuitem_places_vault_activate_cb() {
  create_window_vault();
  window_vault.window->show();
  window_vault.window->raise();
  window_vault.window->activateWindow();
}

void menuitem_places_shipping_activate_cb() {
}

void menuitem_info_vaults_activate_cb() {
}

void menuitem_info_drug_prices_activate_cb() {
}

void menuitem_info_world_cities_activate_cb() {
}

void menuitem_info_shipment_status_activate_cb() {
}

void menuitem_info_history_activate_cb() {
}
