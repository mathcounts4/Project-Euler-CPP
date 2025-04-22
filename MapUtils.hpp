#pragma once

template<class Map, class Key>
auto findOrNull(Map&& map, Key&& key) {
    using T = decltype(&map.find(std::forward<Key>(key))->second);
    if (auto it = map.find(std::forward<Key>(key)); it != map.end()) {
	return &it->second;
    } else {
	return static_cast<T>(nullptr);
    }
}

template<class Map, class Key, class ValueCreator>
decltype(auto) getOrInsert(Map& map, Key&& key, ValueCreator&& valueCreator) {
    if (auto valPtr = findOrNull(map, key)) {
	return (*valPtr);
    } else {
	return (map.emplace(std::forward<Key>(key), valueCreator()).first->second);
    }
}

