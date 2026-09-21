import type { DeviceConfig } from "../state/types";
import {
    cardContractOptionSpec,
    cardContractOptionSupportedFor,
} from "./config_option_core";

// Days shown beside the current conditions on a Daily Forecast card that spans
// `colSpan` columns. Matches weather_days_visible_for_columns in the firmware.
export function weatherDaysVisibleForColumns(colSpan: number): number {
    if (colSpan <= 1) return 0;
    if (colSpan === 2) return 2;
    if (colSpan === 3) return 4;
    return Math.min(colSpan + 1, 6);
}

export function createConfigWeatherOptionsFeature(
    deviceProfile: Pick<DeviceConfig, "disabledCardTypes">,
) {
    function weatherForecastCardsSupported(this: any) {
        const disabled: readonly string[] = deviceProfile.disabledCardTypes || [];
        return disabled.indexOf("weather_forecast") === -1;
    }

    function weatherModeOptions(this: any) {
        const options: any = [
            ["", "Current Conditions"],
            ["today", "Temperatures Today"],
            ["tomorrow", "Temperatures Tomorrow"],
            ["days", "Daily Forecast"],
        ];
        return weatherForecastCardsSupported() ? options : [options[0]];
    }

    function weatherModeOptionValues(this: any) {
        const spec: any = cardContractOptionSpec("weather", "weather_mode");
        const values: any = spec && spec.values ? spec.values.slice() : ["", "today", "tomorrow"];
        return weatherForecastCardsSupported() ? values : values.filter(function (value: any) {
            return value === "";
        });
    }

    function normalizeWeatherCardMode(this: any, mode?: any) {
        mode = String(mode || "");
        return weatherModeOptionValues().indexOf(mode) >= 0 ? mode : "";
    }

    function weatherCardIsForecastMode(this: any, button?: any) {
        return weatherForecastCardsSupported() &&
            !!button &&
            cardContractOptionSupportedFor("weather", "large_numbers", { precision: button.precision });
    }

    function weatherCardIsDaysMode(this: any, button?: any) {
        return weatherForecastCardsSupported() && !!button && button.precision === "days";
    }

    function weatherCardDefaultForecastLabel(this: any, button?: any) {
        return button.precision === "today" ? "Today" : "Tomorrow";
    }

    return {
        weatherCardDefaultForecastLabel,
        weatherCardIsDaysMode,
        weatherCardIsForecastMode,
        weatherForecastCardsSupported,
        weatherModeOptions,
        weatherModeOptionValues,
        normalizeWeatherCardMode,
    };
}

export type ConfigWeatherOptionsFeature = ReturnType<typeof createConfigWeatherOptionsFeature>;
