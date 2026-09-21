#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "weather_days_forecast.h"

using namespace espcontrol;

namespace {

int failures = 0;

void check(bool condition, const char *message) {
  if (!condition) {
    std::fprintf(stderr, "FAIL: %s\n", message);
    failures++;
  }
}

bool near(float actual, float expected) { return std::fabs(actual - expected) < 0.001f; }

void test_full_payload() {
  WeatherDaysForecast forecast;
  const bool ok = parse_weather_days_payload(
    "°C|16.6|13.2|1,sunny,17.1,14|2,partlycloudy,16.5,14.3|3,cloudy,15.4,13.5|"
    "4,partlycloudy,15.2,14.1|5,rainy,14,11|6,pouring,13,10",
    forecast);
  check(ok, "a full forecast parses");
  check(forecast.unit == "°C", "the unit is kept");
  check(near(forecast.today_high, 16.6f) && near(forecast.today_low, 13.2f), "today's high and low are read");
  check(forecast.day_count == 6, "six following days are read");
  check(forecast.days[0].weekday == 1 && forecast.days[0].condition == "sunny", "the first day has its weekday and condition");
  check(near(forecast.days[0].high, 17.1f) && near(forecast.days[0].low, 14.0f), "the first day has its high and low");
  check(forecast.days[5].weekday == 6 && forecast.days[5].condition == "pouring", "the last day is read");
}

void test_partial_payloads() {
  WeatherDaysForecast forecast;
  check(parse_weather_days_payload("°C|||1,sunny,17.1,14", forecast), "a forecast without today still parses");
  check(forecast.today_high == WEATHER_DAYS_TEMP_MISSING, "a missing today high stays missing");
  check(forecast.day_count == 1, "one following day is read");

  check(parse_weather_days_payload("°F|20|10|,rainy,18,", forecast), "a day without a date still parses");
  check(forecast.days[0].weekday == -1, "an unknown weekday is marked unknown");
  check(forecast.days[0].low == WEATHER_DAYS_TEMP_MISSING, "a missing low stays missing");

  check(!parse_weather_days_payload("°C||", forecast), "an empty forecast is not usable");
  check(!parse_weather_days_payload("", forecast), "an empty payload is not usable");

  check(parse_weather_days_payload("°C|1|2|9,sunny,3,4|,,,", forecast), "unexpected values are tolerated");
  check(forecast.days[0].weekday == -1 && forecast.day_count == 1, "invalid weekdays and empty days are skipped");

  const std::string many = "°C|1|2|0,a,1,1|1,b,1,1|2,c,1,1|3,d,1,1|4,e,1,1|5,f,1,1|6,g,1,1";
  check(parse_weather_days_payload(many, forecast) && forecast.day_count == WEATHER_DAYS_MAX,
        "no more than the supported number of days are kept");
}

void test_layout_rules() {
  check(weather_days_visible_for_columns(1) == 0, "a single card shows current conditions only");
  check(weather_days_visible_for_columns(2) == 2, "a wide card shows two days");
  check(weather_days_visible_for_columns(3) == 4, "an extra wide card shows four days");
  check(weather_days_visible_for_columns(4) == 5, "a four column card shows five days");
  check(weather_days_visible_for_columns(5) == 6, "an ultra wide card shows six days");
  check(std::string(weather_weekday_short_name(0)) == "Mon" && std::string(weather_weekday_short_name(6)) == "Sun",
        "weekday numbers follow Monday first");
  check(std::string(weather_weekday_short_name(-1)).empty() && std::string(weather_weekday_short_name(7)).empty(),
        "unknown weekdays have no name");
}

void test_template() {
  const std::string tpl = weather_days_response_template("weather.forecast_home");
  check(tpl.find("{% set entity = 'weather.forecast_home' %}") == 0, "the template targets the configured entity");
  check(tpl.find("ns.days|length < 6") != std::string::npos, "the template limits the number of days");
  check(tpl.find("item_date.weekday()") != std::string::npos, "the template reports weekday numbers");
}

}  // namespace

int main() {
  test_full_payload();
  test_partial_payloads();
  test_layout_rules();
  test_template();
  if (failures != 0) {
    std::fprintf(stderr, "%d weather daily forecast check(s) failed\n", failures);
    return EXIT_FAILURE;
  }
  std::puts("Weather daily forecast tests passed.");
  return EXIT_SUCCESS;
}
