/*
 * MemoryMath.h
 *
 *  Created on: 2020¦~11¤ë19¤é
 *      Author: Kenneth Yeung
 */

#ifndef MEMORYMATH_H_
#define MEMORYMATH_H_

namespace memory
{
constexpr unsigned log_2(unsigned n)
{
	unsigned ret=0;
	unsigned temp = n;
	while ( temp > 0)
	{
		temp = temp >> 1;
		++ret;
	}

	return ret-1;
}


}



#endif /* MEMORYMATH_H_ */
