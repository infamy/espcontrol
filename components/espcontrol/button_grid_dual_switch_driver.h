#pragma once

// Shared main-grid/subpage lifecycle. DualSwitchCtx belongs to a container
// inside the card, so the grid's rebuild and subpage teardown release it with
// its Home Assistant callbacks.

namespace espcontrol::cards {
inline bool dual_switch_driver_matches(const Context &context) {
  return context.runtime.driver == card_runtime::CardDriverId::DUAL_SWITCH;
}

inline bool dual_switch_driver_setup_visual(
    BtnSlot &slot, const ParsedCfg &config, const Context &context,
    const CardPalette &palette) {
  if (!dual_switch_driver_matches(context)) return false;
  setup_dual_switch_card(
    slot, config, palette.has_on ? palette.on_val : DEFAULT_SLIDER_COLOR);
  return true;
}

inline bool dual_switch_driver_handle_main_click(
    const Context &context, lv_obj_t *button) {
  if (!dual_switch_driver_matches(context)) return false;
  dual_switch_handle_click(button);
  return true;
}

inline bool dual_switch_driver_bind_data(
    BtnSlot &slot, const ParsedCfg &config, const Context &context,
    std::function<void(const std::string &)> add_parent_indicator = {}) {
  if (!dual_switch_driver_matches(context)) return false;
  DualSwitchCtx *card = dual_switch_card_for(slot.btn);
  if (!card) return true;
  dual_switch_subscribe(card, config);
  if (context.surface == Surface::SUBPAGE) {
    for (const DualSwitchHalf &half : card->halves) {
      if (add_parent_indicator && !half.entity_id.empty()) {
        add_parent_indicator(half.entity_id);
      }
    }
    lv_obj_add_event_cb(slot.btn, [](lv_event_t *event) {
      dual_switch_handle_press(static_cast<lv_obj_t *>(lv_event_get_current_target(event)));
    }, LV_EVENT_PRESSED, nullptr);
    lv_obj_add_event_cb(slot.btn, [](lv_event_t *event) {
      dual_switch_handle_click(static_cast<lv_obj_t *>(lv_event_get_current_target(event)));
    }, LV_EVENT_CLICKED, nullptr);
  }
  return true;
}
}  // namespace espcontrol::cards
