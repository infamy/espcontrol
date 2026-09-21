#pragma once

// Dual Switch card: two Home Assistant switches in one card, stacked or side
// by side. Each switch shows a round icon badge that fills with the on colour
// while its entity is on. The card stays a single LVGL button, so screensaver
// wake, screen lock and long-press handling are shared with other cards; a tap
// toggles the switch under the touch point.
//
// Storage: entity/label/icon = first switch, sensor/unit/icon_on = second
// switch, precision = layout ("" stacked, "side" side by side).

#include "dual_switch_layout.h"

inline void send_toggle_action(const std::string &entity_id);

struct DualSwitchHalf {
  std::string entity_id;
  lv_obj_t *area = nullptr;
  lv_obj_t *badge = nullptr;
  lv_obj_t *icon_lbl = nullptr;
  lv_obj_t *name_lbl = nullptr;
  bool on = false;
  bool available = true;
};

struct DualSwitchCtx;
inline std::vector<DualSwitchCtx *> &dual_switch_cards() {
  static std::vector<DualSwitchCtx *> cards;
  return cards;
}

struct DualSwitchCtx {
  lv_obj_t *btn = nullptr;
  lv_obj_t *root = nullptr;
  bool side_by_side = false;
  uint32_t on_color = DEFAULT_SLIDER_COLOR;
  lv_color_t text_color = lv_color_hex(DARK_TEXT_PRIMARY);
  int icon_px = 0;
  int text_px = 0;
  DualSwitchHalf halves[2];
  lv_timer_t *press_timer = nullptr;

  ~DualSwitchCtx() {
    ha_release_callbacks_for_owner(this);
    if (press_timer) lv_timer_del(press_timer);
    press_timer = nullptr;
    auto &cards = dual_switch_cards();
    cards.erase(std::remove(cards.begin(), cards.end(), this), cards.end());
  }
};

inline DualSwitchCtx *dual_switch_card_for(lv_obj_t *btn) {
  if (!btn) return nullptr;
  for (DualSwitchCtx *ctx : dual_switch_cards()) {
    if (ctx->btn == btn) return ctx;
  }
  return nullptr;
}

inline const char *dual_switch_icon(const std::string &icon, const std::string &entity_id) {
  if (!icon.empty() && icon != "Auto") return find_icon(icon.c_str());
  return domain_default_icon(entity_id.substr(0, entity_id.find('.')));
}

inline void dual_switch_apply_half(DualSwitchCtx *ctx, int index) {
  if (!ctx || index < 0 || index > 1) return;
  DualSwitchHalf &half = ctx->halves[index];
  if (!half.badge || !half.icon_lbl) return;
  lv_obj_set_style_bg_color(half.badge,
    half.on ? lv_color_hex(ctx->on_color) : ctx->text_color, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(half.badge, half.on ? LV_OPA_COVER : LV_OPA_10, LV_PART_MAIN);
  lv_obj_set_style_text_color(half.icon_lbl,
    half.on ? lv_color_hex(readable_text_color_for_bg(ctx->on_color)) : ctx->text_color,
    LV_PART_MAIN);
  if (half.area) {
    lv_obj_set_style_opa(half.area, half.available ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
  }
}

inline void dual_switch_layout(DualSwitchCtx *ctx) {
  if (!ctx || !ctx->root) return;
  const int width = lv_obj_get_content_width(ctx->root);
  const int height = lv_obj_get_content_height(ctx->root);
  if (width <= 0 || height <= 0) return;
  const int gap = lv_obj_get_style_pad_row(ctx->root, LV_PART_MAIN);
  const int half_width = ctx->side_by_side ? (width - gap) / 2 : width;
  const int half_height = ctx->side_by_side ? height : (height - gap) / 2;
  const int inner_gap = ctx->halves[0].area
    ? lv_obj_get_style_pad_column(ctx->halves[0].area, LV_PART_MAIN) : 0;
  const espcontrol::DualSwitchBadge badge = espcontrol::dual_switch_badge(
    ctx->side_by_side, half_width, half_height, ctx->icon_px, ctx->text_px, inner_gap);
  for (DualSwitchHalf &half : ctx->halves) {
    if (!half.badge || !half.icon_lbl) continue;
    lv_obj_set_size(half.badge, badge.diameter, badge.diameter);
    lv_obj_set_style_transform_scale(half.icon_lbl, badge.icon_scale, LV_PART_MAIN);
    lv_obj_center(half.icon_lbl);
  }
}

inline void dual_switch_create_half(DualSwitchCtx *ctx, int index, const std::string &entity_id,
                                    const std::string &label, const char *icon,
                                    const lv_font_t *icon_font, const lv_font_t *text_font,
                                    lv_coord_t radius, lv_coord_t inner_gap) {
  DualSwitchHalf &half = ctx->halves[index];
  half.entity_id = entity_id;

  lv_obj_t *area = lv_obj_create(ctx->root);
  lv_obj_remove_style_all(area);
  lv_obj_clear_flag(area, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(area, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_grow(area, 1);
  lv_obj_set_style_radius(area, radius, LV_PART_MAIN);
  lv_obj_set_style_bg_color(area, lv_color_hex(ctx->on_color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(area, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(area, LV_OPA_20,
    static_cast<lv_style_selector_t>(LV_PART_MAIN) | static_cast<lv_style_selector_t>(LV_STATE_PRESSED));
  lv_obj_set_style_pad_column(area, inner_gap, LV_PART_MAIN);
  lv_obj_set_style_pad_row(area, inner_gap, LV_PART_MAIN);
  lv_obj_set_flex_flow(area, ctx->side_by_side ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);
  if (ctx->side_by_side) {
    lv_obj_set_size(area, 0, lv_pct(100));
    lv_obj_set_flex_align(area, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  } else {
    lv_obj_set_size(area, lv_pct(100), 0);
    lv_obj_set_flex_align(area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  }
  half.area = area;

  lv_obj_t *badge = lv_obj_create(area);
  lv_obj_remove_style_all(badge);
  lv_obj_clear_flag(badge, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(badge, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_radius(badge, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_size(badge, espcontrol::DUAL_SWITCH_BADGE_MIN_PX, espcontrol::DUAL_SWITCH_BADGE_MIN_PX);
  half.badge = badge;

  lv_obj_t *icon_lbl = lv_label_create(badge);
  if (icon_font) lv_obj_set_style_text_font(icon_lbl, icon_font, LV_PART_MAIN);
  lv_obj_set_style_transform_pivot_x(icon_lbl, lv_pct(50), LV_PART_MAIN);
  lv_obj_set_style_transform_pivot_y(icon_lbl, lv_pct(50), LV_PART_MAIN);
  lv_label_set_display_text(icon_lbl, icon ? icon : "");
  lv_obj_center(icon_lbl);
  half.icon_lbl = icon_lbl;

  lv_obj_t *name_lbl = lv_label_create(area);
  if (text_font) lv_obj_set_style_text_font(name_lbl, text_font, LV_PART_MAIN);
  lv_obj_set_style_text_color(name_lbl, ctx->text_color, LV_PART_MAIN);
  lv_label_set_long_mode(name_lbl, LV_LABEL_LONG_WRAP);
  if (ctx->side_by_side) {
    lv_obj_set_width(name_lbl, lv_pct(100));
  } else {
    lv_obj_set_width(name_lbl, 0);
    lv_obj_set_flex_grow(name_lbl, 1);
  }
  std::string text = label.empty() ? entity_id : label;
  if (entity_id.empty()) text = espcontrol_i18n(std::string("Configure"));
  lv_label_set_display_text(name_lbl, text.c_str());
  half.name_lbl = name_lbl;

  dual_switch_apply_half(ctx, index);
}

inline DualSwitchCtx *setup_dual_switch_card(BtnSlot &s, const ParsedCfg &p, uint32_t on_color) {
  if (!s.btn) return nullptr;
  if (s.icon_lbl) lv_obj_add_flag(s.icon_lbl, LV_OBJ_FLAG_HIDDEN);
  if (s.text_lbl) lv_obj_add_flag(s.text_lbl, LV_OBJ_FLAG_HIDDEN);
  if (s.sensor_container) lv_obj_add_flag(s.sensor_container, LV_OBJ_FLAG_HIDDEN);

  DualSwitchCtx *ctx = new DualSwitchCtx();
  ctx->btn = s.btn;
  ctx->side_by_side = espcontrol::dual_switch_side_by_side(p.precision);
  ctx->on_color = on_color;
  ctx->text_color = lv_obj_get_style_text_color(s.btn, LV_PART_MAIN);
  const lv_font_t *icon_font = s.icon_lbl ? lv_obj_get_style_text_font(s.icon_lbl, LV_PART_MAIN) : nullptr;
  const lv_font_t *text_font = s.text_lbl ? lv_obj_get_style_text_font(s.text_lbl, LV_PART_MAIN) : nullptr;
  ctx->icon_px = icon_font ? lv_font_get_line_height(icon_font) : 0;
  ctx->text_px = text_font ? lv_font_get_line_height(text_font) : 0;
  dual_switch_cards().push_back(ctx);

  const lv_coord_t pad = lv_obj_get_style_pad_left(s.btn, LV_PART_MAIN);
  const lv_coord_t radius = lv_obj_get_style_radius(s.btn, LV_PART_MAIN);
  lv_obj_t *root = lv_obj_create(s.btn);
  lv_obj_remove_style_all(root);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_style_pad_row(root, pad / 2, LV_PART_MAIN);
  lv_obj_set_style_pad_column(root, pad / 2, LV_PART_MAIN);
  lv_obj_set_flex_flow(root, ctx->side_by_side ? LV_FLEX_FLOW_ROW : LV_FLEX_FLOW_COLUMN);
  ctx->root = root;
  // The card owns the context through this container, which the grid deletes
  // on every rebuild and with its subpage screen.
  lv_obj_add_event_cb(root, [](lv_event_t *e) {
    delete static_cast<DualSwitchCtx *>(lv_event_get_user_data(e));
  }, LV_EVENT_DELETE, ctx);
  lv_obj_add_event_cb(root, [](lv_event_t *e) {
    dual_switch_layout(static_cast<DualSwitchCtx *>(lv_event_get_user_data(e)));
  }, LV_EVENT_SIZE_CHANGED, ctx);

  const lv_coord_t inner_gap = pad * 3 / 4;
  dual_switch_create_half(ctx, 0, p.entity, p.label, dual_switch_icon(p.icon, p.entity),
                          icon_font, text_font, radius, inner_gap);
  dual_switch_create_half(ctx, 1, p.sensor, p.unit, dual_switch_icon(p.icon_on, p.sensor),
                          icon_font, text_font, radius, inner_gap);
  return ctx;
}

inline void dual_switch_subscribe(DualSwitchCtx *ctx, const ParsedCfg &p) {
  if (!ctx) return;
  HaCallbackOwnerScope callback_owner(ctx);
  for (int index = 0; index < 2; index++) {
    const std::string &entity_id = ctx->halves[index].entity_id;
    if (entity_id.empty()) continue;
    ha_subscribe_state(entity_id, [ctx, index](esphome::StringRef state) {
      DualSwitchHalf &half = ctx->halves[index];
      half.on = is_entity_on_ref(state);
      half.available = normalized_state_text(state) != "unavailable";
      dual_switch_apply_half(ctx, index);
    });
    const std::string &label = index == 0 ? p.label : p.unit;
    if (!label.empty()) continue;
    ha_subscribe_attribute(entity_id, std::string("friendly_name"),
      [ctx, index](esphome::StringRef name) {
        lv_obj_t *name_lbl = ctx->halves[index].name_lbl;
        if (!name_lbl || name.empty()) return;
        lv_label_set_display_text(
          name_lbl, string_ref_limited(name, HA_FRIENDLY_NAME_MAX_LEN).c_str());
      });
  }
  dual_switch_layout(ctx);
}

inline bool dual_switch_touch_point(lv_point_t &point) {
  lv_indev_t *indev = lv_indev_active();
  if (!indev) {
    for (lv_indev_t *next = lv_indev_get_next(nullptr); next; next = lv_indev_get_next(next)) {
      if (lv_indev_get_type(next) == LV_INDEV_TYPE_POINTER) {
        indev = next;
        break;
      }
    }
  }
  if (!indev || lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) return false;
  lv_indev_get_point(indev, &point);
  return true;
}

inline int dual_switch_touched_half(DualSwitchCtx *ctx) {
  lv_point_t point;
  if (!ctx || !ctx->btn || !dual_switch_touch_point(point)) return -1;
  lv_area_t area;
  lv_obj_get_coords(ctx->btn, &area);
  return espcontrol::dual_switch_half_at(
    ctx->side_by_side, area.x1, area.y1, area.x2, area.y2, point.x, point.y);
}

inline void dual_switch_clear_pressed(DualSwitchCtx *ctx) {
  if (!ctx) return;
  if (ctx->press_timer) {
    lv_timer_del(ctx->press_timer);
    ctx->press_timer = nullptr;
  }
  for (DualSwitchHalf &half : ctx->halves) {
    if (half.area) lv_obj_clear_state(half.area, LV_STATE_PRESSED);
  }
}

// Highlights only the touched switch instead of the whole card. A short timer
// clears it when the press turns into a swipe and no click follows.
inline void dual_switch_handle_press(lv_obj_t *btn) {
  DualSwitchCtx *ctx = dual_switch_card_for(btn);
  if (!ctx) return;
  lv_obj_clear_state(btn, LV_STATE_PRESSED);
  dual_switch_clear_pressed(ctx);
  const int index = dual_switch_touched_half(ctx);
  if (index < 0 || !ctx->halves[index].area) return;
  lv_obj_add_state(ctx->halves[index].area, LV_STATE_PRESSED);
  ctx->press_timer = lv_timer_create([](lv_timer_t *timer) {
    DualSwitchCtx *owner = static_cast<DualSwitchCtx *>(lv_timer_get_user_data(timer));
    owner->press_timer = nullptr;
    for (DualSwitchHalf &half : owner->halves) {
      if (half.area) lv_obj_clear_state(half.area, LV_STATE_PRESSED);
    }
  }, 400, ctx);
  if (ctx->press_timer) lv_timer_set_repeat_count(ctx->press_timer, 1);
}

inline void dual_switch_handle_click(lv_obj_t *btn) {
  DualSwitchCtx *ctx = dual_switch_card_for(btn);
  if (!ctx) return;
  const int index = dual_switch_touched_half(ctx);
  dual_switch_clear_pressed(ctx);
  if (index < 0) return;
  const std::string &entity_id = ctx->halves[index].entity_id;
  if (!entity_id.empty()) send_toggle_action(entity_id);
}
