#pragma once

class MainWindow;

void window_main_button_buy_clicked_cb(MainWindow& window);
void window_main_button_sell_clicked_cb(MainWindow& window);
void window_main_button_dump_clicked_cb(MainWindow& window);

void menuitem_places_finances_activate_cb(MainWindow& window);
void menuitem_places_shopping_activate_cb(MainWindow& window);
void menuitem_places_hospital_activate_cb(MainWindow& window);
void menuitem_places_vault_activate_cb(MainWindow& window);
void menuitem_places_shipping_activate_cb(MainWindow& window);

void menuitem_info_vaults_activate_cb(MainWindow& window);
void menuitem_info_world_drug_prices_activate_cb(MainWindow& window);
void menuitem_info_world_cities_activate_cb(MainWindow& window);
void menuitem_info_shipment_status_activate_cb(MainWindow& window);
void menuitem_info_history_activate_cb(MainWindow& window);

void window_main_button_stayhere_clicked_cb(MainWindow& window);
void window_main_button_flyaway_clicked_cb(MainWindow& window);
void window_main_button_about_clicked_cb(MainWindow& window);
void window_main_button_docs_clicked_cb(MainWindow& window);
void window_main_button_highscores_clicked_cb(MainWindow& window);
void window_main_button_newgamequit_clicked_cb(MainWindow& window);
