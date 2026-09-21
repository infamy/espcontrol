#!/usr/bin/env python3
"""Check how the Camera screensaver mode is wired into the firmware YAML."""
from pathlib import Path
import yaml

ROOT = Path(__file__).resolve().parents[2]


class Loader(yaml.SafeLoader):
    pass


def construct_tagged(loader, _suffix, node):
    if isinstance(node, yaml.MappingNode):
        return loader.construct_mapping(node)
    if isinstance(node, yaml.SequenceNode):
        return loader.construct_sequence(node)
    return loader.construct_scalar(node)


Loader.add_multi_constructor("!", construct_tagged)


def load(relative):
    return yaml.load((ROOT / relative).read_text(), Loader)


def script(config, script_id):
    return next(item for item in config["script"] if item["id"] == script_id)


def strings(value):
    """All text inside a parsed YAML value, such as the lambdas in a script."""
    if isinstance(value, str):
        yield value
    elif isinstance(value, dict):
        for item in value.values():
            yield from strings(item)
    elif isinstance(value, list):
        for item in value:
            yield from strings(item)


def check_camera_addon():
    addon = load("common/addon/camera_motion.yaml")
    wake = addon["camera_motion"]["on_motion"][0]["if"]
    condition = wake["condition"]["lambda"]
    assert 'id(screensaver_mode).state == "camera"' in condition, "motion must only wake in Camera mode"
    assert "presence_can_wake_display" in condition, "motion must follow presence wake rules"
    assert {"script.execute": "screensaver_wake"} in wake["then"], "motion must use the normal wake path"

    run = addon["interval"][0]["then"][0]["lambda"]
    assert 'id(screensaver_mode).state == "camera" && screen_asleep' in run, (
        "the camera must only run for Camera mode while the screen is asleep")
    assert "id(camera_motion_test_mode).state" in run, "test mode must keep the camera running"

    sensitivity = next(item for item in addon["number"] if item["id"] == "camera_motion_sensitivity")
    assert (sensitivity["min_value"], sensitivity["max_value"], sensitivity["step"]) == (1, 100, 1), (
        "sensitivity must run from 1 to 100")
    assert sensitivity["internal"] is True, "sensitivity is a panel setting, not a Home Assistant control"
    assert sensitivity["name"] == "${entity_camera_motion_sensitivity}", "sensitivity must use the shared name"

    test_mode = next(item for item in addon["switch"] if item["id"] == "camera_motion_test_mode")
    assert test_mode["restore_mode"] == "ALWAYS_OFF", "test mode must not survive a restart"


def check_camera_mode_sleeps_like_timer():
    backlight = load("common/addon/backlight.yaml")
    idle = script(backlight, "screensaver_idle_check")["then"][0]["if"]["condition"]["lambda"]
    for mode in ("timer", "sensor", "camera"):
        assert f'id(screensaver_mode).state == "{mode}"' in idle, f"{mode} mode must start the idle timer"

    wake = "\n".join(strings(script(backlight, "screensaver_wake")))
    assert 'id(screensaver_mode).state != "camera"' in wake, (
        "cover art must treat Camera mode as an active screensaver after wake")

    cover_art = (ROOT / "common/device/screen_cover_art.yaml").read_text()
    assert 'id(screensaver_mode).state != "camera") ||' in cover_art, (
        "cover art playback must treat Camera mode as an active screensaver")


def check_only_camera_panels_include_addon():
    for packages in sorted((ROOT / "devices").glob("*/packages.yaml")):
        included = "common/addon/camera_motion.yaml" in packages.read_text()
        expected = packages.parent.name == "guition-esp32-p4-jc1060p470-v2"
        assert included == expected, f"{packages.relative_to(ROOT)}: camera add-on inclusion is {included}"


def main():
    check_camera_addon()
    check_camera_mode_sleeps_like_timer()
    check_only_camera_panels_include_addon()
    print("Camera motion wiring checks passed.")


if __name__ == "__main__":
    main()
