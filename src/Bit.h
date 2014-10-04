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

#ifndef BIT_H
#define BIT_H

#include <cmath>
#include <iostream>
#include <utility> 

namespace BitSpace {

template< typename typev = unsigned long long int> class Bit;

template< typename typev>
class Bit {
//enums
	public:
		enum {
			INFTY = static_cast<typev>(1)<<(8*sizeof(typev) - 1),
			NANUM = static_cast<typev>(1)<<(8*sizeof(typev) - 2)
		};
//init
	public:
		Bit(typev u = 0, bool sign = true) : _value(u), _sign(sign) {}
		Bit(const Bit &cpy) {init(cpy);}
		Bit(const Bit &cpy, bool) {//invert _sign
			init(cpy);
			_sign = !cpy.sign();
		}
		Bit& operator=(const Bit& cpy) {
			init(cpy); 
			return *this;
		}
	private:
		void init(const Bit& cpy) {
			_value = cpy.value();
			_sign = cpy.sign();
		}
//info
	public:
		int size() const {return sizeof(typev);}
		bool isInf() const {return value() & INFTY;}
		bool isNan() const {return value() & NANUM;}
		bool isReal() const {return value() < NANUM;}
//access
	public:
		bool  sign()  const {return _sign;}
		typev value() const {return _value;}
		unsigned long long toULL() const {return value();}
//arithmetic
	public:
		void setInf() {_value = INFTY;}
		void setNan() {_value = NANUM;}
		inline Bit& operator+=(const Bit& rhs);
		inline Bit& operator-=(const Bit& rhs);
		inline Bit& operator*=(const Bit& rhs);
		inline Bit& operator/=(const Bit& rhs);
		inline Bit& operator%=(const Bit& rhs);
		inline Bit& operator<<=(const Bit& rhs);
		inline Bit& operator>>=(const Bit& rhs);

		const Bit operator>>(const Bit& rhs) {return Bit(*this) >>= rhs;}
		const Bit operator<<(const Bit& rhs) {return Bit(*this) <<= rhs;}

		inline Bit& operator|=(const Bit& rhs);
		inline Bit& operator&=(const Bit& rhs);

		const Bit& operator+() {return *this;}
		const Bit operator-()  {return Bit(*this, true);}
		const Bit& operator++() {return *this += 1;}
		const Bit& operator--() {return *this -= 1;}

		inline const Bit operator++(int);
		inline const Bit operator--(int);
	private:
		inline Bit& plus(const Bit& rhs);
		inline Bit& minus(const Bit& rhs);
//compare
	public:
		inline bool smallerAbs(const Bit& rhs) const;
		inline bool equalAbs(const Bit& rhs) const;
//members
	private:
		typev _value;
		bool _sign;
};

}//end BitSpace

//output
template<typename typev>
std::ostream& operator<<(std::ostream& out, const BitSpace::Bit<typev>& bit) {
	if(!bit.sign()) out<<"-";
	out << bit.value();
	return out;
}
template<> std::ostream& operator<< <unsigned char>(std::ostream& out, const BitSpace::Bit<unsigned char>& bit);


#include "Bit_impl.hpp"
#include "Bit_func.hpp"

#endif
