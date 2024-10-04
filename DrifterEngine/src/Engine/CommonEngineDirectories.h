#pragma once


static const std::filesystem::path WORKING_DIRECTORY = std::filesystem::current_path();

static const std::filesystem::path RESOURCES_DIRECTORY = WORKING_DIRECTORY / "resources";

static const std::filesystem::path TEXTURES_DIRECTORY = RESOURCES_DIRECTORY / "Textures";
static const std::filesystem::path FONTS_DIRECTORY = RESOURCES_DIRECTORY / "Fonts";
static const std::filesystem::path ICONS_DIRECTORY = RESOURCES_DIRECTORY / "Icons";
static const std::filesystem::path THEMES_DIRECTORY = RESOURCES_DIRECTORY / "GUI" / "Themes";

static const std::filesystem::path SAVE_DIRECTORY = WORKING_DIRECTORY / "data" / "savegame";
static const std::filesystem::path STATIC_DATA_DIRECTORY = WORKING_DIRECTORY / "data" / "static";

static const std::filesystem::path PLAYER_SAVE_FILE_PATH = SAVE_DIRECTORY / "player_save.json";
static const std::filesystem::path GAMESTATE_SAVE_FILE_PATH = SAVE_DIRECTORY / "gamestate.json";
static const std::filesystem::path MAIN_REGISTRY_FILE_PATH = SAVE_DIRECTORY / "registry.json";
static const std::filesystem::path CHUNK_DIRECTORY = SAVE_DIRECTORY / "chunks";

static const std::filesystem::path ENTITIES_DIRECTORY = STATIC_DATA_DIRECTORY / "entities";
static const std::filesystem::path WORLD_GENERATION_FILE_PATH = STATIC_DATA_DIRECTORY / "world_generation.json";