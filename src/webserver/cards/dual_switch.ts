import type { CardRegistry } from "../application/card_registry";
import { domainIcons, iconSlug } from "../application/ui_primitives";
import {
  cardContractAllowInSubpage,
  cardContractCardLabel,
  cardContractDefaultConfig,
  cardContractDomains,
} from "../generated/card_contract";

// Dual Switch card: two switches in one card, stacked or side by side.
// Storage: entity/label/icon = top or left switch, sensor/unit/icon_on =
// bottom or right switch, precision = layout ("" stacked, "side").

export const DUAL_SWITCH_SIDE_LAYOUT = "side";

export function dualSwitchSideBySide(b: { precision?: string } | null | undefined): boolean {
  return !!b && b.precision === DUAL_SWITCH_SIDE_LAYOUT;
}

function dualSwitchIconSlug(icon: string | undefined, entity: string | undefined): string {
  if (icon && icon !== "Auto") return iconSlug(icon);
  const domain = String(entity || "").split(".")[0] || "";
  return domainIcons[domain] || "toggle-switch-variant";
}

function dualSwitchSwitchNames(side: boolean): [string, string] {
  return side ? ["Left Switch", "Right Switch"] : ["Top Switch", "Bottom Switch"];
}

export function dualSwitchPreviewHtml(b: any, escHtml: (value: unknown) => string): string {
  const halves = [
    { entity: b.entity, label: b.label, icon: b.icon },
    { entity: b.sensor, label: b.unit, icon: b.icon_on },
  ];
  return '<span class="sp-dual-switch' + (dualSwitchSideBySide(b) ? " sp-dual-switch-side" : "") + '">' +
    halves.map(function (half) {
      return '<span class="sp-dual-switch-half">' +
        '<span class="sp-dual-switch-badge"><span class="mdi mdi-' + dualSwitchIconSlug(half.icon, half.entity) + '"></span></span>' +
        '<span class="sp-dual-switch-name">' + escHtml(half.label || half.entity || "Configure") + '</span>' +
        '</span>';
    }).join("") +
    '</span>';
}

export function registerDualSwitchCardTypes(registry: CardRegistry): void {
registry.register("dual_switch", {
  label: () => cardContractCardLabel("dual_switch"),
  allowInSubpage: () => cardContractAllowInSubpage("dual_switch"),
  hideLabel: true,
  cardMetadata: { preview: { badge: "toggle-switch-variant" } },
  defaultConfig: () => cardContractDefaultConfig("dual_switch"),
  onSelect: function (b) {
    b.entity = "";
    b.label = "";
    b.icon = "Auto";
    b.icon_on = "Auto";
    b.sensor = "";
    b.unit = "";
    b.precision = "";
    b.options = "";
  },
  renderSettings: function (panel, b, _slot, helpers) {
    const domains = cardContractDomains("dual_switch");
    const sections: any[] = [];

    function renameSections() {
      const names = dualSwitchSwitchNames(dualSwitchSideBySide(b));
      sections.forEach(function (section, index) {
        section.button.firstChild.textContent = names[index];
      });
    }

    helpers.renderCardSegmentControl(panel, b, helpers, {
      label: "Layout",
      options: [
        ["stacked", "Top and Bottom"],
        ["side", "Side by Side"],
      ],
      value: function () { return dualSwitchSideBySide(b) ? "side" : "stacked"; },
      onSelect: function (_b: any, _helpers: any, value: string) {
        b.precision = value === "side" ? DUAL_SWITCH_SIDE_LAYOUT : "";
        helpers.saveField("precision", b.precision);
        renameSections();
      },
    });

    const names = dualSwitchSwitchNames(dualSwitchSideBySide(b));
    const fields = [
      { entity: "entity", label: "label", icon: "icon", placeholder: "e.g. light.kitchen", name: "e.g. Kitchen" },
      { entity: "sensor", label: "unit", icon: "icon_on", placeholder: "e.g. light.dining", name: "e.g. Dining" },
    ];
    fields.forEach(function (field, index) {
      const section = helpers.disclosureSection(names[index], helpers.idPrefix + "dual-switch-" + (index + 1), true);
      sections.push(section);
      panel.appendChild(section.panel);
      const entityField = helpers.entityField(
        "Entity", helpers.idPrefix + field.entity, b[field.entity] || "", field.placeholder, domains,
        field.entity, true, "Add an entity for each switch before saving.");
      section.section.appendChild(entityField.field);
      if (index === 0) helpers.markCardPrimaryField(entityField.field, "entity");
      helpers.renderCardTextField(section.section, b, helpers, {
        label: "Label",
        idSuffix: field.label,
        placeholder: field.name,
        bindName: field.label,
      });
      helpers.renderCardIconPicker(section.section, b, helpers, {
        field: field.icon,
        fallback: "Auto",
        label: "Icon",
      });
    });
  },
  renderPreview: function (b, helpers) {
    return {
      buttonClass: "sp-btn-dual-switch",
      iconHtml: dualSwitchPreviewHtml(b, helpers.escHtml),
      labelHtml: "",
    };
  },
});
}
