/*
 * Cache.h
 *
 *  Created on: 2020年11月16日
 *      Author: Kenneth Yeung
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <array>
#include <cstring>
//for debug only
#include <iostream>

#include "CacheReplacementPolicy.h"
#include "CacheReplacementHelper.h"

#include "../MemoryMath.h"
#include "../Address.h"

//Capacity must be divisible by BlockSize
//Block Size must be power of 2
//todo: do compile time check, (with c++20 concept?)
//todo: access latency
template<unsigned Capacity, unsigned BlockSize, unsigned Associativity, CacheReplacementPolicy Policy>
class Cache
{
public:
	typedef char Byte;
	typedef std::array<Byte, BlockSize> Block;
	typedef Address::Type Tag;
	struct CacheEntry
	{
		Block block;
		Tag tag;
		bool is_valid=false;
		bool dirty=false;
	};
	typedef std::array<CacheEntry, Associativity> Set;

	Cache()
	{
		std::cout << "Cache::Cache" << std::endl;
		std::cout << "Capcity: " << Capacity << std::endl;
		std::cout << "BlockSize: " << BlockSize << std::endl;
		std::cout << "Associativity: " << Associativity << std::endl;
	}

	bool has_tag(const Address& address) const
	{
		const CacheAddress* cache_address = reinterpret_cast<const CacheAddress*>(&address);
//		std::cout << "Cache::has_tag " << cache_address->tag << " " << cache_address->index << " " << cache_address->offset << std::endl;

		const auto& set = sets_[cache_address->index];
		for ( const auto& cache_entry: set)
		{
			if ( cache_entry.is_valid && cache_entry.tag == cache_address->tag)
				return true;
		}
		return false;
	}

	void load_block(const Address& address)
	{
		const CacheAddress* cache_address = reinterpret_cast<const CacheAddress*>(&address);

		auto& set = sets_[cache_address->index];
		for ( size_t i=0; i < set.size(); ++i)
		{
			if ( set[i].is_valid && set[i].tag == cache_address->tag)
			{
				cache_replacement_helper_.last_used_is(*cache_address, i);
				return;
			}
		}
	}

	//for store from upper level caches/memory only, not for updating data
	//return true if there is space for the block, otherwise return false
	bool store_block(const Address& address, const Block& block)
	{
		const CacheAddress* cache_address = reinterpret_cast<const CacheAddress*>(&address);
		auto& set = sets_[cache_address->index];
		auto block_index = free_slot(set);
		if ( block_index != -1)
		{
			set[block_index].tag = cache_address->tag;
			set[block_index].block = block;
			set[block_index].is_valid = true;
			set[block_index].dirty = false;
			cache_replacement_helper_.last_used_is(*cache_address, block_index);
			return true;
		}
		else
			return false;
	}

	bool write(const Address& address, const Byte* data, size_t size)
	{
		const CacheAddress* cache_address = reinterpret_cast<const CacheAddress*>(&address);
		auto& set = sets_[cache_address->index];
		for ( size_t i=0; i < set.size(); ++i)
		{
			if ( set[i].is_valid && set[i].tag == cache_address->tag)
			{
				std::memcpy( set[i].block.data() + cache_address->offset, data, size);
				cache_replacement_helper_.last_used_is(*cache_address, i);
				set[i].dirty = true;
				return true;
			}
		}
		return false;
	}

	CacheEntry evict_and_replace(const Address& address, const Block& block)
	{
		const CacheAddress* cache_address = reinterpret_cast<const CacheAddress*>(&address);
		auto& set = sets_[cache_address->index];
		const unsigned block_index = cache_replacement_helper_.least_recently_used(*cache_address);
		const auto ret = set[block_index];

		//std::cout << "Cache::evict_and_replace, set: " << cache_address->index << ", block_index: " << block_index << std::endl;

		set[block_index].tag = cache_address->tag;
		set[block_index].block = block;
		set[block_index].is_valid = true;
		set[block_index].dirty = false;
		cache_replacement_helper_.last_used_is(*cache_address, block_index);

		return ret;
	}

	//tag size in bits
	static constexpr unsigned tag_size()
	{
//		 tag_length = address_length - index_length - block_offset_length
		unsigned tag_length = 0;
		tag_length = sizeof(Address) * 8 - memory::log_2( num_of_sets() ) - memory::log_2(BlockSize);

		return tag_length;
	}
	static constexpr unsigned num_of_sets() { return Capacity / (BlockSize * Associativity); }
	static constexpr unsigned num_of_blocks() { return Capacity / BlockSize; }


	struct CacheAddress
	{
		typedef Address::Type Type;

		//little endian
		Type offset: memory::log_2(BlockSize);
		Type index: memory::log_2( num_of_sets() );
		Type tag: Cache<Capacity, BlockSize, Associativity, Policy>::tag_size();
	};

private:
	int free_slot(const Set& set) const
	{
		for ( unsigned i=0; i < Associativity; ++i)
		{
			if ( !set[i].is_valid)
				return i;
		}

		return -1;
	}

	std::array<Set, num_of_sets()> sets_;
	CacheReplacementHelper<num_of_sets(), Associativity, Policy> cache_replacement_helper_;
};



#endif /* CACHE_H_ */
