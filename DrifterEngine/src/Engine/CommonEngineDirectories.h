#pragma once


static const std::filesystem::path WORKING_DIRECTORY = std::filesystem::current_path();

static const std::filesystem::path RESOURCES_DIRECTORY = WORKING_DIRECTORY / "resources";

static const std::filesystem::path TEXTURES_DIRECTORY = RESOURCES_DIRECTORY / "Textures";
static const std::filesystem::path FONTS_DIRECTORY = RESOURCES_DIRECTORY / "Fonts";
static const std::filesystem::path ICONS_DIRECTORY = RESOURCES_DIRECTORY / "Icons";
static const std::filesystem::path THEMES_DIRECTORY = RESOURCES_DIRECTORY / "GUI" / "Themes";

static const std::filesystem::path SAVE_DIRECTORY = WORKING_DIRECTORY / "data" / "savegame";
static const std::filesystem::path STATIC_DATA_DIRECTORY = WORKING_DIRECTORY / "data" / "static";
static const std::filesystem::path SETTINGS_DIRECTORY = WORKING_DIRECTORY / "data" / "settings";
static const std::filesystem::path SAVED_SETTINGS_DIRECTORY = SETTINGS_DIRECTORY / "saved";
static const std::filesystem::path DEFAULT_SETTINGS_DIRECTORY = SETTINGS_DIRECTORY / "default";

static const std::filesystem::path PLAYER_SAVE_FILE_PATH = SAVE_DIRECTORY / "player_save.json";
static const std::filesystem::path GAMESTATE_SAVE_FILE_PATH = SAVE_DIRECTORY / "gamestate.json";
static const std::filesystem::path MAIN_REGISTRY_FILE_PATH = SAVE_DIRECTORY / "registry.json";
static const std::filesystem::path REGION_DIRECTORY = SAVE_DIRECTORY / "regions";

static const std::filesystem::path ENTITIES_DIRECTORY = STATIC_DATA_DIRECTORY / "entities";
static const std::filesystem::path UNIVERSE_DIRECTORY = STATIC_DATA_DIRECTORY / "universe";
static const std::filesystem::path SOLAR_SYSTEM_FILE_PATH = STATIC_DATA_DIRECTORY / "solar_system.json";