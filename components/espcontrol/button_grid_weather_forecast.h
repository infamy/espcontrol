#pragma once

// Internal implementation detail for button_grid.h. Include button_grid.h from device YAML.

inline std::string normalize_weather_state(std::string state) {
  state = trim_display_unit(state);
  std::string normalized;
  normalized.reserve(state.size());
  bool last_dash = false;
  for (char ch : state) {
    unsigned char uch = static_cast<unsigned char>(ch);
    if (std::isalnum(uch)) {
      normalized.push_back(static_cast<char>(std::tolower(uch)));
      last_dash = false;
    } else if (!last_dash) {
      normalized.push_back('-');
      last_dash = true;
    }
  }
  while (!normalized.empty() && normalized.back() == '-') normalized.pop_back();
  if (normalized.compare(0, 4, "mdi-") == 0) normalized = normalized.substr(4);
  if (normalized.compare(0, 8, "weather-") == 0) normalized = normalized.substr(8);
  if (normalized == "clear" || normalized == "mostly-clear" || normalized == "mostly-sunny") return "sunny";
  if (normalized == "clear-day") return "sunny";
  if (normalized == "overcast" || normalized == "broken-clouds" ||
      normalized == "mostly-cloudy" || normalized == "scattered-clouds") return "cloudy";
  if (normalized == "foggy") return "fog";
  if (normalized == "night") return "clear-night";
  if (normalized == "mostly-clear-night" || normalized == "night-clear") return "clear-night";
  if (normalized == "partly-cloudy") return "partlycloudy";
  if (normalized == "partly-sunny" || normalized == "few-clouds") return "partlycloudy";
  if (normalized == "partly-cloudy-day") return "partlycloudy";
  if (normalized == "partly-cloudy-night" || normalized == "mostly-cloudy-night" ||
      normalized == "cloudy-night" || normalized == "few-clouds-night" ||
      normalized == "night-cloudy") return "night-partly-cloudy";
  if (normalized == "drizzle" || normalized == "light-rain" ||
      normalized == "rain" || normalized == "showers") return "rainy";
  if (normalized == "heavy-rain" || normalized == "heavy-showers") return "pouring";
  if (normalized == "possibly-rainy-day" || normalized == "possibly-rainy-night") return "rainy";
  if (normalized == "possibly-sleet-day" || normalized == "possibly-sleet-night") return "snowy-rainy";
  if (normalized == "possibly-snow-day" || normalized == "possibly-snow-night") return "snowy";
  if (normalized == "possibly-thunderstorm-day" || normalized == "possibly-thunderstorm-night") return "lightning-rainy";
  if (normalized == "freezing-rain") return "snowy-rainy";
  if (normalized == "blizzard" || normalized == "heavy-snow") return "snowy-heavy";
  if (normalized == "sleet") return "snowy-rainy";
  if (normalized == "snow") return "snowy";
  if (normalized == "storm" || normalized == "stormy" ||
      normalized == "thunderstorm" || normalized == "thunderstorms") return "lightning";
  if (normalized == "sunny-off" || normalized == "unknown") return "unavailable";
  return normalized;
}

inline const char* weather_icon_for_state(const std::string &state) {
  std::string normalized = normalize_weather_state(state);
  if (normalized == "sunny") return find_icon("Weather Sunny");
  if (normalized == "clear-night") return find_icon("Weather Night");
  if (normalized == "partlycloudy") return find_icon("Weather Partly Cloudy");
  if (normalized == "cloudy") return find_icon("Weather Cloudy");
  if (normalized == "cloudy-alert") return find_icon("Weather Cloudy Alert");
  if (normalized == "dust") return find_icon("Weather Dust");
  if (normalized == "fog") return find_icon("Weather Fog");
  if (normalized == "hail") return find_icon("Weather Hail");
  if (normalized == "hazy") return find_icon("Weather Hazy");
  if (normalized == "hurricane") return find_icon("Weather Hurricane");
  if (normalized == "lightning") return find_icon("Weather Lightning");
  if (normalized == "lightning-rainy") return find_icon("Weather Lightning Rainy");
  if (normalized == "night-partly-cloudy") return find_icon("Weather Night Cloudy");
  if (normalized == "partly-lightning") return find_icon("Weather Partly Lightning");
  if (normalized == "partly-rainy") return find_icon("Weather Partly Rainy");
  if (normalized == "partly-snowy") return find_icon("Weather Partly Snowy");
  if (normalized == "partly-snowy-rainy") return find_icon("Weather Partly Snowy Rainy");
  if (normalized == "pouring") return find_icon("Weather Pouring");
  if (normalized == "rainy") return find_icon("Weather Rainy");
  if (normalized == "snowy") return find_icon("Weather Snowy");
  if (normalized == "snowy-heavy") return find_icon("Weather Snowy Heavy");
  if (normalized == "snowy-rainy") return find_icon("Weather Snowy Rainy");
  if (normalized == "sunny-alert") return find_icon("Weather Sunny Alert");
  if (normalized == "sunset") return find_icon("Weather Sunset");
  if (normalized == "sunset-down") return find_icon("Weather Sunset Down");
  if (normalized == "sunset-up") return find_icon("Weather Sunset Up");
  if (normalized == "tornado") return find_icon("Weather Tornado");
  if (normalized == "windy") return find_icon("Weather Windy");
  if (normalized == "windy-variant") return find_icon("Weather Windy Variant");
  if (normalized == "unavailable" || normalized.empty()) return find_icon("Weather Sunny Off");
  return find_icon("Weather Cloudy Alert");
}

inline bool weather_state_has_localized_label(const std::string &state) {
  std::string value = trim_display_unit(state);
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  const std::string normalized = normalize_weather_state(state);
  return value == normalized &&
         (normalized == "sunny" || normalized == "clear-night" ||
          normalized == "partlycloudy" || normalized == "cloudy" ||
          normalized == "cloudy-alert" || normalized == "dust" ||
          normalized == "fog" || normalized == "hail" || normalized == "hazy" ||
          normalized == "hurricane" || normalized == "lightning" ||
          normalized == "lightning-rainy" || normalized == "night-partly-cloudy" ||
          normalized == "partly-lightning" || normalized == "partly-rainy" ||
          normalized == "partly-snowy" || normalized == "partly-snowy-rainy" ||
          normalized == "pouring" || normalized == "rainy" || normalized == "snowy" ||
          normalized == "snowy-heavy" || normalized == "snowy-rainy" ||
          normalized == "sunny-alert" || normalized == "sunset" ||
          normalized == "sunset-down" || normalized == "sunset-up" ||
          normalized == "tornado" || normalized == "windy" ||
          normalized == "windy-variant" || normalized == "exceptional");
}

inline std::string weather_label_for_state(const std::string &state) {
  std::string normalized = normalize_weather_state(state);
  std::string value = trim_display_unit(state);
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  if (value == "unknown") return espcontrol_i18n(std::string("Unknown"));
  if (value == "unavailable" || value.empty()) {
    return espcontrol_i18n(std::string("Unavailable"));
  }
  if (!weather_state_has_localized_label(state)) {
    return sentence_cap_text(trim_display_unit(state));
  }
  if (normalized == "sunny") return espcontrol_i18n(std::string("Sunny"));
  if (normalized == "clear-night") return espcontrol_i18n(std::string("Clear Night"));
  if (normalized == "partlycloudy") return espcontrol_i18n(std::string("Partly Cloudy"));
  if (normalized == "cloudy") return espcontrol_i18n(std::string("Cloudy"));
  if (normalized == "cloudy-alert") return espcontrol_i18n(std::string("Cloudy Alert"));
  if (normalized == "dust") return espcontrol_i18n(std::string("Dust"));
  if (normalized == "fog") return espcontrol_i18n(std::string("Fog"));
  if (normalized == "hail") return espcontrol_i18n(std::string("Hail"));
  if (normalized == "hazy") return espcontrol_i18n(std::string("Hazy"));
  if (normalized == "hurricane") return espcontrol_i18n(std::string("Hurricane"));
  if (normalized == "lightning") return espcontrol_i18n(std::string("Lightning"));
  if (normalized == "lightning-rainy") return espcontrol_i18n(std::string("Lightning And Rain"));
  if (normalized == "night-partly-cloudy") return espcontrol_i18n(std::string("Partly Cloudy Night"));
  if (normalized == "partly-lightning") return espcontrol_i18n(std::string("Partly Lightning"));
  if (normalized == "partly-rainy") return espcontrol_i18n(std::string("Partly Rainy"));
  if (normalized == "partly-snowy") return espcontrol_i18n(std::string("Partly Snowy"));
  if (normalized == "partly-snowy-rainy") return espcontrol_i18n(std::string("Partly Snow And Rain"));
  if (normalized == "pouring") return espcontrol_i18n(std::string("Pouring"));
  if (normalized == "rainy") return espcontrol_i18n(std::string("Rainy"));
  if (normalized == "snowy") return espcontrol_i18n(std::string("Snowy"));
  if (normalized == "snowy-heavy") return espcontrol_i18n(std::string("Heavy Snow"));
  if (normalized == "snowy-rainy") return espcontrol_i18n(std::string("Snowy And Rain"));
  if (normalized == "sunny-alert") return espcontrol_i18n(std::string("Sunny Alert"));
  if (normalized == "sunset") return espcontrol_i18n(std::string("Sunset"));
  if (normalized == "sunset-down") return espcontrol_i18n(std::string("Sunset Down"));
  if (normalized == "sunset-up") return espcontrol_i18n(std::string("Sunset Up"));
  if (normalized == "tornado") return espcontrol_i18n(std::string("Tornado"));
  if (normalized == "windy") return espcontrol_i18n(std::string("Windy"));
  if (normalized == "windy-variant") return espcontrol_i18n(std::string("Windy And Cloudy"));
  if (normalized == "exceptional") return espcontrol_i18n(std::string("Exceptional"));
  if (normalized == "unknown") return espcontrol_i18n(std::string("Unknown"));
  if (normalized == "unavailable" || normalized.empty()) return espcontrol_i18n(std::string("Unavailable"));

  return sentence_cap_text(state);
}

// Fonts for the Daily Forecast layout, taken from the card's own labels so the
// strip matches each display's card typography.
struct WeatherDaysCardFonts {
  const lv_font_t *icon = nullptr;
  const lv_font_t *value = nullptr;
  const lv_font_t *text = nullptr;
};

#if defined(ESPCONTROL_DISABLE_WEATHER_FORECAST) && ESPCONTROL_DISABLE_WEATHER_FORECAST

inline void reset_weather_forecast_cards() {}

inline bool register_weather_days_card(lv_obj_t *, const WeatherDaysCardFonts &,
                                       const std::string &) {
  return false;
}

inline void weather_days_card_set_columns(lv_obj_t *, int) {}

inline void weather_days_set_current_state(lv_obj_t *, const std::string &) {}

inline void weather_days_set_current_temperature(lv_obj_t *, const std::string &) {}

inline void weather_days_set_current_unit(lv_obj_t *, const std::string &) {}

inline bool weather_days_card_registered(lv_obj_t *) { return false; }

inline void refresh_weather_forecast_card_visuals() {}

inline void register_weather_forecast_card(lv_obj_t *btn,
                                           lv_obj_t *value_lbl, lv_obj_t *unit_lbl,
                                           lv_obj_t *label_lbl,
                                           const std::string &entity_id,
                                           const std::string &day,
                                           const std::string &label) {
  (void) btn;
  (void) value_lbl;
  (void) unit_lbl;
  (void) label_lbl;
  (void) entity_id;
  (void) day;
  (void) label;
}

inline void weather_forecast_cancel_pending_requests() {}

inline bool weather_forecast_cancel_stale_requests() { return false; }

inline void weather_forecast_send_next_queued() {}

inline void refresh_weather_forecast_cards() {}

#else

struct WeatherForecastCardRef {
  lv_obj_t *btn;
  lv_obj_t *value_lbl;
  lv_obj_t *unit_lbl;
  lv_obj_t *label_lbl;
  std::string entity_id;
  std::string day;
  std::string label;
  std::string status_label;
  bool valid = false;
  float high = 0.0f;
  float low = 0.0f;
  std::string source_unit;
};

inline WeatherForecastCardRef *weather_forecast_card_refs() {
  static WeatherForecastCardRef refs[MAX_GRID_SLOTS + MAX_SUBPAGE_ITEMS];
  return refs;
}

inline int &weather_forecast_card_count() {
  static int count = 0;
  return count;
}

// Daily Forecast cards hold more state, so only a few are supported at once.
constexpr int WEATHER_DAYS_CARDS_MAX = 4;

struct WeatherDaysCardRef {
  lv_obj_t *btn = nullptr;
  lv_obj_t *root = nullptr;
  lv_obj_t *current_icon = nullptr;
  lv_obj_t *current_temp = nullptr;
  lv_obj_t *current_condition = nullptr;
  lv_obj_t *today_range = nullptr;
  lv_obj_t *days_row = nullptr;
  lv_obj_t *columns[espcontrol::WEATHER_DAYS_MAX] = {};
  lv_obj_t *day_lbls[espcontrol::WEATHER_DAYS_MAX] = {};
  lv_obj_t *icon_lbls[espcontrol::WEATHER_DAYS_MAX] = {};
  lv_obj_t *range_lbls[espcontrol::WEATHER_DAYS_MAX] = {};
  int visible_days = 0;
  std::string entity_id;
  bool valid = false;
  espcontrol::WeatherDaysForecast forecast;
  std::string current_temp_text;
  std::string current_temp_unit;
};

inline WeatherDaysCardRef *weather_days_card_refs() {
  static WeatherDaysCardRef refs[WEATHER_DAYS_CARDS_MAX];
  return refs;
}

inline int &weather_days_card_count() {
  static int count = 0;
  return count;
}

inline void reset_weather_forecast_cards() {
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  for (int i = 0; i < MAX_GRID_SLOTS + MAX_SUBPAGE_ITEMS; i++) {
    refs[i] = WeatherForecastCardRef();
  }
  weather_forecast_card_count() = 0;
  WeatherDaysCardRef *days = weather_days_card_refs();
  for (int i = 0; i < WEATHER_DAYS_CARDS_MAX; i++) {
    days[i] = WeatherDaysCardRef();
  }
  weather_days_card_count() = 0;
}

constexpr float WEATHER_FORECAST_TEMP_MISSING = 32767.0f;
constexpr int WEATHER_FORECAST_PENDING_MAX = 8;
constexpr uint32_t WEATHER_FORECAST_REQUEST_TIMEOUT_MS = 60000;
constexpr uint32_t WEATHER_FORECAST_RETRY_DELAY_MS = 300000;

struct WeatherForecastPendingRequest {
  uint32_t call_id = 0;
  uint32_t started_ms = 0;
  std::string entity_id;
  std::string day;
};

struct WeatherForecastQueuedRequest {
  std::string entity_id;
  std::string day;
};

struct WeatherForecastRetryRequest {
  std::string entity_id;
  std::string day;
  uint32_t due_ms = 0;
};

inline std::string weather_forecast_unit_symbol(const std::string &unit) {
  (void)unit;
  return display_temperature_unit_symbol();
}

inline int weather_forecast_display_temp(float value, const std::string &unit) {
  if (value == WEATHER_FORECAST_TEMP_MISSING) return value;
  float converted = convert_temperature_value_for_display_float(value, unit);
  return static_cast<int>(converted >= 0.0f ? converted + 0.5f : converted - 0.5f);
}

inline void apply_weather_forecast_card_text(const WeatherForecastCardRef &ref,
                                             bool valid, float high, float low,
                                             const std::string &unit) {
  if (ref.label_lbl) {
    std::string label = !ref.status_label.empty()
      ? ref.status_label
      : (ref.label.empty()
          ? (ref.day == "today" ? espcontrol_i18n(std::string("Today")) : espcontrol_i18n(std::string("Tomorrow")))
          : ref.label);
    lv_label_set_display_text(ref.label_lbl, label.c_str());
  }
  if (!ref.value_lbl || !ref.unit_lbl) return;
  if (!valid) {
    lv_label_set_display_text(ref.value_lbl, "--/--");
    std::string normalized_unit = weather_forecast_unit_symbol(unit);
    lv_label_set_display_text(ref.unit_lbl, normalized_unit.c_str());
    return;
  }
  char buf[24];
  char high_buf[12];
  char low_buf[12];
  if (high == WEATHER_FORECAST_TEMP_MISSING) snprintf(high_buf, sizeof(high_buf), "--");
  else snprintf(high_buf, sizeof(high_buf), "%d", weather_forecast_display_temp(high, unit));
  if (low == WEATHER_FORECAST_TEMP_MISSING) snprintf(low_buf, sizeof(low_buf), "--");
  else snprintf(low_buf, sizeof(low_buf), "%d", weather_forecast_display_temp(low, unit));
  snprintf(buf, sizeof(buf), "%s/%s", high_buf, low_buf);
  lv_label_set_display_text(ref.value_lbl, buf);
  std::string normalized_unit = weather_forecast_unit_symbol(unit);
  lv_label_set_display_text(ref.unit_lbl, normalized_unit.c_str());
}

inline bool weather_forecast_card_ref_ready(const WeatherForecastCardRef &ref) {
  if (!esphome::App.is_setup_complete()) return false;
  if (!lv_display_get_default()) return false;
  if (!ref.btn || !ref.value_lbl || !ref.unit_lbl) return false;
  if (!lv_obj_is_valid(ref.btn)) return false;
  if (!lv_obj_is_valid(ref.value_lbl)) return false;
  if (!lv_obj_is_valid(ref.unit_lbl)) return false;
  if (ref.label_lbl && !lv_obj_is_valid(ref.label_lbl)) return false;
  return true;
}

// ── Daily Forecast cards ─────────────────────────────────────────────

inline std::string weather_days_temp_text(float value, const std::string &unit) {
  if (value == espcontrol::WEATHER_DAYS_TEMP_MISSING) return "--";
  return std::to_string(weather_forecast_display_temp(value, unit)) + "°";
}

inline std::string weather_days_range_text(float high, float low, const std::string &unit) {
  const bool has_high = high != espcontrol::WEATHER_DAYS_TEMP_MISSING;
  const bool has_low = low != espcontrol::WEATHER_DAYS_TEMP_MISSING;
  if (!has_high && !has_low) return "--/--";
  if (!has_low) return weather_days_temp_text(high, unit);
  if (!has_high) return weather_days_temp_text(low, unit);
  return weather_days_temp_text(high, unit) + "/" + weather_days_temp_text(low, unit);
}

inline bool weather_days_card_ref_ready(const WeatherDaysCardRef &ref) {
  if (!esphome::App.is_setup_complete()) return false;
  if (!lv_display_get_default()) return false;
  return ref.btn && ref.root && lv_obj_is_valid(ref.btn) && lv_obj_is_valid(ref.root);
}

inline WeatherDaysCardRef *weather_days_card_for(lv_obj_t *btn) {
  WeatherDaysCardRef *refs = weather_days_card_refs();
  for (int i = 0; i < weather_days_card_count(); i++) {
    if (refs[i].btn == btn) return &refs[i];
  }
  return nullptr;
}

inline bool weather_days_card_registered(lv_obj_t *btn) { return weather_days_card_for(btn) != nullptr; }

inline void apply_weather_days_current_temperature(WeatherDaysCardRef &ref) {
  float value = 0.0f;
  const std::string &unit = ref.current_temp_unit.empty() ? ref.forecast.unit : ref.current_temp_unit;
  const std::string text = espcontrol::weather_days_parse_temp(ref.current_temp_text, value)
    ? weather_days_temp_text(value, unit)
    : std::string("--");
  lv_label_set_display_text(ref.current_temp, text.c_str());
}

inline void apply_weather_days_card_text(WeatherDaysCardRef &ref) {
  const espcontrol::WeatherDaysForecast &forecast = ref.forecast;
  const std::string today = ref.valid
    ? weather_days_range_text(forecast.today_high, forecast.today_low, forecast.unit)
    : std::string("--/--");
  lv_label_set_display_text(ref.today_range, today.c_str());
  for (int i = 0; i < espcontrol::WEATHER_DAYS_MAX; i++) {
    const bool has_day = ref.valid && i < forecast.day_count;
    const espcontrol::WeatherDay &day = forecast.days[i];
    const std::string name = has_day
      ? espcontrol_i18n(std::string(espcontrol::weather_weekday_short_name(day.weekday)))
      : std::string("--");
    const std::string range = has_day
      ? weather_days_range_text(day.high, day.low, forecast.unit)
      : std::string("--/--");
    lv_label_set_display_text(ref.day_lbls[i], name.c_str());
    lv_label_set_display_text(ref.icon_lbls[i],
      has_day ? weather_icon_for_state(day.condition) : find_icon("Weather Sunny Off"));
    lv_label_set_display_text(ref.range_lbls[i], range.c_str());
  }
  apply_weather_days_current_temperature(ref);
}

inline bool refresh_weather_days_card_visuals() {
  WeatherDaysCardRef *refs = weather_days_card_refs();
  bool updated = false;
  for (int i = 0; i < weather_days_card_count(); i++) {
    if (!weather_days_card_ref_ready(refs[i])) continue;
    apply_weather_days_card_text(refs[i]);
    updated = true;
  }
  return updated;
}

inline lv_obj_t *weather_days_box(lv_obj_t *parent, lv_flex_flow_t flow) {
  lv_obj_t *box = lv_obj_create(parent);
  lv_obj_set_size(box, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(box, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(box, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box, 0, LV_PART_MAIN);
  lv_obj_clear_flag(box, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(box, flow);
  lv_obj_set_flex_align(box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  return box;
}

inline lv_obj_t *weather_days_label(lv_obj_t *parent, const lv_font_t *font) {
  lv_obj_t *label = lv_label_create(parent);
  if (font) lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_label_set_display_text(label, "");
  return label;
}

// Builds the strip inside the card: current conditions on the left, then one
// column per forecast day. Columns are shown by weather_days_card_set_columns.
inline bool register_weather_days_card(lv_obj_t *btn, const WeatherDaysCardFonts &fonts,
                                       const std::string &entity_id) {
  int &count = weather_days_card_count();
  if (count >= WEATHER_DAYS_CARDS_MAX) {
    ESP_LOGW("weather_forecast", "Too many Daily Forecast cards; showing current weather only");
    return false;
  }
  WeatherDaysCardRef &ref = weather_days_card_refs()[count++];
  ref = WeatherDaysCardRef();
  ref.btn = btn;
  ref.entity_id = entity_id;

  ref.root = weather_days_box(btn, LV_FLEX_FLOW_ROW);
  lv_obj_set_size(ref.root, lv_pct(100), lv_pct(100));
  lv_obj_center(ref.root);
  lv_obj_set_style_pad_column(ref.root, 12, LV_PART_MAIN);

  lv_obj_t *current = weather_days_box(ref.root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(current, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_t *now_row = weather_days_box(current, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_column(now_row, 8, LV_PART_MAIN);
  ref.current_icon = weather_days_label(now_row, fonts.icon);
  ref.current_temp = weather_days_label(now_row, fonts.value);
  ref.current_condition = weather_days_label(current, fonts.text);
  ref.today_range = weather_days_label(current, fonts.text);
  lv_obj_set_style_text_opa(ref.today_range, LV_OPA_70, LV_PART_MAIN);
  lv_label_set_display_text(ref.current_icon, find_icon("Weather Cloudy"));

  ref.days_row = weather_days_box(ref.root, LV_FLEX_FLOW_ROW);
  lv_obj_set_height(ref.days_row, lv_pct(100));
  lv_obj_set_flex_grow(ref.days_row, 1);
  lv_obj_set_flex_align(ref.days_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  for (int i = 0; i < espcontrol::WEATHER_DAYS_MAX; i++) {
    ref.columns[i] = weather_days_box(ref.days_row, LV_FLEX_FLOW_COLUMN);
    ref.day_lbls[i] = weather_days_label(ref.columns[i], fonts.text);
    lv_obj_set_style_text_opa(ref.day_lbls[i], LV_OPA_80, LV_PART_MAIN);
    ref.icon_lbls[i] = weather_days_label(ref.columns[i], fonts.icon);
    ref.range_lbls[i] = weather_days_label(ref.columns[i], fonts.text);
    lv_obj_add_flag(ref.columns[i], LV_OBJ_FLAG_HIDDEN);
  }
  apply_weather_days_card_text(ref);
  return true;
}

inline void weather_days_card_set_columns(lv_obj_t *btn, int col_span) {
  WeatherDaysCardRef *ref = weather_days_card_for(btn);
  // Layout runs while the grid is built, which can be before setup completes.
  if (!ref || !ref->root || !lv_obj_is_valid(ref->root)) return;
  ref->visible_days = espcontrol::weather_days_visible_for_columns(col_span);
  for (int i = 0; i < espcontrol::WEATHER_DAYS_MAX; i++) {
    if (i < ref->visible_days) lv_obj_clear_flag(ref->columns[i], LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(ref->columns[i], LV_OBJ_FLAG_HIDDEN);
  }
  // A single-column card shows only the current conditions, centred.
  if (ref->visible_days == 0) lv_obj_add_flag(ref->days_row, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_clear_flag(ref->days_row, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_flex_align(ref->root,
                        ref->visible_days == 0 ? LV_FLEX_ALIGN_CENTER : LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
}

inline void weather_days_set_current_state(lv_obj_t *btn, const std::string &state) {
  WeatherDaysCardRef *ref = weather_days_card_for(btn);
  if (!ref || !weather_days_card_ref_ready(*ref)) return;
  lv_label_set_display_text(ref->current_icon, weather_icon_for_state(state));
  lv_label_set_display_text(ref->current_condition, weather_label_for_state(state).c_str());
  notify_dashboard_content_changed();
}

inline void weather_days_set_current_temperature(lv_obj_t *btn, const std::string &value) {
  WeatherDaysCardRef *ref = weather_days_card_for(btn);
  if (!ref || !weather_days_card_ref_ready(*ref)) return;
  ref->current_temp_text = value;
  apply_weather_days_current_temperature(*ref);
  notify_dashboard_content_changed();
}

inline void weather_days_set_current_unit(lv_obj_t *btn, const std::string &unit) {
  WeatherDaysCardRef *ref = weather_days_card_for(btn);
  if (!ref || !weather_days_card_ref_ready(*ref)) return;
  ref->current_temp_unit = unit;
  apply_weather_days_current_temperature(*ref);
  notify_dashboard_content_changed();
}

inline void refresh_weather_forecast_card_visuals() {
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  bool updated = refresh_weather_days_card_visuals();
  for (int i = 0; i < count; i++) {
    if (!weather_forecast_card_ref_ready(refs[i])) continue;
    apply_weather_forecast_card_text(refs[i], refs[i].valid, refs[i].high,
                                     refs[i].low, refs[i].source_unit);
    updated = true;
  }
  if (updated) notify_dashboard_content_changed();
}

inline lv_timer_t *&weather_forecast_visual_refresh_timer() {
  static lv_timer_t *timer = nullptr;
  return timer;
}

inline void weather_forecast_apply_visuals_cb(lv_timer_t *timer) {
  lv_timer_t *&active_timer = weather_forecast_visual_refresh_timer();
  if (active_timer == timer) active_timer = nullptr;
  lv_timer_del(timer);
  refresh_weather_forecast_card_visuals();
}

inline void weather_forecast_schedule_visual_refresh() {
  lv_timer_t *&timer = weather_forecast_visual_refresh_timer();
  if (timer) lv_timer_reset(timer);
  else timer = lv_timer_create(weather_forecast_apply_visuals_cb, 25, nullptr);
}

inline void apply_weather_forecast_to_entity(const std::string &entity_id,
                                             const std::string &day,
                                             bool valid, float high, float low,
                                             const std::string &unit) {
  ESP_LOGI("weather_forecast", "Applying %s forecast for %s: %s high=%.1f low=%.1f unit=%s",
    day.c_str(), entity_id.c_str(), valid ? "valid" : "unavailable",
    high, low, unit.c_str());
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  for (int i = 0; i < count; i++) {
    if (refs[i].entity_id == entity_id && refs[i].day == day) {
      refs[i].valid = valid;
      refs[i].high = high;
      refs[i].low = low;
      refs[i].source_unit = unit;
      refs[i].status_label = "";
      weather_forecast_schedule_visual_refresh();
    }
  }
}

inline void apply_weather_forecast_unavailable_for_entity(const std::string &entity_id) {
  ESP_LOGW("weather_forecast", "Marking forecast unavailable for %s", entity_id.c_str());
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  for (int i = 0; i < count; i++) {
    if (refs[i].entity_id == entity_id) {
      refs[i].valid = false;
      refs[i].high = 0;
      refs[i].low = 0;
      refs[i].source_unit = "";
      refs[i].status_label = "";
      weather_forecast_schedule_visual_refresh();
    }
  }
}

inline void apply_weather_days_to_entity(const std::string &entity_id, bool valid,
                                         const espcontrol::WeatherDaysForecast &forecast) {
  ESP_LOGI("weather_forecast", "Applying daily forecast for %s: %s, %d days",
    entity_id.c_str(), valid ? "valid" : "unavailable", forecast.day_count);
  WeatherDaysCardRef *refs = weather_days_card_refs();
  for (int i = 0; i < weather_days_card_count(); i++) {
    if (refs[i].entity_id != entity_id) continue;
    refs[i].valid = valid;
    if (valid) refs[i].forecast = forecast;
    weather_forecast_schedule_visual_refresh();
  }
}

inline void apply_weather_days_unavailable_for_entity(const std::string &entity_id) {
  WeatherDaysCardRef *refs = weather_days_card_refs();
  for (int i = 0; i < weather_days_card_count(); i++) {
    if (refs[i].entity_id != entity_id) continue;
    refs[i].valid = false;
    weather_forecast_schedule_visual_refresh();
  }
}

// Marks the cards that were waiting on a failed request as unavailable.
inline void weather_forecast_mark_unavailable(const std::string &entity_id, const std::string &day) {
  if (day == "days") apply_weather_days_unavailable_for_entity(entity_id);
  else apply_weather_forecast_unavailable_for_entity(entity_id);
}

inline void apply_weather_forecast_unavailable_all() {
  ESP_LOGW("weather_forecast", "Marking all forecast cards unavailable");
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  for (int i = 0; i < count; i++) {
    refs[i].valid = false;
    refs[i].high = 0;
    refs[i].low = 0;
    refs[i].source_unit = "";
    refs[i].status_label = "";
    weather_forecast_schedule_visual_refresh();
  }
  WeatherDaysCardRef *days = weather_days_card_refs();
  for (int i = 0; i < weather_days_card_count(); i++) {
    days[i].valid = false;
    weather_forecast_schedule_visual_refresh();
  }
}

inline void apply_weather_forecast_actions_required_for_entity(const std::string &entity_id) {
  ESP_LOGW("weather_forecast",
    "Forecast request timed out for %s; check that this ESPHome device is allowed to perform Home Assistant actions",
    entity_id.c_str());
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  for (int i = 0; i < count; i++) {
    if (refs[i].entity_id == entity_id) {
      refs[i].valid = false;
      refs[i].high = 0;
      refs[i].low = 0;
      refs[i].source_unit = "";
      refs[i].status_label = "";
      weather_forecast_schedule_visual_refresh();
    }
  }
}

inline bool weather_forecast_error_is_timeout(const std::string &message) {
  std::string lower;
  lower.reserve(message.size());
  for (char ch : message) {
    lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  }
  return lower.find("timeout") != std::string::npos ||
         lower.find("timed out") != std::string::npos;
}

inline bool weather_forecast_request_matches(const std::string &entity_id,
                                             const std::string &day,
                                             const std::string &other_entity_id,
                                             const std::string &other_day) {
  return entity_id == other_entity_id && day == other_day;
}

inline void register_weather_forecast_card(lv_obj_t *btn,
                                           lv_obj_t *value_lbl, lv_obj_t *unit_lbl,
                                           lv_obj_t *label_lbl,
                                           const std::string &entity_id,
                                           const std::string &day,
                                           const std::string &label) {
  int &count = weather_forecast_card_count();
  if (count >= MAX_GRID_SLOTS + MAX_SUBPAGE_ITEMS) {
    ESP_LOGW("weather_forecast", "Too many forecast cards; skipping updates");
    return;
  }
  weather_forecast_card_refs()[count++] = {
    btn, value_lbl, unit_lbl, label_lbl, entity_id, day, label, "", false, 0, 0, ""
  };
  apply_weather_forecast_card_text(weather_forecast_card_refs()[count - 1], false, 0, 0, "");
}

inline bool weather_forecast_entity_id_safe(const std::string &entity_id) {
  if (entity_id.compare(0, 8, "weather.") != 0) return false;
  for (char ch : entity_id) {
    if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '.')) return false;
  }
  return true;
}

inline bool parse_weather_forecast_temp(const std::string &value, float &out) {
  if (value.empty()) return false;
  char *end = nullptr;
  float parsed = strtof(value.c_str(), &end);
  if (end == value.c_str()) return false;
  if (!std::isfinite(parsed)) return false;
  out = parsed;
  return true;
}

struct WeatherForecastPayload {
  bool today_valid = false;
  float today_high = WEATHER_FORECAST_TEMP_MISSING;
  float today_low = WEATHER_FORECAST_TEMP_MISSING;
  bool tomorrow_valid = false;
  float tomorrow_high = WEATHER_FORECAST_TEMP_MISSING;
  float tomorrow_low = WEATHER_FORECAST_TEMP_MISSING;
  std::string unit;
};

inline bool parse_weather_forecast_payload(const std::string &payload,
                                           WeatherForecastPayload &out) {
  size_t p1 = payload.find('|');
  if (p1 == std::string::npos) return false;
  size_t p2 = payload.find('|', p1 + 1);
  if (p2 == std::string::npos) return false;
  size_t p3 = payload.find('|', p2 + 1);
  if (p3 == std::string::npos) return false;
  size_t p4 = payload.find('|', p3 + 1);
  if (p4 == std::string::npos) return false;

  std::string today_high_text = payload.substr(0, p1);
  std::string today_low_text = payload.substr(p1 + 1, p2 - p1 - 1);
  std::string tomorrow_high_text = payload.substr(p2 + 1, p3 - p2 - 1);
  std::string tomorrow_low_text = payload.substr(p3 + 1, p4 - p3 - 1);
  out.unit = payload.substr(p4 + 1);

  bool today_has_high = parse_weather_forecast_temp(today_high_text, out.today_high);
  bool today_has_low = parse_weather_forecast_temp(today_low_text, out.today_low);
  bool tomorrow_has_high = parse_weather_forecast_temp(tomorrow_high_text, out.tomorrow_high);
  bool tomorrow_has_low = parse_weather_forecast_temp(tomorrow_low_text, out.tomorrow_low);
  out.today_valid = today_has_high || today_has_low;
  out.tomorrow_valid = tomorrow_has_high || tomorrow_has_low;
  return out.today_valid || out.tomorrow_valid;
}

inline std::string weather_forecast_response_template(const std::string &entity_id) {
  return std::string("{% set entity = '") + entity_id + "' %}"
    "{% set response_data = response if response is defined and response is not none else {} %}"
    "{% set entity_response = response_data if 'forecast' in response_data else (response_data[entity] if entity in response_data else {}) %}"
    "{% set forecasts = entity_response['forecast'] if 'forecast' in entity_response else [] %}"
    "{% set today_date = now().date() %}{% set tomorrow_date = (now() + timedelta(days=1)).date() %}"
    "{% set ns = namespace(today=none, tomorrow=none) %}{% for item in forecasts %}"
    "{% set item_dt = as_datetime(item['datetime']) if 'datetime' in item else none %}{% set item_date = as_local(item_dt).date() if item_dt is not none else (as_datetime(item['date']).date() if 'date' in item else none) %}"
    "{% if item_date == today_date and ns.today is none %}{% set ns.today = item %}{% elif item_date == tomorrow_date and ns.tomorrow is none %}{% set ns.tomorrow = item %}{% endif %}"
    "{% endfor %}"
    "{% set today = ns.today if ns.today is not none else (forecasts[0] if forecasts|length > 0 else none) %}"
    "{% set tomorrow = ns.tomorrow if ns.tomorrow is not none else (forecasts[1] if forecasts|length > 1 else none) %}"
    "{% set high_keys = ['temperature','native_temperature','temperature_high','native_temperature_high','high_temperature','max_temperature','temperature_max','temp_high','max_temp','high'] %}"
    "{% set low_keys = ['templow','native_templow','temperature_low','native_temperature_low','low_temperature','min_temperature','temperature_min','temp_low','min_temp','low'] %}"
    "{% set unit_keys = ['temperature_unit','native_temperature_unit','unit_of_measurement','native_unit_of_measurement','unit'] %}"
    "{% set out = namespace(today_high='', today_low='', tomorrow_high='', tomorrow_low='', unit='') %}"
    "{% for key in high_keys %}{% if out.today_high == '' and today is not none and key in today %}{% set out.today_high = today[key] %}{% endif %}{% if out.tomorrow_high == '' and tomorrow is not none and key in tomorrow %}{% set out.tomorrow_high = tomorrow[key] %}{% endif %}{% endfor %}"
    "{% for key in low_keys %}{% if out.today_low == '' and today is not none and key in today %}{% set out.today_low = today[key] %}{% endif %}{% if out.tomorrow_low == '' and tomorrow is not none and key in tomorrow %}{% set out.tomorrow_low = tomorrow[key] %}{% endif %}{% endfor %}"
    "{% for key in unit_keys %}{% if out.unit == '' and key in entity_response %}{% set out.unit = entity_response[key] %}{% endif %}{% if out.unit == '' and today is not none and key in today %}{% set out.unit = today[key] %}{% endif %}{% if out.unit == '' and tomorrow is not none and key in tomorrow %}{% set out.unit = tomorrow[key] %}{% endif %}{% endfor %}"
    "{{ out.today_high }}|{{ out.today_low }}|{{ out.tomorrow_high }}|{{ out.tomorrow_low }}|"
    "{{ out.unit or state_attr(entity, 'temperature_unit') or state_attr(entity, 'native_temperature_unit') or state_attr(entity, 'unit_of_measurement') or '' }}";
}

inline uint32_t next_weather_forecast_call_id() {
  static uint32_t call_id = 1;
  return call_id++;
}

inline WeatherForecastPendingRequest *weather_forecast_pending_requests() {
  static WeatherForecastPendingRequest requests[WEATHER_FORECAST_PENDING_MAX];
  return requests;
}

inline WeatherForecastQueuedRequest *weather_forecast_queued_requests() {
  static WeatherForecastQueuedRequest requests[WEATHER_FORECAST_PENDING_MAX];
  return requests;
}

inline WeatherForecastRetryRequest *weather_forecast_retry_requests() {
  static WeatherForecastRetryRequest requests[WEATHER_FORECAST_PENDING_MAX];
  return requests;
}

inline uint32_t &weather_forecast_action_ready_ms() {
  static uint32_t due_ms = 0;
  return due_ms;
}

inline bool weather_forecast_actions_ready() {
  if (!ha_api_state_connected()) {
    weather_forecast_action_ready_ms() = 0;
    return false;
  }
  uint32_t &due_ms = weather_forecast_action_ready_ms();
  uint32_t now = esphome::millis();
  if (due_ms == 0) {
    due_ms = now + 10000;
    ESP_LOGI("weather_forecast",
      "Waiting 10 seconds for Home Assistant action subscription before requesting forecasts");
    return false;
  }
  return (int32_t) (now - due_ms) >= 0;
}

inline bool weather_forecast_pending_key(const std::string &entity_id,
                                         const std::string &day) {
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].call_id != 0 &&
        weather_forecast_request_matches(entity_id, day, requests[i].entity_id, requests[i].day)) {
      return true;
    }
  }
  return false;
}

inline bool weather_forecast_queue_key(const std::string &entity_id,
                                       const std::string &day) {
  WeatherForecastQueuedRequest *requests = weather_forecast_queued_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (!requests[i].entity_id.empty() &&
        weather_forecast_request_matches(entity_id, day, requests[i].entity_id, requests[i].day)) {
      return true;
    }
  }
  return false;
}

inline bool weather_forecast_retry_key(const std::string &entity_id,
                                       const std::string &day) {
  WeatherForecastRetryRequest *requests = weather_forecast_retry_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (!requests[i].entity_id.empty() &&
        weather_forecast_request_matches(entity_id, day, requests[i].entity_id, requests[i].day)) {
      return true;
    }
  }
  return false;
}

inline bool weather_forecast_any_pending() {
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].call_id != 0) return true;
  }
  return false;
}

inline bool weather_forecast_track_pending(uint32_t call_id,
                                           const std::string &entity_id,
                                           const std::string &day) {
  if (call_id == 0) return false;
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].call_id == call_id) return true;
  }
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].call_id == 0) {
      requests[i].call_id = call_id;
      requests[i].started_ms = esphome::millis();
      requests[i].entity_id = entity_id;
      requests[i].day = day;
      return true;
    }
  }
  return false;
}

inline void weather_forecast_clear_pending(uint32_t call_id) {
  if (call_id == 0) return;
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].call_id == call_id) requests[i] = WeatherForecastPendingRequest();
  }
}

inline void weather_forecast_clear_queue() {
  WeatherForecastQueuedRequest *requests = weather_forecast_queued_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    requests[i] = WeatherForecastQueuedRequest();
  }
}

inline void weather_forecast_clear_retry(const std::string &entity_id,
                                         const std::string &day) {
  WeatherForecastRetryRequest *requests = weather_forecast_retry_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (!requests[i].entity_id.empty() &&
        weather_forecast_request_matches(entity_id, day, requests[i].entity_id, requests[i].day)) {
      requests[i] = WeatherForecastRetryRequest();
    }
  }
}

inline void weather_forecast_clear_retries() {
  WeatherForecastRetryRequest *requests = weather_forecast_retry_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    requests[i] = WeatherForecastRetryRequest();
  }
}

inline bool weather_forecast_schedule_retry(const std::string &entity_id,
                                            const std::string &day,
                                            const char *reason) {
  if (!weather_forecast_entity_id_safe(entity_id)) return false;
  if (weather_forecast_pending_key(entity_id, day) ||
      weather_forecast_queue_key(entity_id, day) ||
      weather_forecast_retry_key(entity_id, day)) {
    return true;
  }
  WeatherForecastRetryRequest *requests = weather_forecast_retry_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].entity_id.empty()) {
      requests[i].entity_id = entity_id;
      requests[i].day = day;
      requests[i].due_ms = esphome::millis() + WEATHER_FORECAST_RETRY_DELAY_MS;
      ESP_LOGW("weather_forecast", "Retrying forecast request for %s in %u seconds: %s",
        entity_id.c_str(), (unsigned) (WEATHER_FORECAST_RETRY_DELAY_MS / 1000),
        reason ? reason : "failed");
      return true;
    }
  }
  ESP_LOGW("weather_forecast", "Too many delayed forecast retries; skipping %s",
    entity_id.c_str());
  return false;
}

inline bool weather_forecast_enqueue(const std::string &entity_id,
                                     const std::string &day) {
  if (!weather_forecast_entity_id_safe(entity_id)) return false;
  weather_forecast_clear_retry(entity_id, day);
  if (weather_forecast_pending_key(entity_id, day) ||
      weather_forecast_queue_key(entity_id, day)) {
    return true;
  }
  WeatherForecastQueuedRequest *requests = weather_forecast_queued_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].entity_id.empty()) {
      requests[i].entity_id = entity_id;
      requests[i].day = day;
      return true;
    }
  }
  ESP_LOGW("weather_forecast", "Too many queued forecast requests; skipping %s",
    entity_id.c_str());
  return false;
}

inline bool weather_forecast_dequeue(std::string &entity_id,
                                     std::string &day) {
  WeatherForecastQueuedRequest *requests = weather_forecast_queued_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].entity_id.empty()) continue;
    entity_id = requests[i].entity_id;
    day = requests[i].day;
    requests[i] = WeatherForecastQueuedRequest();
    return true;
  }
  return false;
}

inline bool weather_forecast_enqueue_due_retries() {
  if (!ha_api_state_connected()) return false;
  WeatherForecastRetryRequest *requests = weather_forecast_retry_requests();
  uint32_t now = esphome::millis();
  bool queued = false;
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    if (requests[i].entity_id.empty()) continue;
    if ((int32_t) (now - requests[i].due_ms) < 0) continue;
    std::string entity_id = requests[i].entity_id;
    std::string day = requests[i].day;
    requests[i] = WeatherForecastRetryRequest();
    queued = weather_forecast_enqueue(entity_id, day) || queued;
  }
  return queued;
}

inline void weather_forecast_send_next_queued();

inline void weather_forecast_cancel_pending_requests() {
  weather_forecast_action_ready_ms() = 0;
  weather_forecast_clear_queue();
  weather_forecast_clear_retries();
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    uint32_t call_id = requests[i].call_id;
    if (call_id == 0) continue;
    requests[i] = WeatherForecastPendingRequest();
    ha_cancel_action_response_callback(call_id, "api disconnected");
  }
}

inline bool weather_forecast_cancel_stale_requests() {
  WeatherForecastPendingRequest *requests = weather_forecast_pending_requests();
  uint32_t now = esphome::millis();
  for (int i = 0; i < WEATHER_FORECAST_PENDING_MAX; i++) {
    uint32_t call_id = requests[i].call_id;
    if (call_id == 0) continue;
    if (now - requests[i].started_ms < WEATHER_FORECAST_REQUEST_TIMEOUT_MS) continue;
    std::string entity_id = requests[i].entity_id;
    requests[i] = WeatherForecastPendingRequest();
    ESP_LOGW("weather_forecast", "Cancelling forecast request %u for %s: timeout",
      (unsigned) call_id, entity_id.c_str());
    ha_cancel_action_response_callback(call_id, "timeout");
    return true;
  }
  return false;
}

inline void request_weather_forecast_entity(const std::string &entity_id,
                                            const std::string &day) {
  if (!weather_forecast_entity_id_safe(entity_id) ||
      !ha_api_state_connected() ||
      !weather_forecast_actions_ready()) {
    weather_forecast_mark_unavailable(entity_id, day);
    return;
  }
#ifdef ESP_PLATFORM
  size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  size_t internal_largest = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (internal_free < HA_ACTION_INTERNAL_FREE_MIN_BYTES ||
      internal_largest < HA_ACTION_INTERNAL_LARGEST_MIN_BYTES) {
    ESP_LOGW("weather_forecast",
             "Deferring forecast request for %s: internal heap free=%u largest=%u",
             entity_id.c_str(), (unsigned) internal_free, (unsigned) internal_largest);
    weather_forecast_schedule_retry(entity_id, day, "low internal heap");
    return;
  }
#endif

  esphome::api::HomeassistantActionRequest req;
  uint32_t call_id = next_weather_forecast_call_id();
  if (!ha_action_begin(req, "weather.get_forecasts", false, 2, call_id)) {
    weather_forecast_mark_unavailable(entity_id, day);
    weather_forecast_schedule_retry(entity_id, day, "request setup failed");
    return;
  }
  req.wants_response = true;
  const bool daily_strip = day == "days";
  std::string response_template = daily_strip
    ? espcontrol::weather_days_response_template(entity_id)
    : weather_forecast_response_template(entity_id);
  req.response_template = decltype(req.response_template)(response_template);
  ha_action_add_entity(req, entity_id);
  ha_action_add_data(req, "type", "daily");
  uint32_t generation = ha_subscription_generation();

  if (!ha_register_action_response_callback(
    req.call_id,
    [entity_id, day, call_id = req.call_id, generation](const esphome::api::ActionResponse &response) {
      weather_forecast_clear_pending(call_id);
      if (generation != ha_subscription_generation()) {
        weather_forecast_send_next_queued();
        return;
      }
      if (!response.is_success()) {
        std::string error_message = response.get_error_message();
        ESP_LOGW("weather_forecast", "Forecast request failed for %s: %s",
          entity_id.c_str(), error_message.c_str());
        if (weather_forecast_error_is_timeout(error_message) && day != "days") {
          apply_weather_forecast_actions_required_for_entity(entity_id);
        } else {
          weather_forecast_mark_unavailable(entity_id, day);
        }
        weather_forecast_schedule_retry(entity_id, day, error_message.c_str());
        weather_forecast_send_next_queued();
        return;
      }
      auto json = response.get_json();
      const char *payload = json["response"].as<const char *>();
      if (payload == nullptr) {
        ESP_LOGW("weather_forecast", "Forecast response for %s did not include a rendered payload",
          entity_id.c_str());
        weather_forecast_mark_unavailable(entity_id, day);
        weather_forecast_schedule_retry(entity_id, day, "empty response");
        weather_forecast_send_next_queued();
        return;
      }
      if (day == "days") {
        espcontrol::WeatherDaysForecast days_forecast;
        const bool days_valid = espcontrol::parse_weather_days_payload(payload, days_forecast);
        if (!days_valid) {
          ESP_LOGW("weather_forecast", "No usable daily forecast for %s: %s", entity_id.c_str(), payload);
          weather_forecast_schedule_retry(entity_id, day, "no usable daily forecast");
        }
        apply_weather_days_to_entity(entity_id, days_valid, days_forecast);
        weather_forecast_send_next_queued();
        return;
      }
      WeatherForecastPayload forecast;
      bool valid = parse_weather_forecast_payload(payload, forecast);
      if (!valid) {
        ESP_LOGW("weather_forecast", "No usable forecast temperatures for %s: %s",
          entity_id.c_str(), payload);
        weather_forecast_schedule_retry(entity_id, day, "no usable forecast temperatures");
      }
      apply_weather_forecast_to_entity(entity_id, "today", forecast.today_valid,
        forecast.today_high, forecast.today_low, forecast.unit);
      apply_weather_forecast_to_entity(entity_id, "tomorrow", forecast.tomorrow_valid,
        forecast.tomorrow_high, forecast.tomorrow_low, forecast.unit);
      weather_forecast_send_next_queued();
    })) {
    weather_forecast_mark_unavailable(entity_id, day);
    weather_forecast_schedule_retry(entity_id, day, "callback setup failed");
    return;
  }
  if (!weather_forecast_track_pending(req.call_id, entity_id, day)) {
    ha_cancel_action_response_callback(req.call_id, "too many pending forecasts");
    weather_forecast_mark_unavailable(entity_id, day);
    return;
  }
  ESP_LOGI("weather_forecast", "Requesting daily forecast for %s%s", entity_id.c_str(),
    daily_strip ? " (daily forecast card)" : "");
  if (!ha_action_send(req)) {
    weather_forecast_clear_pending(req.call_id);
    ha_cancel_action_response_callback(req.call_id, "send failed");
    weather_forecast_mark_unavailable(entity_id, day);
    weather_forecast_schedule_retry(entity_id, day, "send failed");
    weather_forecast_send_next_queued();
  }
}

inline void weather_forecast_send_next_queued() {
  if (!weather_forecast_actions_ready() || weather_forecast_any_pending()) return;
  weather_forecast_enqueue_due_retries();
  std::string entity_id;
  std::string day;
  if (!weather_forecast_dequeue(entity_id, day)) return;
  request_weather_forecast_entity(entity_id, day);
}

inline void refresh_weather_forecast_cards() {
  WeatherForecastCardRef *refs = weather_forecast_card_refs();
  int count = weather_forecast_card_count();
  WeatherDaysCardRef *days = weather_days_card_refs();
  int days_count = weather_days_card_count();
  if (count <= 0 && days_count <= 0) return;
  std::vector<std::string> requested;
  requested.reserve(count);
  for (int i = 0; i < count; i++) {
    const std::string &entity_id = refs[i].entity_id;
    if (entity_id.empty()) continue;
    std::string request_key = entity_id;
    bool already_requested = false;
    for (const auto &existing : requested) {
      if (existing == request_key) {
        already_requested = true;
        break;
      }
    }
    if (already_requested) continue;
    requested.push_back(request_key);
    weather_forecast_enqueue(entity_id, "");
  }
  for (int i = 0; i < days_count; i++) {
    const std::string &entity_id = days[i].entity_id;
    if (entity_id.empty()) continue;
    // weather_forecast_enqueue ignores an entity that is already queued.
    weather_forecast_enqueue(entity_id, "days");
  }
  weather_forecast_send_next_queued();
}

#endif
