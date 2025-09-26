#pragma once

/*
std::size_t seed = 0;
HashCombine(seed, h1, h2, h3);
*/
inline void HashCombine([[maybe_unused]] std::size_t& seed) noexcept {}
template <typename T, typename... Rest>
inline void HashCombine(std::size_t& seed, const T& v, Rest... rest) noexcept
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	HashCombine(seed, rest...);
}