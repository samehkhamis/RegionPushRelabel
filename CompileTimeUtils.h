/////////////////////////////////////////////////////////////////////////////
// Filename: CompileTimeUtils.h
// Author:   Sameh Khamis
//
// Description: Template metaprogramming helpers, mostly for the layout
//              descriptor, that evaluate at compile time
/////////////////////////////////////////////////////////////////////////////
#ifndef _COMPILE_TIME_UTILS
#define _COMPILE_TIME_UTILS

#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/count.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/placeholders.hpp>
namespace mp = boost::mpl;

// Arc and helpers
template <ptrdiff_t iFrom, ptrdiff_t iTo, typename iOffset>
struct Arc
{
	static const ptrdiff_t From = iFrom;
	static const ptrdiff_t To = iTo;
	typedef iOffset Offset;
};

struct CollectIntegers
{
	CollectIntegers(ptrdiff_t arr[]) { integers = arr; idx = 0; }
	template <typename T>
	void operator()(T val)
	{
		integers[idx] = val;
		idx++;
	}
private:
	ptrdiff_t* integers;
	size_t idx;
};

struct CollectEdge
{
	CollectEdge(vector<ptrdiff_t>* arr) { offsets = arr; d = 1; }
	template <typename Offset>
	void operator()(Offset val)
	{
		offsets[d].push_back(val);
		d++;
	}
private:
	vector<ptrdiff_t>* offsets;
	size_t d;
};

struct CollectOffsets
{
	CollectOffsets(vector<ptrdiff_t>* arr, vector<size_t>* t, size_t n) { offsets = arr; tos = t; ndim = n; }
	template <ptrdiff_t From, ptrdiff_t To, typename Offset>
	void operator()(Arc<From, To, Offset>&)
	{
		tos[From].push_back(To);
		offsets[From * ndim].push_back(To - From);
		mp::for_each<Offset>(CollectEdge(&offsets[From * ndim]));
	}
private:
	vector<ptrdiff_t>* offsets;
	vector<size_t>* tos;
	size_t ndim;
};


// Collect from from offsets
template <typename Froms, typename Offset>
struct push_from :
	public mp::push_back<Froms, mp::int_<Offset::From> >::type {};

template <typename Offsets>
struct all_from :
	public mp::fold<
		Offsets,
		mp::vector_c<int>,
		push_from<mp::_1, mp::_2> >::type {};

// Product of mpl vector of any size
template <typename Numbers>
struct product :
	public mp::fold<
		Numbers,
		mp::int_<1>,
		mp::times<mp::_1, mp::_2> >::type {};

// Sum of mpl vector of any size
template <typename Numbers>
struct sum :
	public mp::fold<
		Numbers,
		mp::int_<0>,
		mp::plus<mp::_1, mp::_2> >::type {};

template <size_t N, size_t Base>
struct log_n :
	public mp::plus<mp::int_<1>, typename log_n<N / Base, Base>::type> {};
template <size_t Base>
struct log_n<1, Base> :
	public mp::int_<0>::type {};
template <size_t Base>
struct log_n<0, Base> :
	public mp::int_<0>::type {};

template <size_t N, size_t Power>
struct pow_n :
	public mp::times<mp::int_<N>,  pow_n<N, Power - 1> >::type {};
template <size_t N>
struct pow_n<N, 0> :
	public mp::int_<1>::type {};

template <size_t A, size_t B>
struct max_of :
	public mp::if_<mp::greater<mp::int_<A>, mp::int_<B> >, mp::int_<A>, mp::int_<B> >::type {};


// Count of a specific number in a vector
template <typename Numbers, typename Num>
struct equal_count :
	public mp::count_if<Numbers, mp::equal_to<mp::_1, mp::int_<Num::value> > >::type {};

template <typename Numbers>
struct all_equal_count :
	public mp::fold<
		mp::range_c<int, 0, mp::max_element<Numbers>::type::type::value + 1>,
		mp::vector_c<int>,
		mp::push_back<mp::_1, equal_count<Numbers, mp::_2> > >::type {};


// Static assert helper
template <typename Dimensions, typename BlockDimensions>
struct must_be_true :
	public mp::equal_to<
		typename sum<
			typename mp::transform<Dimensions, BlockDimensions, mp::modulus<mp::_1, mp::_2> >::type>::type,
		mp::int_<0> >::type {};


// Hiding mpl from the interface
class ThreadCountTag {};
template <size_t X> class ThreadCount : public mp::int_<X>, public ThreadCountTag {};
class MaxBlocksPerRegionTag {};
template <size_t X> class MaxBlocksPerRegion : public mp::int_<X>, public MaxBlocksPerRegionTag {};
class DischargesPerBlockTag {};
template <size_t X> class DischargesPerBlock : public mp::int_<X>, public DischargesPerBlockTag {};
class BucketDensityTag {};
template <size_t X> class BucketDensity : public mp::int_<X>, public BucketDensityTag {};
class BlocksPerMemoryPageTag {};
template <size_t X> class BlocksPerMemoryPage : public mp::int_<X>, public BlocksPerMemoryPageTag {};
class GlobalUpdateFrequencyTag {};
template <size_t X> class GlobalUpdateFrequency : public mp::int_<X>, public GlobalUpdateFrequencyTag {};

class OffsetsTag {};

template <ptrdiff_t C1 = LONG_MAX, ptrdiff_t C2 = LONG_MAX, ptrdiff_t C3 = LONG_MAX, ptrdiff_t C4 = LONG_MAX,
	ptrdiff_t C5 = LONG_MAX, ptrdiff_t C6 = LONG_MAX, ptrdiff_t C7 = LONG_MAX, ptrdiff_t C8 = LONG_MAX,
	ptrdiff_t C9 = LONG_MAX, ptrdiff_t C10 = LONG_MAX, ptrdiff_t C11 = LONG_MAX, ptrdiff_t C12 = LONG_MAX,
	ptrdiff_t C13 = LONG_MAX, ptrdiff_t C14 = LONG_MAX, ptrdiff_t C15 = LONG_MAX, ptrdiff_t C16 = LONG_MAX,
	ptrdiff_t C17 = LONG_MAX, ptrdiff_t C18 = LONG_MAX, ptrdiff_t C19 = LONG_MAX, ptrdiff_t C20 = LONG_MAX>
class Offsets :
	public mp::vector_c<ptrdiff_t, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20>, public OffsetsTag {};

class DimensionsTag {};

template <ptrdiff_t C1 = LONG_MAX, ptrdiff_t C2 = LONG_MAX, ptrdiff_t C3 = LONG_MAX, ptrdiff_t C4 = LONG_MAX,
	ptrdiff_t C5 = LONG_MAX, ptrdiff_t C6 = LONG_MAX, ptrdiff_t C7 = LONG_MAX, ptrdiff_t C8 = LONG_MAX,
	ptrdiff_t C9 = LONG_MAX, ptrdiff_t C10 = LONG_MAX, ptrdiff_t C11 = LONG_MAX, ptrdiff_t C12 = LONG_MAX,
	ptrdiff_t C13 = LONG_MAX, ptrdiff_t C14 = LONG_MAX, ptrdiff_t C15 = LONG_MAX, ptrdiff_t C16 = LONG_MAX,
	ptrdiff_t C17 = LONG_MAX, ptrdiff_t C18 = LONG_MAX, ptrdiff_t C19 = LONG_MAX, ptrdiff_t C20 = LONG_MAX>
class Dimensions :
	public mp::vector_c<ptrdiff_t, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20>, public DimensionsTag {};

class BlockDimensionsTag {};

template <ptrdiff_t C1 = LONG_MAX, ptrdiff_t C2 = LONG_MAX, ptrdiff_t C3 = LONG_MAX, ptrdiff_t C4 = LONG_MAX,
	ptrdiff_t C5 = LONG_MAX, ptrdiff_t C6 = LONG_MAX, ptrdiff_t C7 = LONG_MAX, ptrdiff_t C8 = LONG_MAX,
	ptrdiff_t C9 = LONG_MAX, ptrdiff_t C10 = LONG_MAX, ptrdiff_t C11 = LONG_MAX, ptrdiff_t C12 = LONG_MAX,
	ptrdiff_t C13 = LONG_MAX, ptrdiff_t C14 = LONG_MAX, ptrdiff_t C15 = LONG_MAX, ptrdiff_t C16 = LONG_MAX,
	ptrdiff_t C17 = LONG_MAX, ptrdiff_t C18 = LONG_MAX, ptrdiff_t C19 = LONG_MAX, ptrdiff_t C20 = LONG_MAX>
class BlockDimensions :
	public mp::vector_c<ptrdiff_t, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20>, public BlockDimensionsTag {};

template <typename C1 = mp::na, typename C2 = mp::na, typename C3 = mp::na, typename C4 = mp::na,
	typename C5 = mp::na, typename C6 = mp::na, typename C7 = mp::na, typename C8 = mp::na,
	typename C9 = mp::na, typename C10 = mp::na, typename C11 = mp::na, typename C12 = mp::na,
	typename C13 = mp::na, typename C14 = mp::na, typename C15 = mp::na, typename C16 = mp::na,
	typename C17 = mp::na, typename C18 = mp::na, typename C19 = mp::na, typename C20 = mp::na>
class Array :
	public mp::vector<C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20> {};

#endif
