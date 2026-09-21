import {
    cardContractAllowInSubpage,
    cardContractCard,
    cardContractCardLabel,
    cardContractDefaultConfig,
    cardContractDomains,
    cardContractHidden,
    cardContractPickerKey,
} from "../generated/card_contract";
import { CARD_SIZE_SINGLE, sizeColSpan } from "../model/grid";
import type { CardRegistry, CardUiServices } from "../application/card_registry";
import { weatherDaysVisibleForColumns, type ConfigWeatherOptionsFeature } from "../application/config_weather_options";
import type { ClockBarFeature } from "../application/clock_bar_state";
import type { ControlsFieldsFeature } from "../application/controls_fields";

export interface WeatherCardRegistration {
    readonly entityMetadata: any;
    readonly previewMetadata: any;
}

export function registerWeatherCardTypes(
    registry: CardRegistry,
    weatherOptions: ConfigWeatherOptionsFeature,
    clockBar: Pick<ClockBarFeature, "temperatureUnitSymbol">,
    fields: ControlsFieldsFeature,
    cardUi: CardUiServices,
): WeatherCardRegistration {
    const { renderButtonSettings } = cardUi;
    const { cardBadgeLabelHtml, cardSensorPreviewHtml } = fields;
    const { temperatureUnitSymbol } = clockBar;
    const {
        weatherCardDefaultForecastLabel,
        weatherCardIsDaysMode,
        weatherCardIsForecastMode,
        weatherModeOptions,
        normalizeWeatherCardMode,
    } = weatherOptions;
    // Read-only weather card: displays either current conditions or high / low temperatures.
    const WEATHER_CARD_METADATA: any = {
        mode: {
            label: "Type",
            idSuffix: "weather-display",
            options: weatherModeOptions,
            value: function (this: any, b?: any) {
                return weatherCardIsForecastMode(b) ? b.precision : "";
            },
            onChange: function (this: any, b?: any, helpers?: any) {
                b.precision = this.value;
                helpers.saveField("precision", b.precision);
                renderButtonSettings();
            },
        },
        entity: {
            label: "Weather Entity",
            idSuffix: "entity",
            placeholder: "e.g. weather.forecast_home",
            domains: function (this: any) { return cardContractDomains("weather"); },
            bindName: "entity",
            rerender: true,
            requiredMessage: "Add an entity before saving.",
        },
        labelField: {
            label: "Label",
            idSuffix: "label",
            placeholder: function (this: any, b?: any) {
                return "e.g. " + weatherCardDefaultForecastLabel(b);
            },
        },
        largeNumbers: {
            label: "Large Temperature Numbers",
            idSuffix: "large-weather-numbers",
            supported: weatherCardIsForecastMode,
        },
        preview: {
            forecastBadge: "weather-partly-cloudy",
            currentBadge: "weather-cloudy",
        },
    };
    // Sample days for the Daily Forecast preview.
    const PREVIEW_DAYS: readonly [string, string, string][] = [
        ["Tue", "weather-sunny", "17\u00B0/14\u00B0"],
        ["Wed", "weather-partly-cloudy", "17\u00B0/14\u00B0"],
        ["Thu", "weather-cloudy", "15\u00B0/14\u00B0"],
        ["Fri", "weather-partly-cloudy", "15\u00B0/14\u00B0"],
        ["Sat", "weather-rainy", "14\u00B0/11\u00B0"],
        ["Sun", "weather-pouring", "13\u00B0/10\u00B0"],
    ];
    function dailyForecastPreviewHtml(this: any, cardSize?: any) {
        const days = weatherDaysVisibleForColumns(sizeColSpan(cardSize || CARD_SIZE_SINGLE));
        const columns = PREVIEW_DAYS.slice(0, days).map(function (day) {
            return '<div class="sp-weather-days-col">' +
                '<span class="sp-weather-days-text sp-weather-days-dim">' + day[0] + '</span>' +
                '<span class="sp-weather-days-icon mdi mdi-' + day[1] + '"></span>' +
                '<span class="sp-weather-days-text">' + day[2] + '</span></div>';
        }).join("");
        return '<div class="sp-weather-days' + (days ? "" : " sp-weather-days-single") + '">' +
            '<div class="sp-weather-days-now">' +
            '<div class="sp-weather-days-now-row"><span class="sp-weather-days-icon mdi mdi-weather-cloudy"></span>' +
            '<span class="sp-weather-days-temp">16\u00B0</span></div>' +
            '<span class="sp-weather-days-text">Cloudy</span>' +
            '<span class="sp-weather-days-text sp-weather-days-dim">17\u00B0/13\u00B0</span></div>' +
            columns +
            '</div>';
    }
    registry.register("weather", {
        label: function (this: any) { return cardContractCardLabel("weather"); },
        allowInSubpage: function (this: any) { return cardContractAllowInSubpage("weather"); },
        pickerKey: function (this: any) { return cardContractPickerKey("weather"); },
        hidden: function (this: any) { return cardContractHidden("weather"); },
        hideLabel: true,
        defaultConfig: function (this: any) { return cardContractDefaultConfig("weather"); },
        cardMetadata: WEATHER_CARD_METADATA,
        onSelect: function (this: any, b?: any) {
            b.label = "";
            b.icon = "Auto";
            b.icon_on = "Auto";
            b.sensor = "";
            b.unit = "";
            b.options = "";
            b.precision = normalizeWeatherCardMode(b.precision);
        },
        renderSettings: function (this: any, panel?: any, b?: any, slot?: any, helpers?: any) {
            helpers.renderCardModeSelector(panel, b, helpers, WEATHER_CARD_METADATA);
            helpers.renderCardEntityField(panel, b, helpers, WEATHER_CARD_METADATA);
            if (!weatherCardIsForecastMode(b))
                return;
            var labelControl: any = helpers.renderCardTextField(panel, b, helpers, WEATHER_CARD_METADATA.labelField);
            var labelInp: any = labelControl.input;
            labelInp.placeholder = "e.g. " + weatherCardDefaultForecastLabel(b);
            helpers.renderCardLargeNumbersToggle(panel, b, helpers, WEATHER_CARD_METADATA);
        },
        renderPreview: function (this: any, b?: any, helpers?: any) {
            if (weatherCardIsDaysMode(b)) {
                return {
                    buttonClass: "sp-weather-days-card",
                    iconHtml: dailyForecastPreviewHtml(helpers && helpers.cardSize),
                    labelHtml: "",
                };
            }
            if (weatherCardIsForecastMode(b)) {
                var defaultLabel: any = weatherCardDefaultForecastLabel(b);
                var label: any = b.label || defaultLabel;
                return {
                    iconHtml: cardSensorPreviewHtml(b, helpers, "18/10", temperatureUnitSymbol(), "sp-forecast-preview", "sp-forecast-value"),
                    labelHtml: cardBadgeLabelHtml(helpers, label, WEATHER_CARD_METADATA.preview.forecastBadge),
                };
            }
            return {
                iconHtml: '<span class="sp-btn-icon mdi mdi-weather-cloudy"></span>',
                labelHtml: cardBadgeLabelHtml(helpers, "Cloudy", WEATHER_CARD_METADATA.preview.currentBadge),
            };
        },
    });
    return {
        entityMetadata: WEATHER_CARD_METADATA.entity,
        previewMetadata: WEATHER_CARD_METADATA.preview,
    };
}
