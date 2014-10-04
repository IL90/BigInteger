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

#ifndef BIG_INTEGER_FUNC_HPP
#define BIG_INTEGER_FUNC_HPP

//output
template<unsigned long long BASE>
std::ostream& operator<<(std::ostream& out, const BigInteger<BASE>& big) {
	if(big.isNan()) {
		out<<"Nan";
		return out;
	}
	if(!big.sign()) out<<"-";
	if(big.isInf()) {
		out<<"Inf";
	}
	else {
		if(big.base() > 10 && big.base() != 16) out<<"|";
		for(int index = big.realSize()-1; index >= 0; --index) {
			if(big.base() == 16) 
				out<<std::hex<<big[index];
			else 
				out<<big[index];
			if(big.base() > 10 && big.base() != 16) out<<"|";
		}
	}
	return out;
}

//arithmetic
template<unsigned long long BASE>
const BigInteger<BASE> operator+(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs)+=rhs;}
template<unsigned long long BASE>
const BigInteger<BASE> operator-(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs)-=rhs;}

template<unsigned long long BASE>
const BigInteger<BASE> operator*(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	if(lhs.isNan()) return lhs;
	else if(rhs.isNan()) return rhs;

	if(lhs.isInf()) {
		BigInteger<BASE> res;
		if(rhs.isNull()) {
			res.setNan();
		}
		else {
			res.setInf();
			res._sign = !(lhs.sign() ^ rhs.sign());
		}
		return res;
	}
	else if(lhs.isNull()) {
		BigInteger<BASE> res;
		if(rhs.isInf())
			res.setNan();
		else {
			res = 0;
			res._sign = !(lhs.sign() ^ rhs.sign());
		}
		return res;
	}
	else if(rhs.isInf()) {
		BigInteger<BASE> res;
		res.setInf();
		res._sign = !(lhs.sign() ^ rhs.sign());
		return res;
	}
	else if(rhs.isNull()) {
		BigInteger<BASE> res;
		res = 0;
		res._sign = !(lhs.sign() ^ rhs.sign());
		return res;
	}
	
	BigInteger<BASE> mnrl(lhs);
	BigInteger<BASE> mnrr(rhs);

	mnrl.abs();
	mnrr.abs();

	int sizel = mnrl.capacity();
	int sizer = mnrr.capacity();

	if(sizel < sizer) 
		while(sizel < sizer) sizel = mnrl.expand();
	else 
		while(sizer < sizel) sizer = mnrr.expand();

	if(sizel == 1) {
		mnrl[0] *= mnrr[0];
		for(int index = 0; index < sizel; ++index) {
			if(mnrl[index] < mnrl.base())
				continue;
			if(index+1 >= sizel)
				sizel = mnrl.expand();
			mnrl[index+1] += mnrl[index] / mnrl.base();
			mnrl[index] %= mnrl.base();
		}
		mnrl.updRealSize();
	}
	else {
		BigInteger<BASE> majl = mnrl.bisection();
		BigInteger<BASE> majr = mnrr.bisection();
		BigInteger<BASE> suml(majl);
		BigInteger<BASE> sumr(majr);

		suml.plus_bit(mnrl);//must be majl.capacity() == mnrl.capacity() == sizel/2
		sumr.plus_bit(mnrr);//|9|9|9|9|(plus_bit)|9|9|9|9| = |19|9|9|8|

		suml = suml*sumr;//recursion
		majl = majl*majr;//recursion
		mnrl = mnrl*mnrr;//recursion

		mnrl += (majl<<sizel) + ((suml-(majl+mnrl))<<(sizel>>1));
	}
	mnrl._sign = !(lhs.sign() ^ rhs.sign());
	return mnrl;
}

template<unsigned long long BASE>
const BigInteger<BASE> operator/(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	std::pair< BigInteger<BASE>, BigInteger<BASE> > res = divide(lhs,rhs);
	return res.first;
}

template<unsigned long long BASE>
const BigInteger<BASE> operator%(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	std::pair< BigInteger<BASE>, BigInteger<BASE> > res = divide(lhs,rhs);
	return res.second;
}

template<unsigned long long BASE>
std::pair< BigInteger<BASE>, BigInteger<BASE> > divide(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	std::pair< BigInteger<BASE>, BigInteger<BASE> > res;//first - q, second - r
	BigInteger<BASE>& q = res.first;
	BigInteger<BASE>& r = res.second;
	
	if(lhs.isNan() || rhs.isNan()) {
		q.setNan();
		r.setNan();
		return res;
	}

	if(lhs.isInf()) {
		if(rhs.isInf()) 
			q.setNan();
		else {
			q.setInf();
			q._sign = !(lhs.sign()^rhs.sign());
		}
		r.setNan();
		return res;
	}
	else if(lhs.isNull()) {
		if(rhs.isNull()) 
			q.setNan();
		else 
			q = BigInteger<BASE>(0, !(lhs.sign()^rhs.sign()));
		r.setNan();
		return res;
	}
	else if(rhs.isInf()) {
		q = BigInteger<BASE>(0, !(lhs.sign()^rhs.sign()));
		r.setNan();
		return res;
	}
	else if(rhs.isNull()) {
		q.setInf();
		q._sign = !(lhs.sign()^rhs.sign());
		r.setNan();
		return res;
	}
	
	int m = lhs.realSize();
	int n = rhs.realSize();
	if(m < n) {
		q = BigInteger<BASE>(0, !(lhs.sign()^rhs.sign()));
		r = lhs;
		return res;
	}
	unsigned long long base = q.base();
	if(n == 1) {
		q.reserve(m);
		r[0] = 0;
		for(int j = m-1; j >= 0; --j) {
			r[0] = r[0]*q.base() + lhs[j];
			q[j] = r[0]/rhs[0];
			r[0] -= q[j]*rhs[0];
		}
		q.updRealSize();
		q.shrink();
		q._sign = !(lhs.sign()^rhs.sign());	
		r._sign = q.sign();	

		return res;
	}

	BigInteger<BASE> un(lhs);
	BigInteger<BASE> vn(rhs);
	un.abs();
	vn.abs();

	un.reserve(m+1);
	vn.reserve(n);

	q.reserve(m-n+1);
	r.reserve(n);

	typename BigInteger<BASE>::bit qhat;
	typename BigInteger<BASE>::bit tmp;
	typename BigInteger<BASE>::bit rhat;

	for(int j = m-n; j >= 0; --j) {
		tmp = un[j+n]*base + un[j+n-1];
		qhat = tmp/vn[n-1];
		rhat = tmp - qhat*vn[n-1];
		
		while(qhat >= base || qhat*vn[n-2] > base*rhat + un[j+n-2]) {
			qhat -= 1;
			rhat += vn[n-1];
			if(rhat >= base) break;
		}
		// *&-
		for(int i = 0; i < n; ++i) {
			un[i+j] -= qhat*vn[i];

			un[i+j+1] += un[i+j] / base;
			un[i+j] %= base;
			if(un[i+j] < 0) {//< +0
				un[i+j] += base;
				un[i+j+1] -= 1;
			}
		}

		q[j] = qhat;
		if(un[n+j] < 0) {//< +0
			q[j] -= 1;
			for(int i = 0; i < n; ++i) {
				un[i+j] += vn[i];
				un[i+j+1] += un[i+j] / base;
				un[i+j] %= base;
			}
		}
	}
	q.updRealSize();
	un.updRealSize();
	q.shrink();
	r = un;
	q._sign = !(lhs.sign()^rhs.sign());	
	r._sign = q.sign();	
	return res;
}

template<unsigned long long BASE>
const BigInteger<BASE> operator<<(const BigInteger<BASE>& lhs, int shift) {return BigInteger<BASE>(lhs)<<=shift;}
template<unsigned long long BASE>
const BigInteger<BASE> operator>>(const BigInteger<BASE>& lhs, int shift) {return BigInteger<BASE>(lhs)>>=shift;}

template<unsigned long long BASE, typename X>
const BigInteger<BASE> operator+(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs)+=rhs;}
template<unsigned long long BASE, typename X>
const BigInteger<BASE> operator-(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs)-=rhs;}
template<unsigned long long BASE, typename X>
const BigInteger<BASE> operator*(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs)*rhs;}
template<unsigned long long BASE, typename X>
const BigInteger<BASE>  operator/(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) / rhs;}
template<unsigned long long BASE, typename X>
const BigInteger<BASE>  operator%(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) % rhs;}
template<unsigned long long BASE, typename X>
std::pair< BigInteger<BASE>, BigInteger<BASE> > divide(const X& lhs, const BigInteger<BASE>& rhs) {return divide(BigInteger<BASE>(lhs), rhs);}

template<unsigned long long BASE, typename Y>
const BigInteger<BASE> operator+(const BigInteger<BASE>& lhs, const Y& rhs) {return BigInteger<BASE>(lhs)+=rhs;}
template<unsigned long long BASE, typename Y>
const BigInteger<BASE> operator-(const BigInteger<BASE>& lhs, const Y& rhs) {return BigInteger<BASE>(lhs) -= rhs;}
template<unsigned long long BASE, typename Y>
const BigInteger<BASE> operator*(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs*BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
const BigInteger<BASE>  operator/(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs / BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
const BigInteger<BASE>  operator%(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs % BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
std::pair< BigInteger<BASE>, BigInteger<BASE> > divide(const BigInteger<BASE>& lhs, const Y& rhs) {return divide(lhs, BigInteger<BASE>(rhs));}

//compare
template<unsigned long long BASE>
bool operator<(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	if(lhs.isNan() || rhs.isNan()) 
		return false;

	if(lhs.sign()) {
		if(rhs.sign()) return lhs.smallerAbs(rhs);
		else return false;
	}
	else {
		if(rhs.sign()) return true;
		else return rhs.smallerAbs(lhs);
	}
}
template<unsigned long long BASE>
bool operator>(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	if(lhs.isNan() || rhs.isNan()) 
		return false;

	if(lhs.sign()) {
		if(rhs.sign()) return rhs.smallerAbs(lhs);
		else return true;
	}
	else {
		if(rhs.sign()) return false;
		else return lhs.smallerAbs(rhs);
	}
}
template<unsigned long long BASE>
bool operator==(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {
	if(lhs.isNan() || rhs.isNan()) 
		return false;

	if(lhs.sign() ^ rhs.sign()) return false;
	return lhs.equalAbs(rhs);
}

template<unsigned long long BASE>
bool operator<=(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {return (lhs.isNan()||rhs.isNan())? false: !(lhs > rhs);}
template<unsigned long long BASE>
bool operator>=(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {return (lhs.isNan()||rhs.isNan())? false: !(lhs < rhs);}
template<unsigned long long BASE>
bool operator!=(const BigInteger<BASE>& lhs, const BigInteger<BASE>& rhs) {return (lhs.isNan()||rhs.isNan())? true:  !(lhs == rhs);}

template<unsigned long long BASE, typename X>
bool operator<(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) < rhs;}
template<unsigned long long BASE, typename X>
bool operator>(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) > rhs;}
template<unsigned long long BASE, typename X>
bool operator==(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) == rhs;}
template<unsigned long long BASE, typename X>
bool operator<=(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) <= rhs;}
template<unsigned long long BASE, typename X>
bool operator>=(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) >= rhs;}
template<unsigned long long BASE, typename X>
bool operator!=(const X& lhs, const BigInteger<BASE>& rhs) {return BigInteger<BASE>(lhs) != rhs;}

template<unsigned long long BASE, typename Y>
bool operator<(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs < BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
bool operator>(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs > BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
bool operator==(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs == BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
bool operator<=(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs <= BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
bool operator>=(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs >= BigInteger<BASE>(rhs);}
template<unsigned long long BASE, typename Y>
bool operator!=(const BigInteger<BASE>& lhs, const Y& rhs) {return lhs != BigInteger<BASE>(rhs);}

#endif
