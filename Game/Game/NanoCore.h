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

// Recursive template code derived from Matthieu M.
template<class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl
{
	static void Apply(size_t& seed, const Tuple& tuple)
	{
		HashValueImpl<Tuple, Index - 1>::Apply(seed, tuple);
		HashCombine(seed, std::get<Index>(tuple));
	}
};

template<class Tuple>
struct HashValueImpl<Tuple, 0>
{
	static void Apply(size_t& seed, const Tuple& tuple)
	{
		HashCombine(seed, std::get<0>(tuple));
	}
};

template<typename T>
struct Hash;
template<typename... TT>
struct Hash<std::tuple<TT...>>
{
	size_t operator()(const std::tuple<TT...>& tt) const
	{
		size_t seed = 0;
		HashValueImpl<std::tuple<TT...>>::Apply(seed, tt);
		return seed;
	}
};