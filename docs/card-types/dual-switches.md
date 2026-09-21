---
title: Dual Switch Cards
description:
  How to use dual switch cards on your EspControl panel to control two Home Assistant entities from one card.
---

# Dual Switch

A Dual Switch card controls two Home Assistant entities from one card. Use it to fit more switches on a page, or to keep related switches together, such as a ceiling light and a lamp in the same room.

Each switch has a round icon badge and a name. The badge fills with the on colour while Home Assistant reports that entity as active.

## Setting Up a Dual Switch Card

1. Add a new card and select **Dual Switch**.
2. Choose a **Layout**: **Top and Bottom** stacks the two switches, and **Side by Side** places them next to each other.
3. For each switch, enter an **Entity** - the Home Assistant entity you want to control, for example `light.kitchen` or `switch.coffee_maker`.
4. Set a **Label** if you want custom text. If left blank, the friendly name from Home Assistant is used.
5. Choose an **Icon**, or leave it as **Auto** so the panel picks an icon from the entity type.

**Top and Bottom** suits single cards and wide cards. **Side by Side** has more room on a wide card; on a single card, keep the labels short.

## How It Works on the Panel

- Tapping one half of the card sends a Home Assistant toggle action for that half's entity. The other switch is not changed.
- The icon badge lights up when Home Assistant reports an active state such as `on`, `open`, `playing`, or `home`.
- A switch whose entity is unavailable in Home Assistant is shown faded.
- If an entity is changed somewhere else, such as in Home Assistant or by an automation, the card updates to match.
- Dual Switch cards can be used on subpages. A subpage card shows as active while either switch is on.

Use a [Switch](/card-types/switches) card if you need **Confirmation Required**, **Active Display**, or separate on and off icons.

::: info Requires Home Assistant actions
Dual Switch cards send Home Assistant actions from the panel. If tapping a card does nothing, check [Enable Actions](/getting-started/home-assistant-actions).
:::
