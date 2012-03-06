/////////////////////////////////////////////////////////////////////////////
// Filename: Layout.tpl
// Author:   Sameh Khamis
//
// Description: Layout descriptor for the Regular Push-Relabel algorithm,
//              implemented using template metaprogramming
/////////////////////////////////////////////////////////////////////////////
#ifndef _LAYOUT
#define _LAYOUT

#ifdef _MSC_VER
  #define INLINE __forceinline	// VC++
#else
  #define INLINE inline			// GCC
#endif

#include <set>
#include <vector>
using namespace std;

#include "CompileTimeUtils.h"

// Use Array to initialize an array of arcs, which just wraps mpl::vector
// If you need more than 20 arcs, you will have to directly use mpl::vectorN instead of Array,
// where N is the number of arcs you need. You will also need to include "boost/mpl/vector/vectorM.hpp"
// where M is N rounded up to the next ten (e.g, vector30.hpp to use the vector24 template)

class LayoutTag {};

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
class Layout : LayoutTag
{
public:
	// Type definitions
	typedef OffsetVector OffsetVector_;
	typedef all_equal_count<all_from<OffsetVector> > FromCountVector;

	// Static assert: block dimensions must divide dimensions
	BOOST_STATIC_ASSERT((must_be_true<Dimensions, BlockDimensions>::value));

	// Constants
	static const size_t DIM_COUNT = mpl::size<typename mpl::at_c<OffsetVector, 0>::type::Offset>::value + 1;
	static const size_t NODE_EDGE_COUNT = mpl::max_element<FromCountVector>::type::type::value;
	static const size_t NODES_PER_CELL = mpl::size<FromCountVector>::value;
	static const size_t NODE_COUNT = NODES_PER_CELL * product<Dimensions>::value;
	static const size_t NODES_PER_BLOCK = NODES_PER_CELL * product<BlockDimensions>::value;

	// Coord struct to convert from coords into indices and back
	struct Coord
	{
		size_t value[DIM_COUNT];
		const size_t& operator[](const size_t d) const { return value[d]; }
		size_t& operator[](const size_t d) { return value[d]; }
	};

	static size_t block_edge_count;
	static size_t block_count;
	static size_t location_counts[NODES_PER_CELL];
	static ptrdiff_t block_dimensions[DIM_COUNT];

	static void init();
	static unsigned long get_boundary_membership(Coord& coord);
	static void get_node_block_index(size_t node_id, size_t& block_id, size_t& node_subid);

	static ptrdiff_t* get_sister_edges(unsigned char cell_index);
	static ptrdiff_t* get_block_edge(unsigned char cell_index, unsigned short location_index);
	static ptrdiff_t* get_node_edge_mask(unsigned char cell_index, unsigned short location_index);
	static ptrdiff_t get_edge_count(unsigned char cell_index);

	static unsigned short get_node_location_index(Coord& coord);
	static unsigned short get_block_location_index(Coord& coord);

	static ptrdiff_t* get_node_shift_vector(unsigned char cell_index, unsigned short location_index);
	static ptrdiff_t* get_block_shift_vector(unsigned short location_index);

	static void get_node_coord(size_t block_id, size_t node_subid, Coord& coord);
	static void get_block_coord(size_t block_id, Coord& coord);

private:
	// Initialized static members
	static bool initialized;

	// Edge sisters and block correspondance
	static ptrdiff_t edge_sister[NODES_PER_CELL][NODE_EDGE_COUNT];
	static vector<vector<ptrdiff_t> > block_edge[NODES_PER_CELL];
	static vector<vector<ptrdiff_t> > node_edge_mask[NODES_PER_CELL];
	static ptrdiff_t edge_count_by_cell_index[NODES_PER_CELL];

	// Boundary calcuations
	static vector<ptrdiff_t> offsets[NODES_PER_CELL][DIM_COUNT];

	// Node shift calculation
	static ptrdiff_t sizes[DIM_COUNT];
	static size_t block_dimension_strides[DIM_COUNT];
	static size_t size_strides[DIM_COUNT];
	static size_t offset_strides[NODES_PER_CELL][DIM_COUNT];
	static vector<size_t> ranges[NODES_PER_CELL][DIM_COUNT];
	static vector<vector<ptrdiff_t> > shifts[NODES_PER_CELL];

	// Block shift calculation
	static size_t block_offset_strides[DIM_COUNT];
	static size_t block_strides[DIM_COUNT];
	static vector<size_t> block_ranges[DIM_COUNT];
	static vector<vector<ptrdiff_t> > block_shifts;

	// Offset index calculation
	static size_t compute_offset_lut(ptrdiff_t shift_sizes[], size_t shift_strides[],
		vector<ptrdiff_t> offsets[], size_t offset_strides[],
		vector<size_t> ranges[], vector<vector<ptrdiff_t> >& shifts);

	// Block edge calcuation
	static void compute_block_edges(
		ptrdiff_t shift_sizes[], vector<ptrdiff_t> offsets[], size_t offset_strides[],
		vector<size_t> ranges[], size_t location_count,
		vector<ptrdiff_t> block_offsets[], vector<vector<ptrdiff_t> >& block_edges);

	static void compute_node_edge_masks(
		vector<vector<ptrdiff_t> > block_edge[], size_t location_counts[],
		vector<vector<ptrdiff_t> > node_edge_mask[]);

	static void compute_strides(ptrdiff_t sizes[], size_t strides[]);
	static unsigned short get_location_index(Coord& coord, size_t offset_strides[], vector<size_t> ranges[]);
};

// Inline methods
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t* Layout<OffsetVector, Dimensions, BlockDimensions>::get_sister_edges(unsigned char cell_index)
{
	return edge_sister[cell_index];
}

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t* Layout<OffsetVector, Dimensions, BlockDimensions>::get_block_edge(unsigned char cell_index, unsigned short location_index)
{
	return &block_edge[cell_index][location_index][0];
}

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t* Layout<OffsetVector, Dimensions, BlockDimensions>::get_node_edge_mask(unsigned char cell_index, unsigned short location_index)
{
	return &node_edge_mask[cell_index][location_index][0];
}

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t Layout<OffsetVector, Dimensions, BlockDimensions>::get_edge_count(unsigned char cell_index)
{
	return edge_count_by_cell_index[cell_index];
}

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t* Layout<OffsetVector, Dimensions, BlockDimensions>::get_block_shift_vector(unsigned short location_index)
{
	return &block_shifts[location_index][0];
}

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
INLINE ptrdiff_t* Layout<OffsetVector, Dimensions, BlockDimensions>::get_node_shift_vector(unsigned char cell_index, unsigned short location_index)
{
	return &shifts[cell_index][location_index][0];
}

// Static member instantiation
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_count;
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
ptrdiff_t Layout<OffsetVector, Dimensions, BlockDimensions>::edge_sister[NODES_PER_CELL][NODE_EDGE_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<vector<ptrdiff_t> > Layout<OffsetVector, Dimensions, BlockDimensions>::block_edge[NODES_PER_CELL];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<vector<ptrdiff_t> > Layout<OffsetVector, Dimensions, BlockDimensions>::node_edge_mask[NODES_PER_CELL];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
ptrdiff_t Layout<OffsetVector, Dimensions, BlockDimensions>::edge_count_by_cell_index[NODES_PER_CELL];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<ptrdiff_t> Layout<OffsetVector, Dimensions, BlockDimensions>::offsets[NODES_PER_CELL][DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::location_counts[NODES_PER_CELL];

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
ptrdiff_t Layout<OffsetVector, Dimensions, BlockDimensions>::sizes[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_dimension_strides[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::size_strides[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::offset_strides[NODES_PER_CELL][DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<size_t> Layout<OffsetVector, Dimensions, BlockDimensions>::ranges[NODES_PER_CELL][DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<vector<ptrdiff_t> > Layout<OffsetVector, Dimensions, BlockDimensions>::shifts[NODES_PER_CELL];

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_offset_strides[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_strides[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<size_t> Layout<OffsetVector, Dimensions, BlockDimensions>::block_ranges[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
vector<vector<ptrdiff_t> > Layout<OffsetVector, Dimensions, BlockDimensions>::block_shifts;

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
ptrdiff_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_dimensions[DIM_COUNT];
template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
size_t Layout<OffsetVector, Dimensions, BlockDimensions>::block_edge_count;

template <typename OffsetVector, typename Dimensions, typename BlockDimensions>
bool Layout<OffsetVector, Dimensions, BlockDimensions>::initialized = false;

#include "Layout.tpl"

#endif
