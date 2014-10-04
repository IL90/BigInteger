BigInteger
==========
Implementation of long arithmetic. 
The base of the system of numeration is any integer from 2 to 1 << ((sizeof(unsigned long long)<<2)-1)

base = [ 2; 1 << ((sizeof(unsigned char)<<2)-1) ]
[x][x][x][x][x][0/1][0/1][0/1] = [1 byte] 

base = ( 1 << ((sizeof(unsigned char)<<2)-1); 1 << ((sizeof(unsigned)<<2)-1) ]
[1 byte][1 byte] [x][x][0/1][0/1][0/1][0/1][0/1][0/1] [0/1][0/1][0/1][0/1][0/1][0/1][0/1][0/1] = [4 bytes]

base = ( 1 << ((sizeof(unsigned)<<2)-1); 1 << ((sizeof(unsigned long)<<2)-1) ]
[1 byte][1 byte][1 byte][1 byte] [x][x][0/1][0/1][0/1][0/1][0/1][0/1] [0/1][0/1][0/1][0/1][0/1][0/1][0/1][0/1] [0/1][0/1][0/1][0/1][0/1][0/1][0/1][0/1] [0/1][0/1][0/1][0/1][0/1][0/1][0/1][0/1] = [8 bytes]

arithmetic:
+,-,*,/,%

signed zero:
(+0) + (+0) = (+0) - (-0) = (+0)
(-0) + (-0) = (-0)
x-x = +0
...

infinity:
x/(+0) = Inf
Inf * 0 = NaN
Inf - Inf = NaN
-Inf/(-0) = Inf
...

convert:
BigInteger<base1> a;
BigInteger<base2> b;
b.convert(a)
