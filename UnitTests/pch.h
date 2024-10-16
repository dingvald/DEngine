// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#pragma warning(push, 0)

#include <cstdlib>
#include <utility>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <stack>
#include <functional>
#include <queue>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <optional>
#include <type_traits>
#include <typeindex>
#include <string>
#include <typeinfo>
#include <bitset>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <chrono>
#include <cassert>
#include <variant>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// TGUI
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

// EnTT
#include <EnTT/entt.h>

// Cereal
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_set.hpp>
#include <Cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/variant.hpp>

#pragma warning(pop)

#endif //PCH_H
