#include <cstdio>
#include <cstdlib>

#include "dual_switch_layout.h"

using namespace espcontrol;

namespace {

int failures = 0;

void check(bool condition, const char *message) {
  if (!condition) {
    std::fprintf(stderr, "FAIL: %s\n", message);
    failures++;
  }
}

void test_layout_setting() {
  check(!dual_switch_side_by_side(""), "switches are stacked by default");
  check(dual_switch_side_by_side("side"), "side places the switches side by side");
  check(!dual_switch_side_by_side("text"), "other saved values keep the stacked layout");
}

void test_touch_targets() {
  // A card from (100, 200) to (299, 399).
  check(dual_switch_half_at(false, 100, 200, 299, 399, 150, 210) == 0, "a touch near the top toggles the top switch");
  check(dual_switch_half_at(false, 100, 200, 299, 399, 150, 290) == 0, "a touch just above the middle toggles the top switch");
  check(dual_switch_half_at(false, 100, 200, 299, 399, 150, 310) == 1, "a touch just below the middle toggles the bottom switch");
  check(dual_switch_half_at(false, 100, 200, 299, 399, 290, 390) == 1, "a touch near the bottom toggles the bottom switch");
  check(dual_switch_half_at(true, 100, 200, 299, 399, 190, 390) == 0, "a touch left of the middle toggles the left switch");
  check(dual_switch_half_at(true, 100, 200, 299, 399, 210, 210) == 1, "a touch right of the middle toggles the right switch");
}

void test_badge_size() {
  // 7-inch portrait card: 158 x 163 content, 55 px icons, 26 px text, 8 px gap.
  DualSwitchBadge stacked = dual_switch_badge(false, 158, 77, 55, 26, 12);
  check(stacked.diameter == 55, "a roomy stacked switch uses a badge the size of the icon font");
  check(stacked.icon_scale == 153, "the icon fills 60% of the badge");

  DualSwitchBadge short_stacked = dual_switch_badge(false, 158, 40, 55, 26, 12);
  check(short_stacked.diameter == 40, "a short stacked switch fits the badge to its height");
  check(short_stacked.icon_scale == 111, "a smaller badge shrinks its icon with it");

  DualSwitchBadge narrow_stacked = dual_switch_badge(false, 90, 77, 55, 26, 12);
  check(narrow_stacked.diameter == 45, "a stacked badge leaves half the width for the name");

  DualSwitchBadge side = dual_switch_badge(true, 75, 163, 55, 26, 12);
  check(side.diameter == 55, "a side-by-side badge uses the icon font size when it fits");

  DualSwitchBadge side_narrow = dual_switch_badge(true, 50, 163, 55, 26, 12);
  check(side_narrow.diameter == 50, "a narrow side-by-side badge fits the width");

  DualSwitchBadge side_short = dual_switch_badge(true, 150, 80, 55, 26, 12);
  check(side_short.diameter == 42, "a short side-by-side badge leaves room for the name below");

  DualSwitchBadge tiny = dual_switch_badge(false, 20, 10, 55, 26, 12);
  check(tiny.diameter == DUAL_SWITCH_BADGE_MIN_PX, "badges never shrink below the minimum size");
}

}  // namespace

int main() {
  test_layout_setting();
  test_touch_targets();
  test_badge_size();
  if (failures != 0) {
    std::fprintf(stderr, "%d dual switch layout check(s) failed\n", failures);
    return EXIT_FAILURE;
  }
  std::puts("Dual switch layout tests passed.");
  return EXIT_SUCCESS;
}
