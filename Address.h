/*
 * Address.h
 *
 *  Created on: 2020¦~11¤ë18¤é
 *      Author: Kenneth Yeung
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <stdint.h>

class Address
{
public:
	typedef uint64_t Type;
	Address():data_(0){}
	Address(const Type& data): data_(data){}

	template<typename T>
	static Address from(const T& t)
	{
		return Address( reinterpret_cast<const Type>(&t) );
	}

	const Type& value() const { return data_; }

private:
	Type data_;

};




#endif /* ADDRESS_H_ */
