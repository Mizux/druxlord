#include "tui.h"

#include <ncurses.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "config.h"

namespace {

// Color Pair IDs (btop-inspired palette)
enum ColorPair : short {
  CP_DEFAULT = 1,
  CP_BORDER = 2,
  CP_BORDER_ACTIVE = 3,
  CP_TITLE = 4,
  CP_KEY = 5,
  CP_GREEN = 6,
  CP_YELLOW = 7,
  CP_RED = 8,
  CP_CYAN = 9,
  CP_MAGENTA = 10,
  CP_SELECTED = 11,
  CP_HEADER_BAR = 12,
  CP_DIM = 13,
  CP_GRAPH_LINE = 14,
  CP_GRAPH_REF = 15,
};

void init_btop_colors() {
  if (!has_colors()) return;
  start_color();
  use_default_colors();

  init_pair(CP_DEFAULT, COLOR_WHITE, -1);
  init_pair(CP_BORDER, COLOR_BLUE, -1);
  init_pair(CP_BORDER_ACTIVE, COLOR_CYAN, -1);
  init_pair(CP_TITLE, COLOR_WHITE, -1);
  init_pair(CP_KEY, COLOR_RED, -1);
  init_pair(CP_GREEN, COLOR_GREEN, -1);
  init_pair(CP_YELLOW, COLOR_YELLOW, -1);
  init_pair(CP_RED, COLOR_RED, -1);
  init_pair(CP_CYAN, COLOR_CYAN, -1);
  init_pair(CP_MAGENTA, COLOR_MAGENTA, -1);
  init_pair(CP_SELECTED, COLOR_BLACK, COLOR_CYAN);
  init_pair(CP_HEADER_BAR, COLOR_WHITE, COLOR_BLUE);
  init_pair(CP_DIM, COLOR_BLUE, -1);
  init_pair(CP_GRAPH_LINE, COLOR_YELLOW, -1);
  init_pair(CP_GRAPH_REF, COLOR_CYAN, -1);
}

std::string rank_string(int rank) {
  constexpr std::array rank_str = {"wannabe",     "small time operator",
                                   "dealer",      "big time dealer",
                                   "distributor", "drug lord"};
  if (rank >= 0 && rank < static_cast<int>(rank_str.size())) {
    return rank_str[rank];
  }
  return "wannabe";
}

// Draw a btop-style rounded box with embedded title and key hints
void draw_btop_box(int y, int x, int h, int w, const std::string& title,
                   const std::string& right_hint = "", bool active = false,
                   short custom_border_cp = 0) {
  if (h < 2 || w < 4) return;
  short b_cp = custom_border_cp
                   ? custom_border_cp
                   : (active ? CP_BORDER_ACTIVE : CP_BORDER);

  attron(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));
  mvaddstr(y, x, "╭");
  for (int i = 1; i < w - 1; ++i) mvaddstr(y, x + i, "─");
  mvaddstr(y, x + w - 1, "╮");

  for (int r = 1; r < h - 1; ++r) {
    mvaddstr(y + r, x, "│");
    for (int c = 1; c < w - 1; ++c) mvaddch(y + r, x + c, ' ');
    mvaddstr(y + r, x + w - 1, "│");
  }

  mvaddstr(y + h - 1, x, "╰");
  for (int i = 1; i < w - 1; ++i) mvaddstr(y + h - 1, x + i, "─");
  mvaddstr(y + h - 1, x + w - 1, "╯");
  attroff(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));

  if (!title.empty() && w > 8) {
    attron(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));
    mvaddstr(y, x + 2, "┤ ");
    attroff(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));

    attron(COLOR_PAIR(active ? CP_CYAN : CP_TITLE) | A_BOLD);
    std::string t = title;
    if (static_cast<int>(t.size()) > w - 8) t = t.substr(0, w - 8);
    addstr(t.c_str());
    attroff(COLOR_PAIR(active ? CP_CYAN : CP_TITLE) | A_BOLD);

    attron(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));
    addstr(" ├");
    attroff(COLOR_PAIR(b_cp) | (active ? A_BOLD : A_NORMAL));
  }

  if (!right_hint.empty() &&
      static_cast<int>(right_hint.size() + title.size() + 12) < w) {
    int rx = x + w - static_cast<int>(right_hint.size()) - 6;
    attron(COLOR_PAIR(b_cp));
    mvaddstr(y, rx, "┤ ");
    attroff(COLOR_PAIR(b_cp));

    // Render bracketed key hints with colored keys
    bool in_bracket = false;
    for (char ch : right_hint) {
      if (ch == '[') {
        in_bracket = true;
        attron(COLOR_PAIR(CP_DIM));
        addch('[');
        attroff(COLOR_PAIR(CP_DIM));
      } else if (ch == ']') {
        in_bracket = false;
        attron(COLOR_PAIR(CP_DIM));
        addch(']');
        attroff(COLOR_PAIR(CP_DIM));
      } else if (in_bracket) {
        attron(COLOR_PAIR(CP_KEY) | A_BOLD);
        addch(ch);
        attroff(COLOR_PAIR(CP_KEY) | A_BOLD);
      } else {
        attron(COLOR_PAIR(CP_TITLE));
        addch(ch);
        attroff(COLOR_PAIR(CP_TITLE));
      }
    }

    attron(COLOR_PAIR(b_cp));
    addstr(" ├");
    attroff(COLOR_PAIR(b_cp));
  }
}

void draw_progress_bar(int y, int x, int width, double ratio, short color_cp) {
  if (width <= 0) return;
  ratio = std::clamp(ratio, 0.0, 1.0);
  int filled = static_cast<int>(std::round(ratio * width));
  attron(COLOR_PAIR(color_cp) | A_BOLD);
  for (int i = 0; i < filled; ++i) {
    mvaddstr(y, x + i, "█");
  }
  attroff(COLOR_PAIR(color_cp) | A_BOLD);

  attron(COLOR_PAIR(CP_DIM));
  for (int i = filled; i < width; ++i) {
    mvaddstr(y, x + i, "░");
  }
  attroff(COLOR_PAIR(CP_DIM));
}

std::vector<std::string> wrap_text(const std::string& text, int max_width) {
  std::vector<std::string> lines;
  if (max_width <= 4) return lines;
  std::istringstream paragraphs(text);
  std::string para;
  while (std::getline(paragraphs, para, '\n')) {
    if (para.empty()) {
      lines.push_back("");
      continue;
    }
    std::istringstream words(para);
    std::string word;
    std::string current;
    while (words >> word) {
      if (current.empty()) {
        current = word;
      } else if (static_cast<int>(current.size() + 1 + word.size()) <=
                 max_width) {
        current += " " + word;
      } else {
        lines.push_back(current);
        current = word;
      }
    }
    if (!current.empty()) lines.push_back(current);
  }
  return lines;
}

std::string braille_utf8(uint8_t mask) {
  // Unicode Braille Patterns block starts at U+2800 (UTF-8: E2 A0 80 .. E2 A3 BF)
  char buf[4];
  buf[0] = static_cast<char>(0xE2);
  buf[1] = static_cast<char>(0xA0 | ((mask >> 6) & 0x03));
  buf[2] = static_cast<char>(0x80 | (mask & 0x3F));
  buf[3] = '\0';
  return std::string(buf);
}

}  // namespace

TuiApp::TuiApp() : _gameState() {
  _chartCityIdx = _gameState.location;
  syncSelectionAndChart();
}

TuiApp::~TuiApp() = default;

std::string TuiApp::itemName(int item_idx) {
  switch (item_idx) {
    case ITEM_CASH:
      return "Your Cash";
    case ITEM_DEBT:
      return "Your Debt";
    case ITEM_HEALTH:
      return "Your Health";
    default: {
      int drug_idx = item_idx - ITEM_FIRST_DRUG;
      if (drug_idx >= 0 && drug_idx < static_cast<int>(drug_info.size())) {
        return drug_name(drug_info[drug_idx].id);
      }
      return "Unknown";
    }
  }
}

std::vector<int> TuiApp::availableMarketDrugs() const {
  std::vector<int> list;
  int loc = _gameState.location;
  int day = _gameState.day;
  for (size_t i = 0; i < drug_info.size(); ++i) {
    if (_gameState.drug_table[i][loc][day].available) {
      list.push_back(static_cast<int>(i));
    }
  }
  return list;
}

std::vector<int> TuiApp::ownedPocketDrugs() const {
  std::vector<int> list;
  for (size_t i = 0; i < drug_info.size(); ++i) {
    if (_gameState.player_qty[i] > 0) {
      list.push_back(static_cast<int>(i));
    }
  }
  return list;
}

void TuiApp::syncSelectionAndChart() {
  auto m_list = availableMarketDrugs();
  if (m_list.empty()) {
    _marketCursor = 0;
  } else {
    _marketCursor =
        std::clamp(_marketCursor, 0, static_cast<int>(m_list.size()) - 1);
  }

  auto p_list = ownedPocketDrugs();
  if (p_list.empty()) {
    _pocketCursor = 0;
  } else {
    _pocketCursor =
        std::clamp(_pocketCursor, 0, static_cast<int>(p_list.size()) - 1);
  }

  _chartCityIdx = _gameState.location;
  if (_focus == FocusPane::Market && !m_list.empty()) {
    _chartItemIdx = ITEM_FIRST_DRUG + m_list[_marketCursor];
  } else if (_focus == FocusPane::Pocket && !p_list.empty()) {
    _chartItemIdx = ITEM_FIRST_DRUG + p_list[_pocketCursor];
  }
}

int TuiApp::run() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  set_escdelay(25);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
  init_btop_colors();

  while (_running) {
    _gameState.record_daily_history();
    drawDashboard();

    int ch = getch();
    auto m_list = availableMarketDrugs();
    auto p_list = ownedPocketDrugs();

    switch (ch) {
      case 'q':
      case 'Q':
        if (showConfirmModal("Quit Drux Lord",
                             "Are you sure you want to exit Drux Lord TUI?")) {
          _running = false;
        }
        break;

      case '\t':
        _focus = (_focus == FocusPane::Market) ? FocusPane::Pocket
                                               : FocusPane::Market;
        syncSelectionAndChart();
        break;

      case '1':
        _focus = FocusPane::Market;
        syncSelectionAndChart();
        break;

      case '2':
        _focus = FocusPane::Pocket;
        syncSelectionAndChart();
        break;

      case KEY_UP:
      case 'k':
        if (_focus == FocusPane::Market && !m_list.empty()) {
          _marketCursor =
              (_marketCursor - 1 + static_cast<int>(m_list.size())) %
              static_cast<int>(m_list.size());
          syncSelectionAndChart();
        } else if (_focus == FocusPane::Pocket && !p_list.empty()) {
          _pocketCursor =
              (_pocketCursor - 1 + static_cast<int>(p_list.size())) %
              static_cast<int>(p_list.size());
          syncSelectionAndChart();
        }
        break;

      case KEY_DOWN:
      case 'j':
        if (_focus == FocusPane::Market && !m_list.empty()) {
          _marketCursor = (_marketCursor + 1) % static_cast<int>(m_list.size());
          syncSelectionAndChart();
        } else if (_focus == FocusPane::Pocket && !p_list.empty()) {
          _pocketCursor = (_pocketCursor + 1) % static_cast<int>(p_list.size());
          syncSelectionAndChart();
        }
        break;

      case '\n':
      case KEY_ENTER:
        if (_focus == FocusPane::Market) {
          actionBuyDrug();
        } else {
          actionSellDrug();
        }
        break;

      case 'b':
      case 'B':
        actionBuyDrug();
        break;

      case 's':
      case 'S':
        actionSellDrug();
        break;

      case 'd':
      case 'D':
        actionDumpDrug();
        break;

      case ' ':
        actionStayHere();
        break;

      case 't':
      case 'T':
        actionFlyAway();
        break;

      case 'f':
      case 'F':
        showFinancesDialog();
        break;

      case 'p':
        showShoppingDialog();
        break;

      case 'P':
        showPlacesMenu();
        break;

      case 'h':
        showHospitalDialog();
        break;

      case 'H':
        showHighscoresDialog();
        break;

      case 'v':
        showVaultDialog();
        break;

      case 'V':
        showVaultsInfoDialog();
        break;

      case 'w':
      case 'W':
        showWorldDrugPricesDialog();
        break;

      case 'l':
      case 'L':
        showWorldCitiesDialog();
        break;

      case 'g':
      case 'G':
      case 'z':
      case 'Z':
        showHistoryDialog(_chartItemIdx, _gameState.location);
        break;

      case 'c':
      case 'C':
        _chartItemIdx = (_chartItemIdx + 1) % TOTAL_ITEMS;
        break;

      case 'i':
      case 'I':
        showInfoMenu();
        break;

      case 'm':
      case 'M':
        _soundEnabled = !_soundEnabled;
        if (_soundEnabled) beep();
        break;

      case 'a':
      case 'A':
        showAboutDialog();
        break;

      case 'o':
      case 'O':
        showDocsDialog();
        break;

      case '?':
      case KEY_F(1):
        showHelpDialog();
        break;

      case 'n':
      case 'N':
        actionNewGame();
        break;

      default:
        break;
    }
  }

  endwin();
  return 0;
}

void TuiApp::drawDashboard() {
  erase();
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);

  if (rows < 22 || cols < 76) {
    draw_btop_box(0, 0, rows, cols, "Drux Lord TUI", "", true, CP_RED);
    attron(COLOR_PAIR(CP_YELLOW) | A_BOLD);
    mvprintw(rows / 2 - 1, std::max(2, (cols - 46) / 2),
             "Terminal window too small (%dx%d)!", cols, rows);
    mvprintw(rows / 2 + 1, std::max(2, (cols - 46) / 2),
             "Please resize to at least 76x22 for btop layout.");
    attroff(COLOR_PAIR(CP_YELLOW) | A_BOLD);
    refresh();
    return;
  }

  drawTopBar(cols);

  int news_h = std::clamp(rows / 4, 6, 8);
  int main_y = 1;
  int main_h = rows - 2 - news_h;
  int left_w = std::max(36, (cols * 44) / 100);
  int right_w = cols - left_w;

  int pocket_h = std::max(8, (main_h * 38) / 100);
  int status_h = std::max(8, (main_h * 32) / 100);
  int graph_h = main_h - pocket_h - status_h;
  if (graph_h < 6) {
    pocket_h = std::max(6, main_h / 3);
    status_h = std::max(7, main_h / 3);
    graph_h = main_h - pocket_h - status_h;
  }

  drawMarketPane(main_y, 0, main_h, left_w);
  drawPocketPane(main_y, left_w, pocket_h, right_w);
  drawStatusPane(main_y + pocket_h, left_w, status_h, right_w);
  drawGraphPane(main_y + pocket_h + status_h, left_w, graph_h, right_w);
  drawNewsPane(main_y + main_h, 0, news_h, cols);
  drawBottomKeyBar(rows - 1, cols);

  refresh();
}

void TuiApp::drawTopBar(int cols) {
  attron(COLOR_PAIR(CP_HEADER_BAR) | A_BOLD);
  for (int c = 0; c < cols; ++c) mvaddch(0, c, ' ');

  std::string loc_str =
      std::format("{}, {}", city_name(city_info[_gameState.location].id),
                  country_name(city_info[_gameState.location].country));
  int net_worth = _gameState.cash + _gameState.bank - _gameState.debt;
  std::string nw_str =
      (net_worth >= 0)
          ? std::format("${}", money_string(static_cast<unsigned>(net_worth)))
          : std::format("-${}",
                        money_string(static_cast<unsigned>(-net_worth)));

  mvprintw(0, 1, "⚡ DRUX LORD v%s │ 📍 %s │ 📅 Day %d/%d │ 👑 %s",
           std::string(kProgramVersion).c_str(), loc_str.c_str(),
           _gameState.day + 1, DAY_NUM, rank_string(_gameState.rank).c_str());

  std::string right_info =
      std::format("Net Worth: {} │ Sound: {} │ [?]Help [q]Quit ", nw_str,
                  _soundEnabled ? "ON" : "OFF");
  if (static_cast<int>(right_info.size()) + 45 < cols) {
    mvaddstr(0, cols - static_cast<int>(right_info.size()), right_info.c_str());
  }
  attroff(COLOR_PAIR(CP_HEADER_BAR) | A_BOLD);
}

void TuiApp::drawMarketPane(int y, int x, int h, int w) {
  bool active = (_focus == FocusPane::Market);
  draw_btop_box(y, x, h, w, "¹The Market", "[b]Buy [Space]Stay [t]Fly", active);

  attron(COLOR_PAIR(CP_CYAN) | A_BOLD);
  mvprintw(y + 1, x + 2, "%-2s %-13s %6s %10s", "St", "Drug", "Qty", "Price");
  attroff(COLOR_PAIR(CP_CYAN) | A_BOLD);

  attron(COLOR_PAIR(CP_BORDER));
  mvaddstr(y + 2, x, "├");
  for (int i = 1; i < w - 1; ++i) mvaddstr(y + 2, x + i, "─");
  mvaddstr(y + 2, x + w - 1, "┤");
  attroff(COLOR_PAIR(CP_BORDER));

  auto list = availableMarketDrugs();
  int loc = _gameState.location;
  int day = _gameState.day;
  int max_rows = h - 4;

  int start_idx = 0;
  if (_marketCursor >= max_rows) {
    start_idx = _marketCursor - max_rows + 1;
  }

  for (int row = 0; row < max_rows; ++row) {
    int idx = start_idx + row;
    if (idx >= static_cast<int>(list.size())) break;
    int drug_idx = list[idx];
    const auto& ds = _gameState.drug_table[drug_idx][loc][day];
    bool selected = (idx == _marketCursor);

    int ry = y + 3 + row;
    if (selected) {
      attron(COLOR_PAIR(active ? CP_SELECTED : CP_CYAN) | A_BOLD);
      for (int c = 1; c < w - 1; ++c) mvaddch(ry, x + c, ' ');
    }

    std::string st = " ";
    short st_cp = CP_DEFAULT;
    if (ds.event_flag > 0) {
      st = "▲";
      st_cp = CP_RED;
    } else if (ds.event_flag < 0) {
      st = "▼";
      st_cp = CP_GREEN;
    }

    if (!selected) attron(COLOR_PAIR(st_cp) | A_BOLD);
    mvaddstr(ry, x + 2, st.c_str());
    if (!selected) attroff(COLOR_PAIR(st_cp) | A_BOLD);

    std::string d_name = drug_name(drug_info[drug_idx].id);
    std::string p_str = "$" + money_string(ds.price);

    if (!selected) {
      if (ds.event_flag > 0)
        attron(COLOR_PAIR(CP_MAGENTA) | A_BOLD);
      else if (ds.event_flag < 0)
        attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
      else
        attron(COLOR_PAIR(CP_DEFAULT));
    }

    mvprintw(ry, x + 5, "%-13s %6d %10s", d_name.c_str(), ds.qty,
             p_str.c_str());

    // Show miniature relative price bar if width permits
    if (w >= 46) {
      int mean =
          (drug_info[drug_idx].price * city_info[loc].price_factor) / 100;
      double ratio =
          std::clamp(static_cast<double>(ds.price) / (mean * 2.0), 0.0, 1.0);
      if (!selected) {
        draw_progress_bar(ry, x + 36, w - 38, ratio,
                          ds.event_flag > 0
                              ? CP_RED
                              : (ds.event_flag < 0 ? CP_GREEN : CP_YELLOW));
      }
    }

    if (selected) {
      attroff(COLOR_PAIR(active ? CP_SELECTED : CP_CYAN) | A_BOLD);
    } else {
      attroff(COLOR_PAIR(CP_MAGENTA) | COLOR_PAIR(CP_GREEN) |
              COLOR_PAIR(CP_DEFAULT) | A_BOLD);
    }
  }
}

void TuiApp::drawPocketPane(int y, int x, int h, int w) {
  bool active = (_focus == FocusPane::Pocket);
  std::string title = std::format("²Your Pants Pocket ({}/{})",
                                  _gameState.pocket, _gameState.pocket_capacity);
  draw_btop_box(y, x, h, w, title, "[s]Sell [d]Dump [v]Vault", active);

  attron(COLOR_PAIR(CP_CYAN) | A_BOLD);
  mvprintw(y + 1, x + 2, "%-13s %5s %10s %10s", "Drug", "Qty", "AvgPaid",
           "MktNow");
  attroff(COLOR_PAIR(CP_CYAN) | A_BOLD);

  auto list = ownedPocketDrugs();
  if (list.empty()) {
    attron(COLOR_PAIR(CP_DIM));
    mvaddstr(y + h / 2, x + 3, "(Your pockets are currently empty)");
    attroff(COLOR_PAIR(CP_DIM));
    return;
  }

  int max_rows = h - 3;
  int start_idx = 0;
  if (_pocketCursor >= max_rows) start_idx = _pocketCursor - max_rows + 1;

  int loc = _gameState.location;
  int day = _gameState.day;

  for (int row = 0; row < max_rows; ++row) {
    int idx = start_idx + row;
    if (idx >= static_cast<int>(list.size())) break;
    int drug_idx = list[idx];
    bool selected = (idx == _pocketCursor);

    int qty = _gameState.player_qty[drug_idx];
    int paid = _gameState.player_price[drug_idx];
    int mkt = _gameState.drug_table[drug_idx][loc][day].price;

    int ry = y + 2 + row;
    if (selected) {
      attron(COLOR_PAIR(active ? CP_SELECTED : CP_CYAN) | A_BOLD);
      for (int c = 1; c < w - 1; ++c) mvaddch(ry, x + c, ' ');
    } else {
      attron(COLOR_PAIR(mkt >= paid ? CP_GREEN : CP_RED));
    }

    mvprintw(ry, x + 2, "%-13s %5d %10s %10s",
             drug_name(drug_info[drug_idx].id).c_str(), qty,
             ("$" + money_string(paid)).c_str(),
             ("$" + money_string(mkt)).c_str());

    if (selected) {
      attroff(COLOR_PAIR(active ? CP_SELECTED : CP_CYAN) | A_BOLD);
    } else {
      attroff(COLOR_PAIR(mkt >= paid ? CP_GREEN : CP_RED));
    }
  }
}

void TuiApp::drawStatusPane(int y, int x, int h, int w) {
  draw_btop_box(y, x, h, w, "Status & Arsenal",
                "[f]Finance [p]Shop [h]Hospital", false);

  int inner_w = w - 4;
  int bar_w = std::max(8, inner_w - 22);

  // Health Bar
  short hp_color =
      (_gameState.health > 65)
          ? CP_GREEN
          : ((_gameState.health > 30) ? CP_YELLOW : CP_RED);
  attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
  mvprintw(y + 1, x + 2, "Health: %3d%% ", _gameState.health);
  attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);
  draw_progress_bar(y + 1, x + 15, bar_w, _gameState.health / 100.0, hp_color);

  // Pocket Capacity Bar
  double p_ratio = (_gameState.pocket_capacity > 0)
                       ? static_cast<double>(_gameState.pocket) /
                             _gameState.pocket_capacity
                       : 0.0;
  attron(COLOR_PAIR(CP_TITLE));
  mvprintw(y + 2, x + 2, "Pocket: %3d%% ",
           static_cast<int>(std::round(p_ratio * 100.0)));
  attroff(COLOR_PAIR(CP_TITLE));
  draw_progress_bar(y + 2, x + 15, bar_w, p_ratio, CP_CYAN);

  // Cash / Bank / Debt
  attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
  mvprintw(y + 3, x + 2, "Cash: $%-13s", money_string(_gameState.cash).c_str());
  attroff(COLOR_PAIR(CP_GREEN) | A_BOLD);

  attron(COLOR_PAIR(CP_CYAN) | A_BOLD);
  mvprintw(y + 3, x + 23, "Bank: $%-13s",
           money_string(_gameState.bank).c_str());
  attroff(COLOR_PAIR(CP_CYAN) | A_BOLD);

  attron(COLOR_PAIR(_gameState.debt > 0 ? CP_RED : CP_GREEN) | A_BOLD);
  mvprintw(y + 4, x + 2, "Debt: $%-13s", money_string(_gameState.debt).c_str());
  attroff(COLOR_PAIR(_gameState.debt > 0 ? CP_RED : CP_GREEN) | A_BOLD);

  // Best Weapon & Armor
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
  std::string w_str = "Fists";
  if (best_weapon >= 0) {
    if (best_weapon == 5 || best_weapon == 6) {
      w_str = std::format("{} ({})", shop_items[best_weapon].ammo_name,
                          _gameState.ammo_qty[best_weapon]);
    } else if (shop_items[best_weapon].ammo_limit > 0) {
      w_str = std::format("{} ({})", shop_items[best_weapon].name,
                          _gameState.ammo_qty[best_weapon]);
    } else {
      w_str = shop_items[best_weapon].name;
    }
  }
  attron(COLOR_PAIR(CP_YELLOW));
  mvprintw(y + 4, x + 23, "Arm: %d │ NoScent: %d", _gameState.total_armor(),
           _gameState.no_scent_cans());
  attroff(COLOR_PAIR(CP_YELLOW));

  if (h >= 7) {
    int vault_total = 0;
    for (int q : _gameState.vault_qty) vault_total += q;
    attron(COLOR_PAIR(CP_TITLE));
    mvprintw(y + 5, x + 2, "Weapon: %-18s Vault: %d units", w_str.c_str(),
             vault_total);
    attroff(COLOR_PAIR(CP_TITLE));
  }
}

void TuiApp::drawGraphPane(int y, int x, int h, int w) {
  std::string title = std::format(
      "📈 {} — {}", itemName(_chartItemIdx),
      city_name(city_info[_chartCityIdx].id));
  draw_btop_box(y, x, h, w, title, "[c]Cycle [g]Zoom [w]World [l]Cities",
                false);

  if (h >= 5 && w >= 16) {
    renderBrailleChart(y + 1, x + 2, h - 2, w - 4, _chartItemIdx, _chartCityIdx,
                       true);
  }
}

void TuiApp::renderBrailleChart(int y, int x, int h, int w, int item_idx,
                                int city_idx, bool show_axes) {
  if (h < 2 || w < 10) return;

  int max_day = std::clamp(_gameState.day, 0, DAY_NUM - 1);
  std::vector<double> values(max_day + 1, 0.0);
  std::vector<bool> traded(max_day + 1, false);
  double min_val = 1e18;
  double max_val = -1e18;
  double ref_min = -1.0, ref_max = -1.0, ref_avg = -1.0;

  auto record = [&](double v) {
    if (v < min_val) min_val = v;
    if (v > max_val) max_val = v;
  };

  if (item_idx == ITEM_CASH) {
    for (int d = 0; d <= max_day; ++d) {
      values[d] =
          (d == _gameState.day) ? _gameState.cash : _gameState.cash_history[d];
      record(values[d]);
    }
  } else if (item_idx == ITEM_DEBT) {
    for (int d = 0; d <= max_day; ++d) {
      values[d] =
          (d == _gameState.day) ? _gameState.debt : _gameState.debt_history[d];
      record(values[d]);
    }
  } else if (item_idx == ITEM_HEALTH) {
    ref_min = 0.0;
    ref_max = 100.0;
    record(0.0);
    record(100.0);
    for (int d = 0; d <= max_day; ++d) {
      values[d] = (d == _gameState.day) ? _gameState.health
                                        : _gameState.health_history[d];
      record(values[d]);
    }
  } else {
    int drug_idx = std::clamp(item_idx - ITEM_FIRST_DRUG, 0,
                              static_cast<int>(drug_info.size()) - 1);
    int base_price = drug_info[drug_idx].price;
    int city_factor = city_info[city_idx].price_factor;
    double mean = (base_price * city_factor) / 100.0;
    double half = mean / 2.0;
    ref_min = mean - half;
    ref_max = mean + half;
    ref_avg = mean;
    record(ref_min);
    record(ref_max);

    for (int d = 0; d <= max_day; ++d) {
      values[d] = _gameState.drug_table[drug_idx][city_idx][d].price;
      traded[d] = _gameState.drug_table[drug_idx][city_idx][d].traded;
      record(values[d]);
    }
  }

  if (min_val >= max_val) {
    min_val = 0.0;
    max_val = std::max(100.0, min_val + 100.0);
  } else {
    double pad = (max_val - min_val) * 0.1;
    min_val = std::max(0.0, min_val - pad);
    max_val += pad;
  }

  int label_w = show_axes ? 9 : 0;
  int plot_x = x + label_w;
  int plot_w = w - label_w;
  int plot_h = h;
  if (plot_w < 6 || plot_h < 2) return;

  // Braille sub-cell pixel grid: (plot_w * 2) x (plot_h * 4)
  int px_w = plot_w * 2;
  int px_h = plot_h * 4;
  std::vector<uint8_t> data_canvas(plot_w * plot_h, 0);
  std::vector<uint8_t> ref_canvas(plot_w * plot_h, 0);

  auto set_dot = [&](std::vector<uint8_t>& canvas, int px, int py) {
    if (px < 0 || px >= px_w || py < 0 || py >= px_h) return;
    int cell_x = px / 2;
    int cell_y = py / 4;
    int sub_x = px % 2;
    int sub_y = py % 4;
    static const uint8_t bit_map[4][2] = {
        {0x01, 0x08}, {0x02, 0x10}, {0x04, 0x20}, {0x40, 0x80}};
    canvas[cell_y * plot_w + cell_x] |= bit_map[sub_y][sub_x];
  };

  auto draw_line = [&](std::vector<uint8_t>& canvas, int x0, int y0, int x1,
                       int y1) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
      set_dot(canvas, x0, y0);
      if (x0 == x1 && y0 == y1) break;
      int e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y0 += sy;
      }
    }
  };

  auto val_to_py = [&](double v) -> int {
    double norm = (v - min_val) / (max_val - min_val);
    norm = std::clamp(norm, 0.0, 1.0);
    return static_cast<int>(std::round((1.0 - norm) * (px_h - 1)));
  };

  auto day_to_px = [&](int d) -> int {
    return static_cast<int>(
        std::round((static_cast<double>(d) / (DAY_NUM - 1)) * (px_w - 1)));
  };

  // Plot reference lines (min/max/avg)
  if (ref_min >= 0) {
    int py = val_to_py(ref_min);
    for (int px = 0; px < px_w; px += 2) set_dot(ref_canvas, px, py);
  }
  if (ref_max >= 0) {
    int py = val_to_py(ref_max);
    for (int px = 0; px < px_w; px += 2) set_dot(ref_canvas, px, py);
  }
  if (ref_avg >= 0) {
    int py = val_to_py(ref_avg);
    for (int px = 0; px < px_w; px += 4) set_dot(ref_canvas, px, py);
  }

  // Plot data curve
  for (int d = 0; d <= max_day; ++d) {
    int x0 = day_to_px(d);
    int y0 = val_to_py(values[d]);
    set_dot(data_canvas, x0, y0);
    if (d < max_day) {
      int x1 = day_to_px(d + 1);
      int y1 = val_to_py(values[d + 1]);
      draw_line(data_canvas, x0, y0, x1, y1);
    }
  }

  // Render Y axis labels
  if (show_axes) {
    attron(COLOR_PAIR(CP_DIM));
    mvprintw(y, x, "%8s", money_string(static_cast<unsigned>(max_val)).c_str());
    if (plot_h >= 3) {
      double mid = (max_val + min_val) / 2.0;
      mvprintw(y + plot_h / 2, x, "%8s",
               money_string(static_cast<unsigned>(mid)).c_str());
    }
    mvprintw(y + plot_h - 1, x, "%8s",
             money_string(static_cast<unsigned>(min_val)).c_str());
    attroff(COLOR_PAIR(CP_DIM));
  }

  // Render Braille cells
  for (int cy = 0; cy < plot_h; ++cy) {
    for (int cx = 0; cx < plot_w; ++cx) {
      uint8_t d_mask = data_canvas[cy * plot_w + cx];
      uint8_t r_mask = ref_canvas[cy * plot_w + cx];
      if (d_mask != 0) {
        attron(COLOR_PAIR(CP_GRAPH_LINE) | A_BOLD);
        mvaddstr(y + cy, plot_x + cx, braille_utf8(d_mask | r_mask).c_str());
        attroff(COLOR_PAIR(CP_GRAPH_LINE) | A_BOLD);
      } else if (r_mask != 0) {
        attron(COLOR_PAIR(CP_DIM));
        mvaddstr(y + cy, plot_x + cx, braille_utf8(r_mask).c_str());
        attroff(COLOR_PAIR(CP_DIM));
      }
    }
  }

  // Overlay traded day markers in red
  for (int d = 0; d <= max_day; ++d) {
    if (traded[d]) {
      int cx = day_to_px(d) / 2;
      int cy = val_to_py(values[d]) / 4;
      attron(COLOR_PAIR(CP_RED) | A_BOLD);
      mvaddstr(y + cy, plot_x + cx, "◈");
      attroff(COLOR_PAIR(CP_RED) | A_BOLD);
    }
  }
}

void TuiApp::drawNewsPane(int y, int x, int h, int w) {
  draw_btop_box(y, x, h, w, "📰 Market Intelligence, News & Rumors",
                "[P]Places [I]Info [o]Docs [a]About [n]NewGame", false);

  std::string news =
      _gameState.get_market_news(_gameState.location, _gameState.day);
  if (news.empty()) {
    news = "All quiet on the streets today. Watch the market spreads closely.";
  }

  auto lines = wrap_text(news, w - 4);
  int max_lines = h - 2;
  for (int i = 0; i < max_lines && i < static_cast<int>(lines.size()); ++i) {
    const std::string& line = lines[i];
    short cp = CP_DEFAULT;
    if (line.find("rumor") != std::string::npos) {
      cp = CP_CYAN;
    } else if (line.find("Prices") != std::string::npos ||
               line.find("last day") != std::string::npos) {
      cp = CP_YELLOW;
    }
    attron(COLOR_PAIR(cp));
    mvaddstr(y + 1 + i, x + 2, line.c_str());
    attroff(COLOR_PAIR(cp));
  }
}

void TuiApp::drawBottomKeyBar(int y, int cols) {
  attron(COLOR_PAIR(CP_HEADER_BAR));
  for (int c = 0; c < cols; ++c) mvaddch(y, c, ' ');
  std::string bar =
      " [Tab]Pane [↑↓/jk]Select [b]Buy [s]Sell [d]Dump [Space]Stay [t]Fly "
      "[f]Finance [p]Shop [h]Hosp [v]Vault [w]World [l]Cities [g]Graph [q]Quit";
  if (static_cast<int>(bar.size()) > cols) bar = bar.substr(0, cols);
  mvaddstr(y, 0, bar.c_str());
  attroff(COLOR_PAIR(CP_HEADER_BAR));
}

void TuiApp::showMessageModal(const std::string& title,
                              const std::string& message, int border_color) {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 16, 44, 68);
  auto lines = wrap_text(message, w - 6);
  int h = std::clamp(static_cast<int>(lines.size()) + 6, 8, rows - 4);
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, title, "[Enter/Esc] OK", true, border_color);

    for (int i = 0; i < h - 5 && i < static_cast<int>(lines.size()); ++i) {
      attron(COLOR_PAIR(CP_TITLE));
      mvaddstr(y + 2 + i, x + 3, lines[i].c_str());
      attroff(COLOR_PAIR(CP_TITLE));
    }

    attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    mvaddstr(y + h - 2, x + (w - 10) / 2, "  [ OK ]  ");
    attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    refresh();

    int ch = getch();
    if (ch == '\n' || ch == KEY_ENTER || ch == ' ' || ch == 27 || ch == 'q') {
      break;
    }
  }
}

bool TuiApp::showConfirmModal(const std::string& title,
                              const std::string& message) {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 16, 46, 68);
  auto lines = wrap_text(message, w - 6);
  int h = std::clamp(static_cast<int>(lines.size()) + 6, 8, rows - 4);
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;
  bool yes = true;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, title, "[y]Yes [n]No", true, CP_YELLOW);

    for (int i = 0; i < h - 5 && i < static_cast<int>(lines.size()); ++i) {
      attron(COLOR_PAIR(CP_TITLE));
      mvaddstr(y + 2 + i, x + 3, lines[i].c_str());
      attroff(COLOR_PAIR(CP_TITLE));
    }

    int bx = x + (w - 24) / 2;
    if (yes) attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    mvaddstr(y + h - 2, bx, " [ Yes ] ");
    if (yes) attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);

    if (!yes) attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    mvaddstr(y + h - 2, bx + 13, " [ No ] ");
    if (!yes) attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    refresh();

    int ch = getch();
    if (ch == 'y' || ch == 'Y') return true;
    if (ch == 'n' || ch == 'N' || ch == 27) return false;
    if (ch == KEY_LEFT || ch == KEY_RIGHT || ch == '\t' || ch == 'h' ||
        ch == 'l') {
      yes = !yes;
    } else if (ch == '\n' || ch == KEY_ENTER) {
      return yes;
    }
  }
}

int TuiApp::showInputSpinModal(const std::string& title,
                               const std::string& message,
                               const std::string& question, int min_val,
                               int max_val, int initial_val, int unit_price) {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 16, 50, 68);
  auto lines = wrap_text(message, w - 6);
  int h = std::clamp(static_cast<int>(lines.size()) + 9, 11, rows - 4);
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  int val = std::clamp(initial_val, min_val, std::max(min_val, max_val));
  std::string typed;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, title, "[←/→]Adjust [a]Max [Enter]OK [Esc]Cancel",
                  true);

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
      attron(COLOR_PAIR(CP_TITLE));
      mvaddstr(y + 2 + i, x + 3, lines[i].c_str());
      attroff(COLOR_PAIR(CP_TITLE));
    }

    int qy = y + h - 5;
    attron(COLOR_PAIR(CP_CYAN) | A_BOLD);
    mvprintw(qy, x + 3, "%s: %d (Range: %d..%d)", question.c_str(), val,
             min_val, max_val);
    attroff(COLOR_PAIR(CP_CYAN) | A_BOLD);

    if (unit_price > 0) {
      attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
      mvprintw(qy, x + w - 22, "Total: $%s",
               money_string(static_cast<unsigned>(val * unit_price)).c_str());
      attroff(COLOR_PAIR(CP_GREEN) | A_BOLD);
    }

    double ratio = (max_val > min_val)
                       ? static_cast<double>(val - min_val) /
                             static_cast<double>(max_val - min_val)
                       : 1.0;
    draw_progress_bar(qy + 1, x + 3, w - 6, ratio, CP_CYAN);

    attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    mvaddstr(y + h - 2, x + w / 2 - 12, " [ Confirm ] ");
    attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    attron(COLOR_PAIR(CP_TITLE));
    mvaddstr(y + h - 2, x + w / 2 + 3, " [ Cancel (Esc) ] ");
    attroff(COLOR_PAIR(CP_TITLE));

    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q') return -1;
    if (ch == '\n' || ch == KEY_ENTER) return (max_val >= min_val) ? val : -1;
    if (ch == KEY_LEFT || ch == 'h' || ch == '-') {
      typed.clear();
      val = std::max(min_val, val - 1);
    } else if (ch == KEY_RIGHT || ch == 'l' || ch == '+') {
      typed.clear();
      val = std::min(max_val, val + 1);
    } else if (ch == KEY_PPAGE) {
      typed.clear();
      val = std::min(max_val, val + 10);
    } else if (ch == KEY_NPAGE) {
      typed.clear();
      val = std::max(min_val, val - 10);
    } else if (ch == KEY_HOME || ch == 'm') {
      typed.clear();
      val = min_val;
    } else if (ch == KEY_END || ch == 'a' || ch == 'A') {
      typed.clear();
      val = max_val;
    } else if (ch >= '0' && ch <= '9') {
      typed.push_back(static_cast<char>(ch));
      try {
        int v = std::stoi(typed);
        val = std::clamp(v, min_val, max_val);
      } catch (...) {
        typed.clear();
      }
    } else if (ch == KEY_BACKSPACE || ch == 127) {
      if (!typed.empty()) {
        typed.pop_back();
        val = typed.empty() ? min_val
                            : std::clamp(std::stoi(typed), min_val, max_val);
      }
    }
  }
}

void TuiApp::actionBuyDrug() {
  auto list = availableMarketDrugs();
  if (list.empty()) return;
  _marketCursor =
      std::clamp(_marketCursor, 0, static_cast<int>(list.size()) - 1);
  int drug_idx = list[_marketCursor];

  int j = _gameState.location;
  int d = _gameState.day;
  int price = _gameState.drug_table[drug_idx][j][d].price;
  int available_qty = _gameState.drug_table[drug_idx][j][d].qty;
  int max_afford = (price > 0) ? (_gameState.cash / price) : 0;
  int space = _gameState.pocket_capacity - _gameState.pocket;
  int max_buy = std::min(available_qty, std::min(max_afford, space));

  std::string name = drug_name(drug_info[drug_idx].id);
  if (max_buy <= 0) {
    showMessageModal(
        std::format("Buying {}", name),
        std::format("{} costs ${} per unit. You can buy 0 units (check cash or "
                    "pocket capacity!).",
                    name, money_string(price)),
        CP_RED);
    return;
  }

  std::string msg = std::format(
      "{} is currently selling for ${} per unit. With your available funds and "
      "pocket space, you can buy up to {}.",
      name, money_string(price), max_buy);

  int count = showInputSpinModal(std::format("Buying {}", name), msg,
                                 "How many to buy", 1, max_buy, max_buy, price);
  if (count > 0 && count <= max_buy) {
    int cost = count * price;
    _gameState.cash -= cost;
    _gameState.pocket += count;
    int prev_qty = _gameState.player_qty[drug_idx];
    int prev_price = _gameState.player_price[drug_idx];
    _gameState.player_price[drug_idx] =
        (prev_price * prev_qty + cost) / (prev_qty + count);
    _gameState.player_qty[drug_idx] += count;
    _gameState.drug_table[drug_idx][j][d].qty -= count;
    _gameState.drug_table[drug_idx][j][d].traded = true;
    if (_gameState.drug_table[drug_idx][j][d].qty <= 0) {
      _gameState.drug_table[drug_idx][j][d].available = false;
    }
    syncSelectionAndChart();
  }
}

void TuiApp::actionSellDrug() {
  auto list = ownedPocketDrugs();
  if (list.empty()) return;
  _pocketCursor =
      std::clamp(_pocketCursor, 0, static_cast<int>(list.size()) - 1);
  int drug_idx = list[_pocketCursor];
  int owned_qty = _gameState.player_qty[drug_idx];
  if (owned_qty <= 0) return;

  int j = _gameState.location;
  int d = _gameState.day;
  int price = _gameState.drug_table[drug_idx][j][d].price;
  std::string name = drug_name(drug_info[drug_idx].id);
  std::string msg = std::format(
      "{} is currently being bought for ${} per unit. You have {} to sell.",
      name, money_string(price), owned_qty);

  int count = showInputSpinModal(std::format("Selling {}", name), msg,
                                 "How many to sell", 1, owned_qty, owned_qty,
                                 price);
  if (count > 0 && count <= owned_qty) {
    int gain = count * price;
    _gameState.cash += gain;
    _gameState.pocket -= count;
    _gameState.player_qty[drug_idx] -= count;
    _gameState.drug_table[drug_idx][j][d].traded = true;
    if (_gameState.player_qty[drug_idx] == 0) {
      _gameState.player_price[drug_idx] = 0;
    }
    syncSelectionAndChart();
  }
}

void TuiApp::actionDumpDrug() {
  auto list = ownedPocketDrugs();
  if (list.empty()) return;
  _pocketCursor =
      std::clamp(_pocketCursor, 0, static_cast<int>(list.size()) - 1);
  int drug_idx = list[_pocketCursor];
  int owned_qty = _gameState.player_qty[drug_idx];
  if (owned_qty <= 0) return;

  std::string name = drug_name(drug_info[drug_idx].id);
  std::string msg =
      std::format("You have {} {} in your pocket to dump.", owned_qty, name);

  int count = showInputSpinModal(std::format("Dumping {}", name), msg,
                                 "How many to dump", 1, owned_qty, owned_qty);
  if (count > 0 && count <= owned_qty) {
    _gameState.pocket -= count;
    _gameState.player_qty[drug_idx] -= count;
    if (_gameState.player_qty[drug_idx] == 0) {
      _gameState.player_price[drug_idx] = 0;
    }
    syncSelectionAndChart();
  }
}

void TuiApp::checkPostTurnState() {
  if (_gameState.health <= 0) {
    showMessageModal("Game Over", "YOU WERE KILLED!", CP_RED);
    _gameState.newgame();
    syncSelectionAndChart();
    return;
  }
  if (_gameState.day >= DAY_NUM - 1) {
    int score = _gameState.cash + _gameState.bank - _gameState.debt;
    if (score < 0) score = 0;
    std::string msg = std::format(
        "That's it, the game is over! You have a final score of ${}",
        money_string(score));
    showMessageModal("Game Over", msg, CP_YELLOW);
  }
}

void TuiApp::actionStayHere() {
  Encounter enc = _gameState.check_random_encounter();
  _gameState.stay_here();
  if (enc.type != EncounterType::None) {
    if (enc.type == EncounterType::Combat) {
      showCombatDialog(enc.enemy_idx, enc.enemy_count);
    } else {
      showMessageModal("Random Encounter", enc.message, CP_YELLOW);
    }
  }
  syncSelectionAndChart();
  checkPostTurnState();
}

void TuiApp::actionFlyAway() {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 12, 54, 74);
  int h = std::clamp(static_cast<int>(city_info.size()) + 6, 18, rows - 2);
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  int sel = 0;
  for (size_t i = 0; i < city_info.size(); ++i) {
    if (static_cast<int>(i) != _gameState.location &&
        _gameState.cash >= _gameState.flight_cost(city_info[i].id)) {
      sel = static_cast<int>(i);
      break;
    }
  }

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, "✈ Fly Away — Select Destination",
                  "[↑↓]Select [Enter]Fly [Esc]Cancel", true);

    attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
    mvprintw(y + 1, x + 3, "Available Cash: $%s",
             money_string(_gameState.cash).c_str());
    attroff(COLOR_PAIR(CP_GREEN) | A_BOLD);

    int list_h = h - 4;
    for (int i = 0; i < static_cast<int>(city_info.size()) && i < list_h; ++i) {
      int cost = _gameState.flight_cost(city_info[i].id);
      bool is_current = (i == _gameState.location);
      bool can_afford = (_gameState.cash >= cost);
      bool selected = (i == sel);

      int ry = y + 2 + i;
      if (selected) {
        attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
        for (int c = 1; c < w - 1; ++c) mvaddch(ry, x + c, ' ');
      } else if (is_current || !can_afford) {
        attron(COLOR_PAIR(CP_DIM));
      } else {
        attron(COLOR_PAIR(CP_DEFAULT));
      }

      std::string label =
          std::format("{}, {}", city_name(city_info[i].id),
                      country_name(city_info[i].country));
      std::string status =
          is_current ? "(Current Location)"
                     : (can_afford ? std::format("${}", money_string(cost))
                                   : std::format("${} [Too Expensive]",
                                                 money_string(cost)));
      mvprintw(ry, x + 3, "%-26s %24s", label.c_str(), status.c_str());

      if (selected) {
        attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
      } else {
        attroff(COLOR_PAIR(CP_DIM) | COLOR_PAIR(CP_DEFAULT));
      }
    }
    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q') return;
    if (ch == KEY_UP || ch == 'k') {
      sel = (sel - 1 + static_cast<int>(city_info.size())) %
            static_cast<int>(city_info.size());
    } else if (ch == KEY_DOWN || ch == 'j') {
      sel = (sel + 1) % static_cast<int>(city_info.size());
    } else if (ch == '\n' || ch == KEY_ENTER) {
      int cost = _gameState.flight_cost(city_info[sel].id);
      if (sel != _gameState.location && _gameState.cash >= cost) {
        _gameState.cash -= cost;
        _gameState.location = sel;

        // Drug-sniffing dogs check at airport
        static std::random_device rd;
        static std::mt19937 gen(rd());
        if (_gameState.total_drugs() > 0 && (gen() % 3 == 0)) {
          int needed = std::max(1, _gameState.total_drugs() / 10);
          if (_gameState.no_scent_cans() >= needed) {
            std::string prompt = std::format(
                "To make your drugs undetectable to the drug-sniffing dogs, "
                "you will need to use {} {} of No-Scent. You have {} {}.\nDo "
                "you wish to use them?",
                needed, needed == 1 ? "can" : "cans",
                _gameState.no_scent_cans(),
                _gameState.no_scent_cans() == 1 ? "can" : "cans");
            if (showConfirmModal("The drug sniffing dogs smell something!",
                                 prompt)) {
              _gameState.weapon_qty[11] -= needed;
            } else {
              showMessageModal("Airport Security",
                               "The drug sniffing dogs detected your drugs! "
                               "Airport security is moving in!",
                               CP_RED);
              showCombatDialog(9, 2 + (_gameState.rank + 1) * 2);
            }
          } else {
            std::string msg = std::format(
                "To make your drugs undetectable to the drug-sniffing dogs, "
                "you will need to use {} {} of No-Scent. You have {} {}.\n"
                "Airport security is moving in!",
                needed, needed == 1 ? "can" : "cans",
                _gameState.no_scent_cans(),
                _gameState.no_scent_cans() == 1 ? "can" : "cans");
            showMessageModal("The drug sniffing dogs smell something!", msg,
                             CP_RED);
            showCombatDialog(9, 2 + (_gameState.rank + 1) * 2);
          }
        }

        _gameState.stay_here();
        syncSelectionAndChart();
        checkPostTurnState();
        return;
      }
    }
  }
}

void TuiApp::showFinancesDialog() {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 12, 58, 72);
  int h = 18;
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  constexpr std::array modes = {
      "1. Deposit some",     "2. Deposit all",     "3. Deposit all but",
      "4. Withdraw some",    "5. Withdraw all",    "6. Withdraw all but",
      "7. Borrow $2,000",    "8. Repay Loan Debt"};
  int mode_idx = 0;
  int amount = 1000;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, "🏦 Finances — Bank & Loans",
                  "[↑↓]Select [←/→]Amount [Enter]Do It [b]Borrow [r]Repay [Esc]Done",
                  true);

    attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
    mvprintw(y + 2, x + 3, "Cash:    $%-14s",
             money_string(_gameState.cash).c_str());
    attroff(COLOR_PAIR(CP_GREEN) | A_BOLD);

    attron(COLOR_PAIR(CP_CYAN) | A_BOLD);
    mvprintw(y + 2, x + 28, "In Bank: $%-14s (+5%%/day)",
             money_string(_gameState.bank).c_str());
    attroff(COLOR_PAIR(CP_CYAN) | A_BOLD);

    attron(COLOR_PAIR(_gameState.debt > 0 ? CP_RED : CP_GREEN) | A_BOLD);
    mvprintw(y + 3, x + 3, "Debt:    $%-14s (+10%%/day — Buddles)",
             money_string(_gameState.debt).c_str());
    attroff(COLOR_PAIR(_gameState.debt > 0 ? CP_RED : CP_GREEN) | A_BOLD);

    attron(COLOR_PAIR(CP_YELLOW) | A_BOLD);
    mvprintw(y + 5, x + 3, "Amount Parameter: $%s  (use ←/→ or [e]dit amount)",
             money_string(static_cast<unsigned>(amount)).c_str());
    attroff(COLOR_PAIR(CP_YELLOW) | A_BOLD);

    for (size_t i = 0; i < modes.size(); ++i) {
      bool sel = (static_cast<int>(i) == mode_idx);
      int ry = y + 7 + static_cast<int>(i);
      if (sel) {
        attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
        for (int c = 2; c < w - 2; ++c) mvaddch(ry, x + c, ' ');
      }
      mvprintw(ry, x + 4, "%s %s", sel ? "▶" : " ", modes[i]);
      if (sel) attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    }
    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == KEY_UP || ch == 'k') {
      mode_idx = (mode_idx - 1 + static_cast<int>(modes.size())) %
                 static_cast<int>(modes.size());
    } else if (ch == KEY_DOWN || ch == 'j') {
      mode_idx = (mode_idx + 1) % static_cast<int>(modes.size());
    } else if (ch == KEY_LEFT || ch == 'h') {
      amount = std::max(1, amount - 500);
    } else if (ch == KEY_RIGHT || ch == 'l') {
      amount = std::min(1000000000, amount + 500);
    } else if (ch == 'e' || ch == 'E') {
      int v = showInputSpinModal(
          "Set Bank Amount", "Enter custom amount for bank operation:",
          "Amount ($)", 1, std::max(1, std::max(_gameState.cash, _gameState.bank)),
          amount);
      if (v > 0) amount = v;
    } else if (ch == 'b' || ch == 'B') {
      _gameState.cash += 2000;
      _gameState.debt += 2000;
    } else if (ch == 'r' || ch == 'R') {
      int rep = std::min(_gameState.cash, _gameState.debt);
      _gameState.cash -= rep;
      _gameState.debt -= rep;
    } else if (ch == '\n' || ch == KEY_ENTER) {
      switch (mode_idx) {
        case 0: {
          int a = std::min(amount, _gameState.cash);
          _gameState.cash -= a;
          _gameState.bank += a;
          break;
        }
        case 1:
          _gameState.bank += _gameState.cash;
          _gameState.cash = 0;
          break;
        case 2:
          if (_gameState.cash > amount) {
            int dep = _gameState.cash - amount;
            _gameState.cash = amount;
            _gameState.bank += dep;
          }
          break;
        case 3: {
          int a = std::min(amount, _gameState.bank);
          _gameState.bank -= a;
          _gameState.cash += a;
          break;
        }
        case 4:
          _gameState.cash += _gameState.bank;
          _gameState.bank = 0;
          break;
        case 5:
          if (_gameState.bank > amount) {
            int wit = _gameState.bank - amount;
            _gameState.bank = amount;
            _gameState.cash += wit;
          }
          break;
        case 6:
          _gameState.cash += 2000;
          _gameState.debt += 2000;
          break;
        case 7: {
          int rep = std::min(_gameState.cash, _gameState.debt);
          _gameState.cash -= rep;
          _gameState.debt -= rep;
          break;
        }
      }
    }
  }
}

void TuiApp::showShoppingDialog() {
  struct StoreRow {
    int shop_idx;
    bool is_ammo;
    std::string name;
    std::string type;
    int price;
  };

  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 6, 68, 92);
  int h = std::clamp(rows - 4, 20, 28);
  int y = (rows - h) / 2;
  int x = (cols - w) / 2;

  bool focus_store = true;
  int store_sel = 0;
  int inv_sel = 0;

  while (true) {
    std::vector<StoreRow> store_rows;
    std::vector<StoreRow> inv_rows;

    for (size_t i = 0; i < shop_items.size(); ++i) {
      if (!shop_items[i].name.empty()) {
        std::string type = shop_items[i].armor > 0
                               ? "Armor"
                               : (shop_items[i].hit_chance > 0 ? "Weapon" : "Item");
        store_rows.push_back({static_cast<int>(i), false, shop_items[i].name,
                              type, shop_items[i].weapon_price});
      }
      if (!shop_items[i].ammo_name.empty()) {
        store_rows.push_back({static_cast<int>(i), true, shop_items[i].ammo_name,
                              "Ammo", shop_items[i].ammo_price});
      }
      if (_gameState.weapon_qty[i] > 0) {
        std::string type = shop_items[i].armor > 0
                               ? "Armor"
                               : (shop_items[i].hit_chance > 0 ? "Weapon" : "Item");
        inv_rows.push_back({static_cast<int>(i), false, shop_items[i].name,
                            type, shop_items[i].weapon_price});
      }
      if (_gameState.ammo_qty[i] > 0) {
        inv_rows.push_back({static_cast<int>(i), true, shop_items[i].ammo_name,
                            "Ammo", shop_items[i].ammo_price});
      }
    }

    if (!store_rows.empty()) {
      store_sel = std::clamp(store_sel, 0, static_cast<int>(store_rows.size()) - 1);
    }
    if (!inv_rows.empty()) {
      inv_sel = std::clamp(inv_sel, 0, static_cast<int>(inv_rows.size()) - 1);
    }

    drawDashboard();
    draw_btop_box(y, x, h, w,
                  std::format("🔫 Shopping — Cash: ${}",
                              money_string(_gameState.cash)),
                  "[Tab]Switch Pane [b/Enter]Buy [s]Sell [Esc]Done", true);

    int half_w = (w - 3) / 2;
    draw_btop_box(y + 1, x + 1, h - 2, half_w, "Store's Inventory", "",
                  focus_store);
    draw_btop_box(y + 1, x + 1 + half_w, h - 2, w - 2 - half_w,
                  "Your Inventory", "", !focus_store);

    int max_r = h - 5;
    int s_start = (store_sel >= max_r) ? (store_sel - max_r + 1) : 0;
    for (int r = 0; r < max_r && (s_start + r) < static_cast<int>(store_rows.size());
         ++r) {
      int idx = s_start + r;
      const auto& sr = store_rows[idx];
      bool sel = (idx == store_sel);
      if (sel) attron(COLOR_PAIR(focus_store ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(y + 3 + r, x + 3, "%-16s %-6s $%7s", sr.name.c_str(),
               sr.type.c_str(), money_string(sr.price).c_str());
      if (sel) attroff(COLOR_PAIR(focus_store ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    for (int r = 0; r < max_r && r < static_cast<int>(inv_rows.size()); ++r) {
      const auto& ir = inv_rows[r];
      int qty = ir.is_ammo ? _gameState.ammo_qty[ir.shop_idx]
                           : _gameState.weapon_qty[ir.shop_idx];
      bool sel = (r == inv_sel);
      if (sel) attron(COLOR_PAIR(!focus_store ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(y + 3 + r, x + 3 + half_w, "%-15s x%-3d $%7s", ir.name.c_str(),
               qty, money_string(ir.price).c_str());
      if (sel)
        attroff(COLOR_PAIR(!focus_store ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    refresh();
    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == '\t') {
      focus_store = !focus_store;
    } else if (ch == KEY_UP || ch == 'k') {
      if (focus_store && !store_rows.empty()) {
        store_sel = (store_sel - 1 + static_cast<int>(store_rows.size())) %
                    static_cast<int>(store_rows.size());
      } else if (!focus_store && !inv_rows.empty()) {
        inv_sel = (inv_sel - 1 + static_cast<int>(inv_rows.size())) %
                  static_cast<int>(inv_rows.size());
      }
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (focus_store && !store_rows.empty()) {
        store_sel = (store_sel + 1) % static_cast<int>(store_rows.size());
      } else if (!focus_store && !inv_rows.empty()) {
        inv_sel = (inv_sel + 1) % static_cast<int>(inv_rows.size());
      }
    } else if (ch == 'b' || ch == 'B' ||
               (focus_store && (ch == '\n' || ch == KEY_ENTER))) {
      if (store_rows.empty()) continue;
      const auto& sr = store_rows[store_sel];
      int idx = sr.shop_idx;
      if (sr.is_ammo) {
        int price = shop_items[idx].ammo_price;
        int cur = _gameState.ammo_qty[idx];
        int max_lim = shop_items[idx].ammo_limit;
        int max_buy = std::min(max_lim - cur, _gameState.cash / price);
        if (max_buy <= 0) {
          showMessageModal("Shopping", "You cannot buy any more of that!",
                           CP_RED);
          continue;
        }
        int qty = 1;
        if (max_buy > 1) {
          qty = showInputSpinModal(
              "Buy Ammo",
              std::format("How many {} do you want to buy?",
                          shop_items[idx].ammo_plural),
              "Quantity", 1, max_buy, max_buy, price);
          if (qty <= 0) continue;
        }
        _gameState.cash -= qty * price;
        _gameState.ammo_qty[idx] += qty;
      } else {
        int price = shop_items[idx].weapon_price;
        int cur = _gameState.weapon_qty[idx];
        int max_lim = shop_items[idx].weapon_limit;
        int max_buy = std::min(max_lim - cur, _gameState.cash / price);
        if (max_buy <= 0) {
          showMessageModal("Shopping", "You cannot buy any more of that!",
                           CP_RED);
          continue;
        }
        int qty = 1;
        if (max_buy > 1) {
          qty = showInputSpinModal(
              "Buy Item",
              std::format("How many {} do you want to buy?",
                          shop_items[idx].plural),
              "Quantity", 1, max_buy, max_buy, price);
          if (qty <= 0) continue;
        }
        _gameState.cash -= qty * price;
        _gameState.weapon_qty[idx] += qty;
      }
    } else if (ch == 's' || ch == 'S' ||
               (!focus_store && (ch == '\n' || ch == KEY_ENTER))) {
      if (inv_rows.empty()) continue;
      const auto& ir = inv_rows[inv_sel];
      int idx = ir.shop_idx;
      if (ir.is_ammo) {
        int owned = _gameState.ammo_qty[idx];
        int price = shop_items[idx].ammo_price;
        int qty = 1;
        if (owned > 1) {
          qty = showInputSpinModal(
              "Sell Ammo",
              std::format("How many {} do you want to sell?",
                          shop_items[idx].ammo_plural),
              "Quantity", 1, owned, owned, price);
          if (qty <= 0) continue;
        }
        _gameState.cash += qty * price;
        _gameState.ammo_qty[idx] -= qty;
      } else {
        int owned = _gameState.weapon_qty[idx];
        int price = shop_items[idx].weapon_price;
        int qty = 1;
        if (owned > 1) {
          qty = showInputSpinModal(
              "Sell Item",
              std::format("How many {} do you want to sell?",
                          shop_items[idx].plural),
              "Quantity", 1, owned, owned, price);
          if (qty <= 0) continue;
        }
        _gameState.cash += qty * price;
        _gameState.weapon_qty[idx] -= qty;
      }
    }
  }
}

void TuiApp::showHospitalDialog() {
  int target = _gameState.health;
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = 56, h = 12;
  int y = (rows - h) / 2, x = (cols - w) / 2;

  while (true) {
    int needed = std::max(0, target - _gameState.health);
    int cost = needed * 50;

    drawDashboard();
    draw_btop_box(y, x, h, w, "🏥 Hospital — Medical Treatment",
                  "[←/→]Target HP [a]Full [Enter]Heal [Esc]Cancel", true);

    mvprintw(y + 2, x + 3, "Current Health: %d%%   Desired Health: %d%%",
             _gameState.health, target);
    draw_progress_bar(y + 4, x + 3, w - 6, target / 100.0, CP_GREEN);

    attron(COLOR_PAIR(CP_GREEN) | A_BOLD);
    mvprintw(y + 6, x + 3, "Available Cash:      $%s",
             money_string(_gameState.cash).c_str());
    attroff(COLOR_PAIR(CP_GREEN) | A_BOLD);

    attron(COLOR_PAIR(cost <= _gameState.cash ? CP_YELLOW : CP_RED) | A_BOLD);
    mvprintw(y + 7, x + 3, "Cost for treatment:  $%s ($50 / HP)",
             money_string(cost).c_str());
    attroff(COLOR_PAIR(cost <= _gameState.cash ? CP_YELLOW : CP_RED) | A_BOLD);

    refresh();
    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == KEY_LEFT || ch == 'h' || ch == '-') {
      target = std::max(_gameState.health, target - 1);
    } else if (ch == KEY_RIGHT || ch == 'l' || ch == '+') {
      target = std::min(100, target + 1);
    } else if (ch == 'a' || ch == 'A' || ch == KEY_END) {
      int max_affordable =
          std::min(100, _gameState.health + (_gameState.cash / 50));
      target = max_affordable;
    } else if (ch == '\n' || ch == KEY_ENTER) {
      if (_gameState.cash >= cost) {
        _gameState.cash -= cost;
        _gameState.health = target;
        break;
      } else {
        showMessageModal("Hospital", "Insufficient cash for this treatment!",
                         CP_RED);
      }
    }
  }
}

void TuiApp::showVaultDialog() {
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = std::clamp(cols - 10, 62, 78);
  int h = 18;
  int y = (rows - h) / 2, x = (cols - w) / 2;

  bool focus_pocket = true;
  int p_sel = 0, v_sel = 0;

  while (true) {
    std::vector<int> p_list = ownedPocketDrugs();
    std::vector<int> v_list;
    for (size_t i = 0; i < drug_info.size(); ++i) {
      if (_gameState.vault_qty[i] > 0) v_list.push_back(static_cast<int>(i));
    }

    if (!p_list.empty())
      p_sel = std::clamp(p_sel, 0, static_cast<int>(p_list.size()) - 1);
    if (!v_list.empty())
      v_sel = std::clamp(v_sel, 0, static_cast<int>(v_list.size()) - 1);

    drawDashboard();
    draw_btop_box(y, x, h, w, "🔐 The Vault",
                  "[Tab]Switch [→/Enter]Into Vault [←]From Vault [Esc]Close",
                  true);

    int half_w = (w - 3) / 2;
    draw_btop_box(y + 1, x + 1, h - 2, half_w,
                  std::format("Pocket ({}/{})", _gameState.pocket,
                              _gameState.pocket_capacity),
                  "", focus_pocket);
    draw_btop_box(y + 1, x + 1 + half_w, h - 2, w - 2 - half_w, "In the Vault",
                  "", !focus_pocket);

    for (size_t i = 0; i < p_list.size() && static_cast<int>(i) < h - 5; ++i) {
      int d = p_list[i];
      bool sel = (static_cast<int>(i) == p_sel);
      if (sel)
        attron(COLOR_PAIR(focus_pocket ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(y + 3 + static_cast<int>(i), x + 3, "%-14s %6d",
               drug_name(drug_info[d].id).c_str(), _gameState.player_qty[d]);
      if (sel)
        attroff(COLOR_PAIR(focus_pocket ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    for (size_t i = 0; i < v_list.size() && static_cast<int>(i) < h - 5; ++i) {
      int d = v_list[i];
      bool sel = (static_cast<int>(i) == v_sel);
      if (sel)
        attron(COLOR_PAIR(!focus_pocket ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(y + 3 + static_cast<int>(i), x + 3 + half_w, "%-14s %6d",
               drug_name(drug_info[d].id).c_str(), _gameState.vault_qty[d]);
      if (sel)
        attroff(COLOR_PAIR(!focus_pocket ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    refresh();
    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == '\t') {
      focus_pocket = !focus_pocket;
    } else if (ch == KEY_UP || ch == 'k') {
      if (focus_pocket && !p_list.empty())
        p_sel = (p_sel - 1 + static_cast<int>(p_list.size())) %
                static_cast<int>(p_list.size());
      else if (!focus_pocket && !v_list.empty())
        v_sel = (v_sel - 1 + static_cast<int>(v_list.size())) %
                static_cast<int>(v_list.size());
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (focus_pocket && !p_list.empty())
        p_sel = (p_sel + 1) % static_cast<int>(p_list.size());
      else if (!focus_pocket && !v_list.empty())
        v_sel = (v_sel + 1) % static_cast<int>(v_list.size());
    } else if (ch == KEY_RIGHT ||
               (focus_pocket && (ch == '\n' || ch == KEY_ENTER))) {
      if (!p_list.empty()) {
        int d = p_list[p_sel];
        if (_gameState.player_qty[d] > 0) {
          _gameState.player_qty[d] -= 1;
          _gameState.pocket -= 1;
          _gameState.vault_qty[d] += 1;
        }
      }
    } else if (ch == KEY_LEFT ||
               (!focus_pocket && (ch == '\n' || ch == KEY_ENTER))) {
      if (!v_list.empty()) {
        int d = v_list[v_sel];
        if (_gameState.vault_qty[d] > 0 &&
            _gameState.pocket < _gameState.pocket_capacity) {
          _gameState.vault_qty[d] -= 1;
          _gameState.player_qty[d] += 1;
          _gameState.pocket += 1;
        }
      }
    }
  }
  syncSelectionAndChart();
}

void TuiApp::showVaultsInfoDialog() {
  std::string info = "Vault Contents:\n";
  bool empty = true;
  for (size_t i = 0; i < drug_info.size(); ++i) {
    if (_gameState.vault_qty[i] > 0) {
      empty = false;
      info += std::format("  • {}: {}\n", drug_name(drug_info[i].id),
                          _gameState.vault_qty[i]);
    }
  }
  if (empty) info += "  (Vault is empty)";
  showMessageModal("Vaults Summary", info, CP_CYAN);
}

void TuiApp::showShippingDialog() {
  showMessageModal("Shipping",
                   "Shipping is currently not available in this region.",
                   CP_CYAN);
}

void TuiApp::showShipmentStatusDialog() {
  showMessageModal("Shipment Status", "No active shipments.", CP_CYAN);
}

void TuiApp::showWorldDrugPricesDialog() {
  int sel_drug = 0;
  int sel_city = _gameState.location;
  bool focus_drug = true;

  while (true) {
    int rows = 0, cols = 0;
    getmaxyx(stdscr, rows, cols);
    int top_h = std::max(12, (rows * 58) / 100);
    int bot_h = rows - top_h;
    int left_w = 24;

    erase();
    draw_btop_box(0, 0, top_h, left_w, "Drug", "[Tab]Switch", focus_drug);
    draw_btop_box(0, left_w, top_h, cols - left_w,
                  std::format("City Prices for {}",
                              drug_name(drug_info[sel_drug].id)),
                  "[Esc]Close", !focus_drug);
    draw_btop_box(
        top_h, 0, bot_h, cols,
        std::format("Price History — {} in {}",
                    drug_name(drug_info[sel_drug].id),
                    city_name(city_info[sel_city].id)),
        "Red ◈ = Traded Day", false);

    int max_d = top_h - 2;
    for (int i = 0; i < static_cast<int>(drug_info.size()) && i < max_d; ++i) {
      bool sel = (i == sel_drug);
      if (sel) attron(COLOR_PAIR(focus_drug ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(1 + i, 2, "%-18s", drug_name(drug_info[i].id).c_str());
      if (sel) attroff(COLOR_PAIR(focus_drug ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    int day = _gameState.day;
    for (int c = 0; c < static_cast<int>(city_info.size()) && c < max_d; ++c) {
      bool sel = (c == sel_city);
      const auto& ds = _gameState.drug_table[sel_drug][c][day];
      if (sel) attron(COLOR_PAIR(!focus_drug ? CP_SELECTED : CP_CYAN) | A_BOLD);
      std::string cname = std::format("{}, {}", city_name(city_info[c].id),
                                      country_name(city_info[c].country));
      mvprintw(1 + c, left_w + 2, "%-24s Qty: %6d   Price: $%9s", cname.c_str(),
               ds.qty, money_string(ds.price).c_str());
      if (sel)
        attroff(COLOR_PAIR(!focus_drug ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    renderBrailleChart(top_h + 1, 2, bot_h - 2, cols - 4,
                       ITEM_FIRST_DRUG + sel_drug, sel_city, true);
    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == '\t' || ch == KEY_LEFT || ch == KEY_RIGHT) {
      focus_drug = !focus_drug;
    } else if (ch == KEY_UP || ch == 'k') {
      if (focus_drug)
        sel_drug = (sel_drug - 1 + static_cast<int>(drug_info.size())) %
                   static_cast<int>(drug_info.size());
      else
        sel_city = (sel_city - 1 + static_cast<int>(city_info.size())) %
                   static_cast<int>(city_info.size());
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (focus_drug)
        sel_drug = (sel_drug + 1) % static_cast<int>(drug_info.size());
      else
        sel_city = (sel_city + 1) % static_cast<int>(city_info.size());
    }
  }
}

void TuiApp::showWorldCitiesDialog() {
  int sel_city = _gameState.location;
  int sel_drug = 0;
  bool focus_city = true;

  while (true) {
    int rows = 0, cols = 0;
    getmaxyx(stdscr, rows, cols);
    int top_h = std::max(12, (rows * 58) / 100);
    int bot_h = rows - top_h;
    int left_w = 28;

    erase();
    draw_btop_box(0, 0, top_h, left_w, "City", "[Tab]Switch", focus_city);
    draw_btop_box(0, left_w, top_h, cols - left_w,
                  std::format("Drug List in {}",
                              city_name(city_info[sel_city].id)),
                  "[Esc]Close", !focus_city);
    draw_btop_box(
        top_h, 0, bot_h, cols,
        std::format("Price History — {} in {}",
                    drug_name(drug_info[sel_drug].id),
                    city_name(city_info[sel_city].id)),
        "Red ◈ = Traded Day", false);

    int max_r = top_h - 2;
    for (int c = 0; c < static_cast<int>(city_info.size()) && c < max_r; ++c) {
      bool sel = (c == sel_city);
      if (sel) attron(COLOR_PAIR(focus_city ? CP_SELECTED : CP_CYAN) | A_BOLD);
      std::string cname = std::format("{}, {}", city_name(city_info[c].id),
                                      country_name(city_info[c].country));
      mvprintw(1 + c, 2, "%-24s", cname.c_str());
      if (sel) attroff(COLOR_PAIR(focus_city ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    int day = _gameState.day;
    for (int i = 0; i < static_cast<int>(drug_info.size()) && i < max_r; ++i) {
      bool sel = (i == sel_drug);
      const auto& ds = _gameState.drug_table[i][sel_city][day];
      if (sel) attron(COLOR_PAIR(!focus_city ? CP_SELECTED : CP_CYAN) | A_BOLD);
      mvprintw(1 + i, left_w + 2, "%-16s Qty: %6d   Price: $%9s",
               drug_name(drug_info[i].id).c_str(), ds.qty,
               money_string(ds.price).c_str());
      if (sel)
        attroff(COLOR_PAIR(!focus_city ? CP_SELECTED : CP_CYAN) | A_BOLD);
    }

    renderBrailleChart(top_h + 1, 2, bot_h - 2, cols - 4,
                       ITEM_FIRST_DRUG + sel_drug, sel_city, true);
    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q') break;
    if (ch == '\t' || ch == KEY_LEFT || ch == KEY_RIGHT) {
      focus_city = !focus_city;
    } else if (ch == KEY_UP || ch == 'k') {
      if (focus_city)
        sel_city = (sel_city - 1 + static_cast<int>(city_info.size())) %
                   static_cast<int>(city_info.size());
      else
        sel_drug = (sel_drug - 1 + static_cast<int>(drug_info.size())) %
                   static_cast<int>(drug_info.size());
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (focus_city)
        sel_city = (sel_city + 1) % static_cast<int>(city_info.size());
      else
        sel_drug = (sel_drug + 1) % static_cast<int>(drug_info.size());
    }
  }
}

void TuiApp::showHistoryDialog(int initial_item, int initial_city) {
  int item_idx = std::clamp(initial_item, 0, TOTAL_ITEMS - 1);
  int city_idx =
      std::clamp(initial_city, 0, static_cast<int>(city_info.size()) - 1);

  while (true) {
    int rows = 0, cols = 0;
    getmaxyx(stdscr, rows, cols);
    erase();

    std::string title = std::format(
        "📊 Viewing History — {} ({}, {})", itemName(item_idx),
        city_name(city_info[city_idx].id),
        country_name(city_info[city_idx].country));
    draw_btop_box(0, 0, rows, cols, title,
                  "[↑/↓]Drug/Stat [←/→]City [Esc/q]Close", true);

    renderBrailleChart(2, 2, rows - 5, cols - 4, item_idx, city_idx, true);

    attron(COLOR_PAIR(CP_CYAN));
    mvaddstr(
        rows - 2, 3,
        "Legend: Yellow Braille = Daily Value │ Dim Dotted = Normal Range & "
        "Mean │ Red ◈ = Traded Day");
    attroff(COLOR_PAIR(CP_CYAN));
    refresh();

    int ch = getch();
    if (ch == 27 || ch == 'q' || ch == '\n') break;
    if (ch == KEY_UP || ch == 'k') {
      item_idx = (item_idx - 1 + TOTAL_ITEMS) % TOTAL_ITEMS;
    } else if (ch == KEY_DOWN || ch == 'j') {
      item_idx = (item_idx + 1) % TOTAL_ITEMS;
    } else if (ch == KEY_LEFT || ch == 'h') {
      city_idx = (city_idx - 1 + static_cast<int>(city_info.size())) %
                 static_cast<int>(city_info.size());
    } else if (ch == KEY_RIGHT || ch == 'l') {
      city_idx = (city_idx + 1) % static_cast<int>(city_info.size());
    }
  }
}

void TuiApp::showCombatDialog(int enemy_idx, int enemy_count) {
  std::vector<std::string> log_lines;
  log_lines.push_back(std::format("You face {} {}! Choose your action.",
                                  enemy_count, enemy_info[enemy_idx].name));

  bool combat_done = false;
  static std::random_device rd;
  static std::mt19937 gen(rd());

  auto best_weapon_fn = [&]() -> int {
    for (int i = 8; i >= 0; --i) {
      if (i == 5 || i == 6) {
        if (_gameState.ammo_qty[i] > 0) return i;
      } else if (_gameState.weapon_qty[i] > 0) {
        if (shop_items[i].ammo_limit == 0 || _gameState.ammo_qty[i] > 0)
          return i;
      }
    }
    return -1;
  };

  auto enemy_attack_fn = [&]() {
    if (enemy_count <= 0) return;
    if (enemy_count <= 2 && (gen() % 100 < 25)) {
      log_lines.push_back("They decide to flee!");
      enemy_count = 0;
      combat_done = true;
      return;
    }
    int acc = enemy_info[enemy_idx].accuracy;
    if (static_cast<int>(gen() % 100) < acc) {
      int raw_dmg = 5 + static_cast<int>(gen() % 12);
      int dmg = std::max(1, raw_dmg - _gameState.total_armor());
      _gameState.health -= dmg;
      log_lines.push_back(std::format(
          "They attack you! You have been hit for {} {} of damage!", dmg,
          dmg == 1 ? "point" : "points"));
      if (_gameState.health <= 0) {
        _gameState.health = 0;
        log_lines.push_back("YOU ARE DEAD!");
        combat_done = true;
      }
    } else {
      log_lines.push_back("They attack you but miss!");
    }
  };

  while (true) {
    int rows = 0, cols = 0;
    getmaxyx(stdscr, rows, cols);
    int w = std::clamp(cols - 12, 58, 74);
    int h = 19;
    int y = (rows - h) / 2, x = (cols - w) / 2;

    drawDashboard();
    draw_btop_box(y, x, h, w,
                  std::format("⚔ Fighting {}", enemy_info[enemy_idx].name),
                  combat_done
                      ? "[Enter/Space] Done"
                      : "[f]Fight [r]Flee [b]Bribe [s]Surrender",
                  true, CP_RED);

    int bw = best_weapon_fn();
    std::string w_name = "Fists";
    std::string ammo_str = "N/A";
    if (bw >= 0) {
      w_name = (bw == 5 || bw == 6) ? shop_items[bw].ammo_name
                                    : shop_items[bw].name;
      if (shop_items[bw].ammo_limit > 0) {
        ammo_str = std::format("{}/{}", _gameState.ammo_qty[bw],
                               shop_items[bw].ammo_limit);
      }
    }

    attron(COLOR_PAIR(CP_YELLOW) | A_BOLD);
    mvprintw(y + 2, x + 3, "Enemy: %-24s (%d remain)",
             enemy_info[enemy_idx].name.c_str(), enemy_count);
    attroff(COLOR_PAIR(CP_YELLOW) | A_BOLD);

    mvprintw(y + 3, x + 3, "Health: %3d%%   Cash: $%-12s Armor: %d",
             _gameState.health, money_string(_gameState.cash).c_str(),
             _gameState.total_armor());
    mvprintw(y + 4, x + 3, "Weapon: %-18s Ammo: %s", w_name.c_str(),
             ammo_str.c_str());

    draw_btop_box(y + 6, x + 2, h - 9, w - 4, "Combat Log", "", false);
    int max_log = h - 11;
    int start_l = std::max(0, static_cast<int>(log_lines.size()) - max_log);
    for (int i = 0;
         i < max_log && (start_l + i) < static_cast<int>(log_lines.size());
         ++i) {
      mvprintw(y + 7 + i, x + 4, "%.*s", w - 8, log_lines[start_l + i].c_str());
    }

    if (combat_done) {
      attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
      mvaddstr(y + h - 2, x + (w - 14) / 2, " [ Continue ] ");
      attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    } else {
      mvaddstr(y + h - 2, x + 4,
               "[f] Fight    [r] Flee    [b] Bribe    [s] Surrender");
    }

    refresh();
    int ch = getch();
    if (combat_done) {
      if (ch == '\n' || ch == KEY_ENTER || ch == ' ' || ch == 27 || ch == 'q')
        break;
      continue;
    }

    if (ch == 'f' || ch == 'F' || ch == '\n') {
      if (bw >= 0) {
        if (shop_items[bw].ammo_limit > 0) _gameState.ammo_qty[bw]--;
        if (static_cast<int>(gen() % 100) < shop_items[bw].hit_chance) {
          int killed = std::min(enemy_count, shop_items[bw].damage);
          enemy_count -= killed;
          if (enemy_count <= 0) {
            log_lines.push_back(std::format(
                "You use your {}! You killed the remaining {}! All dead!",
                w_name, killed));
            int bounty =
                (static_cast<int>(gen() % 400) + 100) * (_gameState.rank + 1);
            _gameState.cash += bounty;
            log_lines.push_back(std::format(
                "You search the bodies and find ${}!", money_string(bounty)));
            combat_done = true;
            continue;
          } else {
            log_lines.push_back(
                std::format("You use your {}! Killed {}! {} remain.", w_name,
                            killed, enemy_count));
          }
        } else {
          log_lines.push_back(
              std::format("You use your {}... You miss!", w_name));
        }
      } else {
        log_lines.push_back("You attack with your bare hands... You miss!");
      }
      enemy_attack_fn();
    } else if (ch == 'r' || ch == 'R') {
      int chance = 35 + enemy_info[enemy_idx].flee_chance;
      if (static_cast<int>(gen() % 100) < chance) {
        log_lines.push_back("You manage to lose them in the streets!");
        combat_done = true;
      } else {
        log_lines.push_back("You are unable to shake them.");
        enemy_attack_fn();
      }
    } else if (ch == 'b' || ch == 'B') {
      if (!enemy_info[enemy_idx].can_bribe || _gameState.cash <= 0) {
        log_lines.push_back("They cannot be bribed!");
        continue;
      }
      int offer = showInputSpinModal(
          "Bribe",
          std::format("You have ${} to bribe them with.",
                      money_string(_gameState.cash)),
          "Bribe Offer ($)", 1, _gameState.cash,
          std::min(_gameState.cash, 500));
      if (offer <= 0) continue;
      int needed = (enemy_info[enemy_idx].scale * 80) + (enemy_count * 30);
      if (offer >= needed || (gen() % 100 < 50)) {
        _gameState.cash -= offer;
        log_lines.push_back("They accept your bribe!");
        combat_done = true;
      } else {
        log_lines.push_back("They laugh at your attempt!");
        enemy_attack_fn();
      }
    } else if (ch == 's' || ch == 'S') {
      if (enemy_info[enemy_idx].surrender_accept <= 0) {
        log_lines.push_back("They do not accept surrender!");
        continue;
      }
      if (static_cast<int>(gen() % 100) <
          enemy_info[enemy_idx].surrender_accept) {
        bool is_cops = (enemy_idx <= 1 || enemy_idx == 4 || enemy_idx == 8 ||
                        enemy_idx == 9);
        if (is_cops) {
          if (_gameState.total_drugs() > 0) {
            _gameState.player_qty.fill(0);
            _gameState.pocket = 0;
            log_lines.push_back(
                "Surrender accepted. Cops seize all your drugs and release "
                "you.");
          } else {
            log_lines.push_back(
                "Surrender accepted. You are clean! A brief apology is "
                "mumbled.");
          }
        } else {
          _gameState.cash = 0;
          _gameState.player_qty.fill(0);
          _gameState.pocket = 0;
          log_lines.push_back("Surrender accepted, but they take everything!");
        }
        combat_done = true;
      } else {
        log_lines.push_back("Your surrender is not accepted!");
        enemy_attack_fn();
      }
    }
  }
}

void TuiApp::showPlacesMenu() {
  constexpr std::array items = {"1. Finances (Bank & Loans)  [f]",
                                "2. Shopping (Weapons/Items) [p]",
                                "3. Hospital (Heal HP)       [h]",
                                "4. The Vault (Store Drugs)  [v]",
                                "5. Shipping                 "};
  int sel = 0;
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = 42, h = 10;
  int y = (rows - h) / 2, x = (cols - w) / 2;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, "Places...", "[Enter]Open [Esc]Close", true);
    for (size_t i = 0; i < items.size(); ++i) {
      if (static_cast<int>(i) == sel) attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
      mvprintw(y + 2 + static_cast<int>(i), x + 3, " %-34s ", items[i]);
      if (static_cast<int>(i) == sel) attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    }
    refresh();
    int ch = getch();
    if (ch == 27 || ch == 'q') return;
    if (ch == KEY_UP || ch == 'k')
      sel = (sel - 1 + static_cast<int>(items.size())) %
            static_cast<int>(items.size());
    else if (ch == KEY_DOWN || ch == 'j')
      sel = (sel + 1) % static_cast<int>(items.size());
    else if (ch == '\n' || ch == KEY_ENTER) {
      if (sel == 0)
        showFinancesDialog();
      else if (sel == 1)
        showShoppingDialog();
      else if (sel == 2)
        showHospitalDialog();
      else if (sel == 3)
        showVaultDialog();
      else if (sel == 4)
        showShippingDialog();
      return;
    }
  }
}

void TuiApp::showInfoMenu() {
  constexpr std::array items = {"1. Vaults Summary      [V]",
                                "2. World Drug Prices   [w]",
                                "3. World Cities        [l]",
                                "4. Shipment Status     ",
                                "5. Viewing History     [g]"};
  int sel = 0;
  int rows = 0, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int w = 40, h = 10;
  int y = (rows - h) / 2, x = (cols - w) / 2;

  while (true) {
    drawDashboard();
    draw_btop_box(y, x, h, w, "Info...", "[Enter]Open [Esc]Close", true);
    for (size_t i = 0; i < items.size(); ++i) {
      if (static_cast<int>(i) == sel) attron(COLOR_PAIR(CP_SELECTED) | A_BOLD);
      mvprintw(y + 2 + static_cast<int>(i), x + 3, " %-32s ", items[i]);
      if (static_cast<int>(i) == sel) attroff(COLOR_PAIR(CP_SELECTED) | A_BOLD);
    }
    refresh();
    int ch = getch();
    if (ch == 27 || ch == 'q') return;
    if (ch == KEY_UP || ch == 'k')
      sel = (sel - 1 + static_cast<int>(items.size())) %
            static_cast<int>(items.size());
    else if (ch == KEY_DOWN || ch == 'j')
      sel = (sel + 1) % static_cast<int>(items.size());
    else if (ch == '\n' || ch == KEY_ENTER) {
      if (sel == 0)
        showVaultsInfoDialog();
      else if (sel == 1)
        showWorldDrugPricesDialog();
      else if (sel == 2)
        showWorldCitiesDialog();
      else if (sel == 3)
        showShipmentStatusDialog();
      else if (sel == 4)
        showHistoryDialog(_chartItemIdx, _gameState.location);
      return;
    }
  }
}

void TuiApp::showAboutDialog() {
  std::string info = std::format(
      "{}\n{}\n\nAuthor: {}\nVersion: {}\nUI: btop-inspired ncurses TUI",
      kProgramName, kProgramDescription, kProgramAuthorName, kProgramVersion);
  showMessageModal("About Drux Lord", info, CP_CYAN);
}

void TuiApp::showDocsDialog() {
  showMessageModal(
      "Documentation",
      "Welcome to Drux Lord!\n\n"
      "You assume the role of a drug dealer starting with $2,000 in your "
      "pocket and a $1,000 loan from Buddles.\n"
      "It is your goal to turn that investment into as much wealth as you "
      "can in 30 days.\n\n"
      "Buy drugs from the market, sell them for a profit, deposit money in "
      "the bank to earn interest, and repay your loan shark before the "
      "interest gets out of hand!",
      CP_CYAN);
}

void TuiApp::showHighscoresDialog() {
  showMessageModal("High Scores",
                   "High Scores:\n"
                   "1. Drug Lord      - $10,000,000\n"
                   "2. Master Dealer  - $5,000,000\n"
                   "3. Distributor    - $1,000,000\n"
                   "4. Dealer         - $100,000\n"
                   "5. Wannabe        - $2,000",
                   CP_YELLOW);
}

void TuiApp::showHelpDialog() {
  showMessageModal(
      "Keyboard Shortcuts (btop style)",
      "Navigation:  [Tab / 1 / 2] Switch Market / Pocket pane\n"
      "             [Up/Down / j/k] Move selection cursor\n"
      "Trading:     [b] Buy drug   [s] Sell drug   [d] Dump drug\n"
      "Tomorrow:    [Space] Stay Here (advance 1 day)  [t] Fly Away\n"
      "Places:      [f] Finances   [p] Shopping    [h] Hospital\n"
      "             [v] The Vault  [P] Places menu\n"
      "Telemetry:   [c] Cycle Graph Stat/Drug  [g/z] Fullscreen Graph\n"
      "             [w] World Drug Prices      [l] World Cities\n"
      "             [V] Vault Summary          [I] Info menu\n"
      "System:      [m] Toggle Sound  [o] Docs  [a] About  [H] High Scores\n"
      "             [n] New Game      [q] Quit",
      CP_CYAN);
}

void TuiApp::actionNewGame() {
  if (showConfirmModal("New Game",
                       "Start a new 30-day game and reset current progress?")) {
    _gameState.newgame();
    syncSelectionAndChart();
  }
}
