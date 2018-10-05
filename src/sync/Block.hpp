#ifndef __libcr_sync_block_hpp_defined
#define __libcr_sync_block_hpp_defined

#include <type_traits>

namespace cr::sync
{
	/** Type indicating nonblocking operations. */
	typedef std::true_type nonblock;
	/** Type indicating blocking operations. */
	typedef std::false_type block;
	/** Type indicating possibly blocking operations. */
	typedef bool mayblock;
}

#endif