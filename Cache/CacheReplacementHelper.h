/*
 * CacheReplacementHelper.h
 *
 *  Created on: 2020¦~11¤ë21¤é
 *      Author: Kenneth Yeung
 */

#ifndef CACHEREPLACEMENTHELPER_H_
#define CACHEREPLACEMENTHELPER_H_

#include <array>

#include "../Address.h"
#include "CacheReplacementPolicy.h"


template<unsigned Sets, unsigned BlocksPerSet, CacheReplacementPolicy policy>
class CacheReplacementHelper;



//pseudo least recently used
//https://en.wikipedia.org/wiki/Pseudo-LRU#Bit-PLRU
template<unsigned Sets, unsigned BlocksPerSet>
class CacheReplacementHelper<Sets, BlocksPerSet, CacheReplacementPolicy::PLRU1>
{
public:
	class SetUsageInfo
	{
	public:
		SetUsageInfo()
		{
			reset();
		}

		void set(unsigned block_index)
		{
			data_[block_index] = true;
			if ( is_all_true() )
				reset();
		}

		unsigned least_recently_used() const
		{
			for ( unsigned i = 0; i < BlocksPerSet; ++i)
			{
				if ( !data_[i])
					return i;
			}
			return 0;
		}

	private:

		void reset()
		{
			for ( unsigned i = 0; i < BlocksPerSet; ++i)
				data_[i] = false;
		}

		bool is_all_true() const
		{
			for ( unsigned i = 0; i < BlocksPerSet; ++i)
			{
				if (! data_[i])
					return false;
			}
			return true;
		}

		std::array<bool, BlocksPerSet> data_;
	};

	template<typename CacheAddress>
	void last_used_is(const CacheAddress& cache_address, unsigned block_index)
	{
		auto& info = sets_[cache_address.index];
		info.set(block_index);
	}

	template<typename CacheAddress>
	unsigned least_recently_used(const CacheAddress& cache_address) const
	{
		const auto& info = sets_[cache_address.index];
		return info.least_recently_used();
	}

private:
	std::array<SetUsageInfo, Sets> sets_;
};



#endif /* CACHEREPLACEMENTHELPER_H_ */
