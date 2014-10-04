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

#ifndef BIT_FUNC_HPP
#define BIT_FUNC_HPP

namespace BitSpace {

//arithmetic
template<typename typev> 
const Bit<typev> operator+ (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) += rhs;}
template<typename typev> 
const Bit<typev> operator- (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) -= rhs;}
template<typename typev> 
const Bit<typev> operator* (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) *= rhs;}
template<typename typev> 
const Bit<typev> operator/ (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) /= rhs;}
template<typename typev> 
const Bit<typev> operator% (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) %= rhs;}
template<typename typev> 
const Bit<typev> operator| (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) |= rhs;}
template<typename typev> 
const Bit<typev> operator& (const Bit<typev>& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) &= rhs;}


template<typename typev, typename Y> 
const Bit<typev> operator+ (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) += rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator- (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) -= rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator* (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) *= rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator/ (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) /= rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator% (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) %= rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator| (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) |= rhs;}
template<typename typev, typename Y> 
const Bit<typev> operator& (const Bit<typev>& lhs, const Y& rhs) {return Bit<typev>(lhs) &= rhs;}

template<typename typev, typename X> 
const Bit<typev> operator+ (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) += rhs;}
template<typename typev, typename X> 
const Bit<typev> operator- (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) -= rhs;}
template<typename typev, typename X> 
const Bit<typev> operator* (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) *= rhs;}
template<typename typev, typename X> 
const Bit<typev> operator/ (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) /= rhs;}
template<typename typev, typename X> 
const Bit<typev> operator% (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) %= rhs;}
template<typename typev, typename X> 
const Bit<typev> operator| (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) |= rhs;}
template<typename typev, typename X> 
const Bit<typev> operator& (const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) &= rhs;}


//compare
template<typename typev> 
bool operator<(const Bit<typev>& lhs, const Bit<typev>& rhs) {
	if(lhs.sign()) {
		if(rhs.sign()) return lhs.smallerAbs(rhs);
		else return false;
	}
	else {
		if(rhs.sign()) return true;
		else return rhs.smallerAbs(lhs);
	}
}
template<typename typev> 
bool operator>(const Bit<typev>& lhs, const Bit<typev>& rhs) {
	if(lhs.sign()) {
		if(rhs.sign()) return rhs.smallerAbs(lhs);
		else return true;
	}
	else {
		if(rhs.sign()) return false;
		else return lhs.smallerAbs(rhs);
	}
}
template<typename typev> 
bool operator==(const Bit<typev>& lhs, const Bit<typev>& rhs) {
	if(lhs.sign() ^ rhs.sign()) return false;
	return lhs.equalAbs(rhs);
}

template<typename typev> 
bool operator<=(const Bit<typev>& lhs, const Bit<typev>& rhs) {return !(lhs > rhs);}
template<typename typev> 
bool operator>=(const Bit<typev>& lhs, const Bit<typev>& rhs) {return !(lhs < rhs);}
template<typename typev> 
bool operator!=(const Bit<typev>& lhs, const Bit<typev>& rhs) {return !(lhs == rhs);}


template<typename typev, typename Y> 
bool operator<(const Bit<typev>& lhs,  const Y& rhs) {return lhs <  Bit<typev>(rhs);}
template<typename typev, typename Y> 
bool operator>(const Bit<typev>& lhs,  const Y& rhs) {return lhs >  Bit<typev>(rhs);}
template<typename typev, typename Y> 
bool operator==(const Bit<typev>& lhs, const Y& rhs) {return lhs == Bit<typev>(rhs);}
template<typename typev, typename Y> 
bool operator<=(const Bit<typev>& lhs, const Y& rhs) {return lhs <= Bit<typev>(rhs);}
template<typename typev, typename Y> 
bool operator>=(const Bit<typev>& lhs, const Y& rhs) {return lhs >= Bit<typev>(rhs);}
template<typename typev, typename Y> 
bool operator!=(const Bit<typev>& lhs, const Y& rhs) {return lhs != Bit<typev>(rhs);}

template<typename typev, typename X> 
bool operator<(const X& lhs,  const Bit<typev>& rhs) {return Bit<typev>(lhs) <  rhs;}
template<typename typev, typename X> 
bool operator>(const X& lhs,  const Bit<typev>& rhs) {return Bit<typev>(lhs) >  rhs;}
template<typename typev, typename X> 
bool operator==(const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) == rhs;}
template<typename typev, typename X> 
bool operator<=(const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) <= rhs;}
template<typename typev, typename X> 
bool operator>=(const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) >= rhs;}
template<typename typev, typename X> 
bool operator!=(const X& lhs, const Bit<typev>& rhs) {return Bit<typev>(lhs) != rhs;}


}//end BitSpace


#endif
