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

#ifndef BIG_INTEGER_IMPL_HPP
#define BIG_INTEGER_IMPL_HPP

//memory
template<unsigned long long BASE>
int BigInteger<BASE>::realSize() const {//realSize >= 1, realSize <= capacity()
	if(_realSize == 0) {
		for(int index = capacity()-1; index >= 0; --index) {//find realsize
			if(_a[index] != 0) {
				_realSize = index+1;
				return _realSize;
			}
		}
		_realSize = 1;
	}
	return _realSize;
}
template<unsigned long long BASE>
void BigInteger<BASE>::updRealSize(int shift) const {//realSize >= 1, realSize <= capacity()
	if(shift == 0) 
		_realSize = 0;
	else 
		_realSize += shift;
}
template<unsigned long long BASE>
void BigInteger<BASE>::reserve(int n) {
	int size = capacity();
	if(n <= size) 
		return;
	while(n > size) 
		size = increaseSizeRule(size);
	resize(size);
}
template<unsigned long long BASE>
void BigInteger<BASE>::shrink() {//shrink vector _a
	int size = capacity();
	while((size>>=1) > realSize()) 
		resize(size);
}
template<unsigned long long BASE>
int BigInteger<BASE>::expand() {
	int size = capacity();
	size = increaseSizeRule(size);
	resize(size);
	return size;
}

//arithmetic
template<unsigned long long BASE>
BigInteger<BASE>& BigInteger<BASE>::operator<<=(int shift) {
	if(!isReal() || isNull()) 
		return *this;

	if(shift > 0) {
		int size = capacity();
		int realsize = realSize();
		while(realsize + shift > size)
			size = expand();
		if(realsize == 1 && _a[0] == 0) 
			return *this;
		for(int index = realsize-1; index >= 0; --index) 
			_a[index+shift] = _a[index];
		for(int index = shift-1; index >= 0; --index)
			_a[index] = 0;
		updRealSize(shift);
		return *this;
	}
	else if(shift == 0) 
		return *this;
	else 
		return (*this)>>=(-shift);
}
template<unsigned long long BASE>
BigInteger<BASE>& BigInteger<BASE>::operator>>=(int shift) {
	if(!isReal() || isNull()) 
		return *this;

	if(shift > 0) {
		int realsize = realSize();
		for(int index = shift; index < realsize; ++index) 
			_a[index-shift] = _a[index];
		int begin = realsize-shift;
		if(begin >= 0) 
			updRealSize(-shift);
		else {
			begin = 0;
			updRealSize();
		}
		for(int index = begin; index < realsize; ++index)
			_a[index] = 0;
		shrink();
		return *this;
	}
	else if(shift == 0) {
		return *this;
	}
	else {
		return (*this)<<=(-shift);
	}
}
template<unsigned long long BASE>
BigInteger<BASE>& BigInteger<BASE>::plus(const BigInteger<BASE>& rhs) {
	if(isNan() || rhs.isNan()) {
		setNan(); 
		return *this;
	}
	if(isInf() || rhs.isInf()) {
		setInf(); 
		return *this;
	}
	
	int sizelhs = capacity();
	int sizerhs = rhs.capacity();
	while(sizelhs < sizerhs) 
		sizelhs = expand();
	int rsizel = realSize();
	int rsizer = rhs.realSize();
	for(int index = 0; index < rsizer; ++index) 
		_a[index] += rhs[index];
	if(rsizer > rsizel) {
		updRealSize(rsizer - rsizel);
		rsizel = rsizer;
	}

	for(int index = 0; index < rsizel-1; ++index) {//carry
		if(_a[index] < base()) 
			continue;
		_a[index+1] += 1;
		_a[index] -= base();
	}

	if(_a[rsizel-1] >= base()) {
		if(rsizel >= sizelhs) 
			sizelhs = expand();
		_a[rsizel] += 1;
		_a[rsizel-1] -= base();
		updRealSize(1);
	}
	return *this;
}
template<unsigned long long BASE>
BigInteger<BASE>& BigInteger<BASE>::plus_bit(const BigInteger<BASE>& rhs) {//lhs.capacity() == rhs.capacity()
	int size = capacity();

	for(int index = 0; index < size; ++index) 
		_a[index] += rhs._a[index];

	for(int index = 0; index < size - 1; ++index)
		if(_a[index] >= base()) {
			_a[index+1] += 1;
			_a[index] -= base();
		}
	updRealSize();
	return *this;
}
template<unsigned long long BASE>
BigInteger<BASE>& BigInteger<BASE>::minus(const BigInteger<BASE>& rhs) {
	if(isNan() || rhs.isNan()) {
		setNan(); 
		return *this;
	}
	if(isInf()) {
		if(rhs.isInf()) 
			setNan();
		return *this;
	}
	else if(rhs.isInf()) {
		*this = rhs;
		invertSign();
		return *this;
	}
	

	int flag = rhs.compareAbs(*this);//smallerAbs
	if(flag == 0) {
		*this = 0;
		return *this;
	}
	int size = capacity();
	int maxrealsize, minrealsize;
	if(flag < 0) {//rhs < *this
		minrealsize = rhs.realSize();
		maxrealsize = realSize();
		for(int index = 0; index < minrealsize; ++index) {
			_a[index] -= rhs._a[index];
		}
	}
	else {
		minrealsize = realSize();
		maxrealsize = rhs.realSize();
		int sizerhs = rhs.capacity();
		while(size < sizerhs) size = expand();
		for(int index = 0; index < maxrealsize; ++index) {
			_a[index] = rhs._a[index] - _a[index];
		}
		invertSign();
	}

	for(int index = 0; index < maxrealsize-1; ++index) {//carry
		if(_a[index] < 0) {
			_a[index] += base();
			_a[index+1] -= 1;
		}
	}
	if(flag > 0 || _a[maxrealsize-1] == 0) {
		updRealSize();
		shrink();
	}
	return *this;
}
template<unsigned long long BASE>
BigInteger<BASE> BigInteger<BASE>::bisection() {
	BigInteger tmp(*this);
	int size = capacity()>>1;
	int rsize = realSize();
	tmp >>= (size);
	tmp.resize(size);

	resize(size);
	if(rsize > size)
		updRealSize(size-rsize);
	return tmp;
}

//compare
template<unsigned long long BASE>
int BigInteger<BASE>::compareAbs(const BigInteger<BASE>& rhs) const {
	if(isNan() || rhs.isNan()) return 2;//isNan == true
	if(isInf()) {
		if(rhs.isInf()) return 0;//|*this| == |rhs|
		else return 1;//|*this| > |rhs|
	}
	else if(rhs.isInf()) {
		return -1;//|*this| < |rhs|
	}

	int realsizelhs = realSize();
	int realsizerhs = rhs.realSize();
	if(realsizelhs < realsizerhs) return -1;//smallerAbs == true, |*this| < |rhs|
	else if(realsizelhs > realsizerhs) return 1;//|*this| > |rhs|
	for(int index = realsizelhs-1; index >= 0; --index) {
		if(_a[index] < rhs._a[index]) return -1;//smallerAbs == true, |*this| < |rhs|
		else if(_a[index] > rhs._a[index]) return 1;//|*this| > |rhs|
	}
	return 0;//equalAbs == true, |*this| == |rhs|
}

#endif
