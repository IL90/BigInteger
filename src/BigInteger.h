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

#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
using std::vector;

#include <cmath>
#include <utility>

#include "Bit.h"

namespace BigIntegerSpace {
	template<typename T>
	struct BitSize {
		enum {
			bitsize = (sizeof(T)<<2)-1/*bit count*/
		};
	};

	//choose typev for Bit<typev>
	template<int SWITCHER> 
	struct TraitsBase {
		typedef unsigned int typev;
	};
	template<> struct TraitsBase<0>{
		typedef unsigned char typev;
	};
	template<> struct TraitsBase<2>{
		typedef unsigned long typev;
	};
	template<> struct TraitsBase<3>{
		typedef unsigned long long typev;
	};
}
namespace BIS = BigIntegerSpace;

template<unsigned long long BASE = 0> class BigInteger;
template<unsigned long long BASE> std::ostream& operator<<(std::ostream& out, const BigInteger<BASE>& big);
template<unsigned long long BASE> const BigInteger<BASE> operator*(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
template<unsigned long long BASE> const BigInteger<BASE> operator/(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
template<unsigned long long BASE> const BigInteger<BASE> operator%(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
template<unsigned long long BASE> std::pair< BigInteger<BASE>, BigInteger<BASE> > divide(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);

template< unsigned long long BASE>
class BigInteger {
//friends
		friend std::ostream& operator<< <BASE>(std::ostream& out, const BigInteger<BASE>& big);
		friend const BigInteger<BASE> operator* <BASE>(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
		friend const BigInteger<BASE> operator/ <BASE>(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
		friend const BigInteger<BASE> operator% <BASE>(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
		friend std::pair< BigInteger<BASE>, BigInteger<BASE> > divide<BASE>(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs);
//enums
		enum { 
			SWITCHER = 
				(BASE < 2) ? 1/*uint*/ : 
				(BASE <= (1<<(BIS::BitSize< BIS::TraitsBase<0>::typev >::bitsize)) ) ? 0/*uchar*/ : 
				(BASE <= (1<<(BIS::BitSize< BIS::TraitsBase<1>::typev >::bitsize)) ) ? 1/*uint*/ : 
				(BASE <= (1<<(BIS::BitSize< BIS::TraitsBase<2>::typev >::bitsize)) ) ? 2/*ulong*/ : 
				3/*ulonglong*/
		};
//types
	public:
		typedef typename BIS::TraitsBase<SWITCHER>::typev typev;//Bit<typev>
		typedef BitSpace::Bit<typev> bit;
//init
	public:
		BigInteger(long long i = 0) : _sign(i>=0) {
			setBase();
			resize(1);
			i = ::llabs(i);
			convertFromEmb<unsigned long long>(static_cast<unsigned long long>(i));
		}
		BigInteger(unsigned long long u, bool sign) : _sign(sign) {
			setBase();
			resize(1);
			convertFromEmb<unsigned long long>(u);
		}
		BigInteger(const BigInteger& cpy) {init(cpy);}
		BigInteger(const BigInteger& cpy, bool) {//invert _sign
			init(cpy);
			invertSign();
		}
		BigInteger& operator=(const BigInteger& cpy) {
			if(&cpy != this) init(cpy); 
			return *this;
		}
	private:
		void init(const BigInteger& cpy) {
			_a = cpy._a;
			_base = cpy.base();
			_sign = cpy.sign();
			_realSize = cpy.realSize();
			_capacity = cpy.capacity();
		}
		void setBase() {
			if(BASE < 2 || BASE > 1<<(BIS::BitSize<typev>::bitsize)) _base = 1<<(BIS::BitSize<typev>::bitsize);
			else _base = BASE;
		}
//memory
	public:
		int realSize() const;//realSize >= 1, realSize <= capacity()
		void updRealSize(int shift = 0) const;
		int capacity() const {return _capacity;}
		void reserve(int n);
		void shrink();
	private:
		int expand();
		int increaseSizeRule(int size) {return size<<1;}
		void resize(int newSize, typev u = 0) {
			_a.resize(newSize, bit(u));
			_capacity = _a.size();
		}
//info
	public:
		typev base() const {return _base;}
		int bitsize() const {return BIS::BitSize<typev>::bitsize;}
		bool sign() const {return _sign;}
		bool isNan() const {return _a[0].isNan();}
		bool isInf() const {return _a[0].isInf();}
		bool isReal() const {return _a[0].isReal();}
		bool isNull() const {return (realSize() == 1) && (_a[0] == 0);}
//access
	public:
		const bit index(int index) const {return _a[index];}
	private:
		const bit operator[](int index) const {return _a[index];}
		bit& operator[] (int index) {return _a[index];}
//arithmetic
	public:
		template<unsigned long long BASE2>
		BigInteger<BASE>& convert(const BigInteger<BASE2>& cpy) {//b = BASE2, B = BASE, cpy --> *this
			BigInteger<BASE2> den(BASE);
			BigInteger<BASE2> zero(0);//+0
			BigInteger<BASE2> zero2(0,false);//-0
			BigInteger<BASE2> u(cpy);

			std::pair< BigInteger<BASE2>, BigInteger<BASE2> > par;
			BigInteger<BASE2>& quotient = par.first;
			BigInteger<BASE2>& rest = par.second;

			(*this) = 0;
			_sign = cpy.sign();
			int index = 0;
			unsigned long long ull, mul;
			int rsize;
			while(true) {
				par = divide(u, den);
				_a[index] = 0;
				mul = 1;
				rsize = rest.realSize();
				for(int i = 0; i < rsize; ++i) {
					ull = rest.index(i).toULL(); 
					ull *= mul;
					mul *= rest.base();
					_a[index] += ull;
				}
				if(quotient == zero || quotient == zero2) break;
				++index;
				u = quotient;
				reserve(index+1);
			}
			updRealSize();
			return *this;
		}

		void setInf() {
			resize(1);
			updRealSize();
			_a[0].setInf();
		}
		void setNan() {
			resize(1);
			updRealSize();
			_a[0].setNan();
			_sign = true;
		}
		BigInteger& operator+=(const BigInteger& rhs) {
			if(sign() ^ rhs.sign()) 
				return minus(rhs);
			else 
				return plus(rhs);
		}
		BigInteger& operator-=(const BigInteger& rhs) {
			if(sign() ^ rhs.sign()) 
				return plus(rhs);
			else 
				return minus(rhs);
		}
		//Karatsuba algorithm
		//AD + BC = (A+B)*(C+D) - AC - BD
		//(A + B*base^(n/2))*(C + D*base^(n/2)) = AC + BD*base^(n) + (AD + BC)*base^(n/2) = 
		// = AC + BD*base^n + base^(n/2)*((A+B)*(C+D) - AC - BD)
		BigInteger& operator*=(const BigInteger& rhs) {
			(*this) = (*this)*rhs;
			return *this;
		}
		BigInteger& operator/=(const BigInteger& rhs) {
			(*this) = (*this)/rhs;
			return *this;
		}
		BigInteger& operator%=(const BigInteger& rhs) {
			(*this) = (*this)%rhs;
			return *this;
		}
		BigInteger& operator<<=(int shift);
		BigInteger& operator>>=(int shift);
		const BigInteger operator-() {return BigInteger<BASE>(*this, true);}
	private:
		template<typename typeu>
		void convertFromEmb(typeu u) {
			if(u >= base()) {
				unsigned long long tmp,tmp2;
				int index = 0;
				for(;; ++index) {
					reserve(index+1);

					tmp = u / base();
					tmp2 = u - tmp*base();

					_a[index] = tmp2;
					if(tmp == 0) break;
					u = tmp;
				}
				updRealSize();
			}
			else {
				_a[0] = u;
				_realSize = 1;
			}
		}
		BigInteger& plus(const BigInteger& rhs);
		BigInteger& plus_bit(const BigInteger& rhs);//sizelhs == sizerhs
		BigInteger& minus(const BigInteger& rhs);
		BigInteger bisection();
		void invertSign() {_sign = !sign();}
		void abs() {_sign = true;}
//compare
	public:
		bool smallerAbs(const BigInteger& rhs) const {return compareAbs(rhs) <  0;}
		bool equalAbs(const BigInteger& rhs)   const {return compareAbs(rhs) == 0;}
	private:
		int compareAbs(const BigInteger& rhs) const;
//members
	private:
		vector<bit> _a;
		bool _sign;// true-(+), false-(-)
		typev _base;// _base >= 2 && _base <= BitSize<typev>::base
		mutable int _realSize;
		int _capacity;
};

#include "BigInteger_impl.hpp"
#include "BigInteger_func.hpp"

#endif
