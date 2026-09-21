#pragma once

// Data for Weather cards in Daily Forecast mode. Kept free of LVGL and ESPHome
// types so the host firmware tests can check payload parsing and layout rules.

#include <cmath>
#include <cstdlib>
#include <string>

namespace espcontrol {

constexpr int WEATHER_DAYS_MAX = 6;
constexpr float WEATHER_DAYS_TEMP_MISSING = 32767.0f;

struct WeatherDay {
  int weekday = -1;  // 0 = Monday, as Python's date.weekday() returns it
  std::string condition;
  float high = WEATHER_DAYS_TEMP_MISSING;
  float low = WEATHER_DAYS_TEMP_MISSING;
};

struct WeatherDaysForecast {
  std::string unit;
  float today_high = WEATHER_DAYS_TEMP_MISSING;
  float today_low = WEATHER_DAYS_TEMP_MISSING;
  WeatherDay days[WEATHER_DAYS_MAX];
  int day_count = 0;
};

inline bool weather_days_parse_temp(const std::string &text, float &out) {
  if (text.empty()) return false;
  char *end = nullptr;
  const float parsed = std::strtof(text.c_str(), &end);
  if (end == text.c_str() || !std::isfinite(parsed)) return false;
  out = parsed;
  return true;
}

// Returns the next `separator`-delimited field from `text`, starting at `pos`.
inline std::string weather_days_next_field(const std::string &text, size_t &pos, char separator) {
  if (pos > text.size()) return std::string();
  const size_t end = text.find(separator, pos);
  std::string field = text.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
  pos = end == std::string::npos ? text.size() + 1 : end + 1;
  return field;
}

// Parses the Home Assistant template output:
//   unit|today_high|today_low|weekday,condition,high,low|...
// Returns false when it holds no usable forecast.
inline bool parse_weather_days_payload(const std::string &payload, WeatherDaysForecast &out) {
  out = WeatherDaysForecast();
  size_t pos = 0;
  out.unit = weather_days_next_field(payload, pos, '|');
  weather_days_parse_temp(weather_days_next_field(payload, pos, '|'), out.today_high);
  weather_days_parse_temp(weather_days_next_field(payload, pos, '|'), out.today_low);
  while (pos <= payload.size() && out.day_count < WEATHER_DAYS_MAX) {
    const std::string entry = weather_days_next_field(payload, pos, '|');
    size_t entry_pos = 0;
    WeatherDay day;
    const std::string weekday = weather_days_next_field(entry, entry_pos, ',');
    if (weekday.size() == 1 && weekday[0] >= '0' && weekday[0] <= '6') day.weekday = weekday[0] - '0';
    day.condition = weather_days_next_field(entry, entry_pos, ',');
    const bool has_high = weather_days_parse_temp(weather_days_next_field(entry, entry_pos, ','), day.high);
    const bool has_low = weather_days_parse_temp(weather_days_next_field(entry, entry_pos, ','), day.low);
    if (day.condition.empty() && !has_high && !has_low) continue;
    out.days[out.day_count++] = day;
  }
  return out.day_count > 0 || out.today_high != WEATHER_DAYS_TEMP_MISSING ||
         out.today_low != WEATHER_DAYS_TEMP_MISSING;
}

// Days shown beside the current conditions on a card `col_span` columns wide.
inline int weather_days_visible_for_columns(int col_span) {
  if (col_span <= 1) return 0;
  if (col_span == 2) return 2;
  if (col_span == 3) return 4;
  return col_span + 1 < WEATHER_DAYS_MAX ? col_span + 1 : WEATHER_DAYS_MAX;
}

// English short day names; translated through espcontrol_i18n on the panel.
inline const char *weather_weekday_short_name(int weekday) {
  static const char *const NAMES[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  return weekday >= 0 && weekday < 7 ? NAMES[weekday] : "";
}

// Home Assistant template for weather.get_forecasts: today's high and low,
// then up to WEATHER_DAYS_MAX following days, as parse_weather_days_payload
// expects.
inline std::string weather_days_response_template(const std::string &entity_id) {
  return std::string("{% set entity = '") + entity_id + "' %}"
    "{% set response_data = response if response is defined and response is not none else {} %}"
    "{% set entity_response = response_data if 'forecast' in response_data else (response_data[entity] if entity in response_data else {}) %}"
    "{% set forecasts = entity_response['forecast'] if 'forecast' in entity_response else [] %}"
    "{% set high_keys = ['temperature','native_temperature','temperature_high','native_temperature_high','high_temperature','max_temperature','temperature_max','temp_high','max_temp','high'] %}"
    "{% set low_keys = ['templow','native_templow','temperature_low','native_temperature_low','low_temperature','min_temperature','temperature_min','temp_low','min_temp','low'] %}"
    "{% set unit_keys = ['temperature_unit','native_temperature_unit','unit_of_measurement','native_unit_of_measurement','unit'] %}"
    "{% macro pick(item, keys) %}{% set found = namespace(value='') %}{% for key in keys %}{% if found.value == '' and item is not none and key in item and item[key] is not none %}{% set found.value = item[key] %}{% endif %}{% endfor %}{{ found.value }}{% endmacro %}"
    "{% set today_date = now().date() %}"
    "{% set ns = namespace(today=none, days=[], weekdays=[]) %}"
    "{% for item in forecasts %}"
    "{% set item_dt = as_datetime(item['datetime']) if 'datetime' in item else (as_datetime(item['date']) if 'date' in item else none) %}"
    "{% set item_date = as_local(item_dt).date() if item_dt is not none else none %}"
    "{% if item_date is none %}"
    "{% if loop.first and ns.today is none %}{% set ns.today = item %}{% elif ns.days|length < " +
    std::to_string(WEATHER_DAYS_MAX) + " %}{% set ns.days = ns.days + [item] %}{% set ns.weekdays = ns.weekdays + [''] %}{% endif %}"
    "{% elif item_date == today_date %}{% if ns.today is none %}{% set ns.today = item %}{% endif %}"
    "{% elif item_date > today_date and ns.days|length < " + std::to_string(WEATHER_DAYS_MAX) + " %}"
    "{% set ns.days = ns.days + [item] %}{% set ns.weekdays = ns.weekdays + [item_date.weekday()] %}"
    "{% endif %}{% endfor %}"
    "{% set unit = namespace(value='') %}"
    "{% for key in unit_keys %}{% if unit.value == '' and key in entity_response %}{% set unit.value = entity_response[key] %}{% endif %}{% endfor %}"
    "{{ unit.value or state_attr(entity, 'temperature_unit') or '' }}|{{ pick(ns.today, high_keys) }}|{{ pick(ns.today, low_keys) }}"
    "{% for day in ns.days %}|{{ ns.weekdays[loop.index0] }},{{ day['condition'] if 'condition' in day and day['condition'] is not none else '' }},{{ pick(day, high_keys) }},{{ pick(day, low_keys) }}{% endfor %}";
}

}  // namespace espcontrol
