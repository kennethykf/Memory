/*
 * CacheController.h
 *
 *  Created on: 2020¦~11¤ë16¤é
 *      Author: Kenneth Yeung
 */

#ifndef CACHECONTROLLER_H_
#define CACHECONTROLLER_H_

#include <iostream>
#include <array>

#include "Memory/Address.h"
#include "Cache.h"
#include "CacheReplacementPolicy.h"

//The cache controller intercepts read and write memory requests before passing them on to the memory controller.
class CacheController
{
public:
//	typedef Cache<32*1024, 64, 8, CacheReplacementPolicy::PLRU1> CacheType;
	typedef Cache<256, 64, 2, CacheReplacementPolicy::PLRU1> CacheType;
	typedef typename CacheType::Block BlockType;

	CacheController():hit_count_(0), miss_count_(0){}

	void read(const Address& address, size_t size)
	{
		std::cout << __PRETTY_FUNCTION__ << " " << address.value() << std::endl;
		if ( l1_cache_.has_tag(address))
		{
			//cache hit
			++hit_count_;
			std::cout << "Cache hit" << std::endl;
			l1_cache_.load_block(address);

		}
		else
		{
			//cache miss
			++miss_count_;
			std::cout << "Cache miss" << std::endl;
			//todo: pretend to read from L2Cache/Memory
			if ( !l1_cache_.store_block(address, dummy_block_) )
				l1_cache_.evict_and_replace(address, dummy_block_);
		}

	}

	void write(const Address& address, const char* data, size_t size)
	{
		if ( l1_cache_.has_tag(address) )
		{
			//cache hit
			++hit_count_;
			std::cout << "Cache hit" << std::endl;
			l1_cache_.write(address, data, size);
		}
		else
		{
			//cache miss
			++miss_count_;
			std::cout << "Cache miss" << std::endl;
			//todo: pretend to read from L2Cache/Memory
			l1_cache_.store_block(address, dummy_block_);
		}

	}


	//interface for analysis
	uint64_t cache_hit_count() const { return hit_count_; }
	uint64_t cache_miss_count() const { return miss_count_; }

private:
	BlockType dummy_block_;
	CacheType l1_cache_;
	//bus
	uint64_t hit_count_;
	uint64_t miss_count_;
};



#endif /* CACHECONTROLLER_H_ */
