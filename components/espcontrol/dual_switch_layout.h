#pragma once

// Layout rules for Dual Switch cards. Kept free of LVGL and ESPHome types so
// the host firmware tests can check them.

#include <string>

namespace espcontrol {

// Saved in the precision field: "" stacks the two switches, "side" places
// them side by side.
inline bool dual_switch_side_by_side(const std::string &precision) {
  return precision == "side";
}

// Which switch a touch at (x, y) belongs to on a card spanning x1..x2 and
// y1..y2: 0 is the top or left switch (entity), 1 the bottom or right switch
// (sensor).
inline int dual_switch_half_at(bool side_by_side, int x1, int y1, int x2, int y2,
                               int x, int y) {
  if (side_by_side) return x * 2 < x1 + x2 ? 0 : 1;
  return y * 2 < y1 + y2 ? 0 : 1;
}

struct DualSwitchBadge {
  int diameter = 0;
  int icon_scale = 256;  // LVGL transform scale; 256 is full size
};

constexpr int DUAL_SWITCH_BADGE_MIN_PX = 24;
constexpr int DUAL_SWITCH_ICON_PERCENT = 60;

// Round icon badge for one switch area `width` x `height` pixels. Stacked
// switches place the badge beside the name, so it may use up to half the
// width; side-by-side switches place it above a name `text_px` tall. The
// badge prefers the icon font size, and the icon fills 60% of it.
inline DualSwitchBadge dual_switch_badge(bool side_by_side, int width, int height,
                                         int icon_px, int text_px, int gap_px) {
  DualSwitchBadge badge;
  const int ideal = icon_px > DUAL_SWITCH_BADGE_MIN_PX ? icon_px : DUAL_SWITCH_BADGE_MIN_PX;
  int available = side_by_side ? height - text_px - gap_px : height;
  const int width_limit = side_by_side ? width : width / 2;
  if (width_limit < available) available = width_limit;
  badge.diameter = ideal < available ? ideal : available;
  if (badge.diameter < DUAL_SWITCH_BADGE_MIN_PX) badge.diameter = DUAL_SWITCH_BADGE_MIN_PX;
  if (icon_px > 0) {
    badge.icon_scale = badge.diameter * 256 * DUAL_SWITCH_ICON_PERCENT / (100 * icon_px);
  }
  return badge;
}

}  // namespace espcontrol
