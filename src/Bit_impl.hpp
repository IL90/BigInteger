/*
Copyright (C) 2014 Ilya Arkhipovsky
This file is part of BigInteger.
( Implementation of long arithmetic. The base of the system of numeration is any integer from 2 to 1 << ((sizeof(unsigned long long)<<2)-1) )

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact e-mail <arkhipovsky.ilya@yandex.ru>
*/

#ifndef BIT_IMPL_HPP
#define BIT_IMPL_HPP

namespace BitSpace {

//arithmetic
template<typename typev>
Bit<typev>& Bit<typev>::operator+=(const Bit<typev>& rhs) {
	if(sign() ^ rhs.sign()) 
		return minus(rhs);
	else 
		return plus(rhs);
}
template<typename typev>
Bit<typev>& Bit<typev>::operator-=(const Bit<typev>& rhs) {
	if(sign() ^ rhs.sign()) 
		return plus(rhs);
	else 
		return minus(rhs);
}
template<typename typev>
Bit<typev>& Bit<typev>::operator*=(const Bit<typev>& rhs) {
	_value *= rhs.value();
	if(!rhs.sign()) _sign = !sign();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator/=(const Bit<typev>& rhs) {
	_value /= rhs.value();
	if(!rhs.sign()) _sign = !sign();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator%=(const Bit<typev>& rhs) {
	_value %= rhs.value();
	if(value() == 0 ) _sign = true;
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator<<=(const Bit<typev>& rhs) {
	_value <<= rhs.value();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator>>=(const Bit<typev>& rhs) {
	_value >>= rhs.value();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator|=(const Bit<typev>& rhs) {
	_value |= rhs.value();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::operator&=(const Bit<typev>& rhs) {
	_value &= rhs.value();
	return *this;
}
template<typename typev>
const Bit<typev> Bit<typev>::operator++(int) {
	Bit<typev> oldBit(*this);
	++(*this);
	return oldBit;
}
template<typename typev>
const Bit<typev> Bit<typev>::operator--(int) {
	Bit<typev> oldBit(*this);
	--(*this);
	return oldBit;
}
template<typename typev>
Bit<typev>& Bit<typev>::plus(const Bit<typev>& rhs) {
	_value += rhs.value();
	return *this;
}
template<typename typev>
Bit<typev>& Bit<typev>::minus(const Bit<typev>& rhs) {
	if(value() >= rhs.value()) {
		_value -= rhs.value();
		if(value() == 0) _sign = true;
	}
	else {
		typev tmp = rhs.value();
		tmp -= value();
		_value = tmp;
		_sign = !sign();
	}
	return *this;
}

//compare
template<typename typev>
bool Bit<typev>::smallerAbs(const Bit<typev>& rhs) const {
	if(value() < rhs.value()) return true;
	else return false;
}
template<typename typev>
bool Bit<typev>::equalAbs(const Bit<typev>& rhs) const {
	if(value() == rhs.value()) return true;
	else return false;
}

}

#endif
