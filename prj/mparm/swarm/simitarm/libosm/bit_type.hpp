/*************************************************************************
    Copyright (C) 2002,2003,2004 Wei Qin
    See file COPYING for more information.

    This program is free software; you can redistribute it and/or modify    
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*************************************************************************/

#ifndef __BIT_TYPE_H__
#define __BIT_TYPE_H__

#include "bittypes.h"
#include <cstdio>
#include <string>
#include <cassert>

#define _WORD_T   uint32_t
#define _DWORD_T  uint64_t
#define _S_WORD_T  int32_t
#define _S_DWORD_T int64_t

#define _VAL(_a) (_a).val()

#define _INT_T(_a) _int_t_<_a>
#define _UINT_T(_a) _uint_t_<_a>

#define _INT_CTR(_a,_v) _int_t_<_a>(_v)
#define _UINT_CTR(_a,_v) _uint_t_<_a>(_v)

/* For extraction, we only shift here, the constructor will
 * take care of masking the high order bits*/
#define _INT_XTR1(_a,_i) _int_t_<1>(_a.extract(_i))
#define _UINT_XTR1(_a,_i) _uint_t_<1>(_a.extract(_i))

#define _UINT_XTR2(_a,_i1,_i2) _uint_t_<_i1-_i2+1>(_a.extract(_i1,_i2))
#define _INT_XTR2(_a,_i1,_i2) _int_t_<_i1-_i2+1>(_a.extract(_i1,_i2))


#define _INT_DEP1(_a,_i,_v) \
		(_a).deposit((unsigned)(_i), _VAL((_v)))

#define _INT_DEP2(_a,_i1,_i2,_v) \
		(_a).deposit((unsigned)(_i1), (unsigned)(_i2), _VAL((_v)))

#define _UINT_DEP1(_a,_i,_v) \
		(_a).deposit((unsigned)(_i), _VAL((_v)))

#define _UINT_DEP2(_a,_i1,_i2,_v) \
		(_a).deposit((unsigned)(_i1), (unsigned)(_i2), _VAL((_v)))


extern void _strvalue(std::string& s, _WORD_T v, unsigned base);
extern void _strvalue(std::string& s, _DWORD_T v, unsigned base);
extern void _strvalue(std::string& s, _S_WORD_T v, unsigned base);
extern void _strvalue(std::string& s, _S_DWORD_T v, unsigned base);

/* bit class template, for Len<=64 */
template <class _T_, unsigned _Len_> class _bit_t_ {

	public:

		/* Constructor 1 */
		_bit_t_() : _value_(0) {}

		/* Constructor 2 */
		_bit_t_(const _T_& v) {
			_value_ = (v<<(sizeof(_T_)*8-_Len_))>>(sizeof(_T_)*8-_Len_);
		}

		/* OR operation */
		_bit_t_<_T_,_Len_>& operator |= (const _bit_t_<_T_,_Len_>& v) {
			_value_ = _value_ | v._value_;
			return *this;
		}

		/* AND operation */
		_bit_t_<_T_,_Len_>& operator &= (const _bit_t_<_T_,_Len_>& v) {
			_value_ = _value_ & v._value_;
			return *this;
		}

		/* XOR operation */
		_bit_t_<_T_,_Len_>& operator ^= (const _bit_t_<_T_,_Len_>& v) {
			_value_ = _value_ ^ v._value_;
			return *this;
		}

		/* Right shift operation */
		_bit_t_<_T_,_Len_>& operator >>= (const _bit_t_<_T_,_Len_>& v) {
			_value_ = _value_ >> v._value_;
			return *this;
		}

		_bit_t_<_T_,_Len_>& operator >>= (unsigned v) {
			_value_ = _value_ >> v;
			return *this;
		}

		/* deposit value into field */
		void deposit(unsigned lmb, const _T_& v) {
			_value_ = (_value_&~(1<<lmb)) | ((v&1)<<lmb);
		}

		/* deposit value into field */
		void deposit(unsigned lmb, unsigned rmb, const _T_& v) {
			_T_ val1 = ((_T_)-1<<(lmb-rmb))<<1; // we shift twice here so as to 
											// avoid things like useless <<32.
			_T_ val2 = ((_T_)-1)^val1;	// a mask of (lmb-rmb+1) bits
			_T_ val3 = val2&v;
			_value_ = (_value_&~(val2<<rmb)) | (val3<<rmb);
		}

		/* extract value from field */
		_T_ extract(unsigned lmb) const {
			return (_value_>>lmb)&1;
		}

		/* extract value from field */
		_T_ extract(unsigned lmb, unsigned rmb) const {
			_T_ val1 = ((_T_)-1<<(lmb-rmb))<<1;
			_T_ val2 = ((_T_)-1)^val1;	// a mask of (lmb-rmb+1) bits
			return (_value_>>rmb)&val2;
		}

		/* Get the value */
		_T_ val() const {return _value_;}

		/* Output as hexical value */
		std::string hex() const {
			std::string x;
			_strvalue(x, _value_, 16);
			return x;
		}

		/* Output as decimal value */
		std::string dec() const {
			std::string x;
			_strvalue(x, _value_, 10);
			return x;
		}

		/* Output as oct value */
		std::string oct() const {
			std::string x;
			_strvalue(x, _value_, 8);
			return x;
		}

		/* Output as binary value */
		std::string bin() const {
			std::string x;
			_strvalue(x, _value_, 2);
			return x;
		}

	private:
		_T_ _value_;
};

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator + (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	return _bit_t_<_T_,_L_> (x.val() + y.val());
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator - (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x.val() - y.val());
	return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator * (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x.val() * y.val());
	return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator / (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x.val() / y.val());
	return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator % (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x.val() % y.val());
	return _result_;
}

/* for the logic operators, we copy first to a temp,
 * then compute within the temp, this saves us from constructor
 * overhead (left shift followed by right shift).
 * Such savings are only valid for these operators, the others
 * need canonicalization to ensure that the high order bits are good. */
template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator | (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x);
	_result_ |= y;
	return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator & (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x);
	_result_ &= y;
	return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_T_,_L_> operator ^ (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>& y) {
	_bit_t_<_T_,_L_> _result_(x);
	_result_ ^= y;
	return _result_;
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator ~ (const _bit_t_<_T_,_L_>&x)
{
	_bit_t_<_T_,_L_> _result_(~x.val());
    return _result_;
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator - (const _bit_t_<_T_,_L_>&x)
{
	 _bit_t_<_T_,_L_> _result_(-x.val());
     return _result_;
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_WORD_T, 1> operator == (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() == y.val();
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_WORD_T, 1> operator != (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() != y.val();
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_WORD_T, 1> operator <  (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() < y.val();
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_WORD_T, 1> operator >  (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() > y.val();
}

template <class _T_, unsigned _L_> 
inline const _bit_t_<_WORD_T, 1> operator <=  (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() <= y.val();
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_WORD_T, 1> operator >=  (const _bit_t_<_T_,_L_>& x,
											const _bit_t_<_T_,_L_>& y) {
	return x.val() >= y.val();
}

/* right shift operand can also use the saving style since the
 * high order bits will not be screwed. */
template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator >> (const _bit_t_<_T_,_L_>& x,
				   					const _bit_t_<_T_,_L_>&y) {
	_bit_t_<_T_,_L_> _result_(x);
	_result_ >>= y;
	return _result_;
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator >> (const _bit_t_<_T_,_L_>& x,
			   						unsigned y) {
	_bit_t_<_T_,_L_> _result_(x);
	_result_ >>= y;
	return _result_;
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator << (const _bit_t_<_T_,_L_>& x,
			   						const _bit_t_<_T_,_L_>&y) {
	_bit_t_<_T_,_L_> _result_(x.val() << y.val());
	return _result_;
}

template <class _T_, unsigned _L_>
inline const _bit_t_<_T_,_L_> operator << (const _bit_t_<_T_,_L_>& x,
			   						unsigned y) {
	_bit_t_<_T_,_L_> _result_(x.val() << y);
	return _result_;
}

template <class _T_, unsigned _L_>
std::ostream& operator << (std::ostream& os, const _bit_t_<_T_,_L_>& x)
{
	return os << x.val();
}

#include "mpint_type.hpp"

#if 0
/* Since ISO C++ does not support partial specialization of function templates,
 * I use functor for partial specialization. */
template <int _L_, class _T_> class _int_cast_ {
  public:
	_int_t_<_L_> operator()(const _T_& _v) {
		assert(_LONG_INT_IMPLEMENTED);
		return 0;
	}
};

template <int _L_, class _T_> class _uint_cast_ {
  public:
	_uint_t_<_L_> operator()(const _T_& _v) {
		assert(_LONG_INT_IMPLEMENTED);
		return 0;
	}
};
#endif


#define _SPECIALIZE_INT32(_i) \
template <> class _int_t_<_i> : public _bit_t_<_S_WORD_T, _i> { \
  public: \
	typedef _S_WORD_T BASE_T; \
	_int_t_() : _bit_t_<_S_WORD_T, _i>() {} \
	_int_t_(const _S_WORD_T& v) : _bit_t_<_S_WORD_T, _i>(v) {} \
	_int_t_(const _bit_t_<_S_WORD_T, _i>& v) : _bit_t_<_S_WORD_T, _i>(v) {} \
}; \
template <class _T_> class _int_cast_<_i,_T_> { \
	public: \
		_int_t_< _i > operator()(const _T_& _v) { \
			return _int_t_<_i>((_S_WORD_T)(_VAL(_v))); \
		} \
};

_SPECIALIZE_INT32(1);
_SPECIALIZE_INT32(2);
_SPECIALIZE_INT32(3);
_SPECIALIZE_INT32(4);
_SPECIALIZE_INT32(5);
_SPECIALIZE_INT32(6);
_SPECIALIZE_INT32(7);
_SPECIALIZE_INT32(8);
_SPECIALIZE_INT32(9);
_SPECIALIZE_INT32(10);
_SPECIALIZE_INT32(11);
_SPECIALIZE_INT32(12);
_SPECIALIZE_INT32(13);
_SPECIALIZE_INT32(14);
_SPECIALIZE_INT32(15);
_SPECIALIZE_INT32(16);
_SPECIALIZE_INT32(17);
_SPECIALIZE_INT32(18);
_SPECIALIZE_INT32(19);
_SPECIALIZE_INT32(20);
_SPECIALIZE_INT32(21);
_SPECIALIZE_INT32(22);
_SPECIALIZE_INT32(23);
_SPECIALIZE_INT32(24);
_SPECIALIZE_INT32(25);
_SPECIALIZE_INT32(26);
_SPECIALIZE_INT32(27);
_SPECIALIZE_INT32(28);
_SPECIALIZE_INT32(29);
_SPECIALIZE_INT32(30);
_SPECIALIZE_INT32(31);
_SPECIALIZE_INT32(32);

#undef _SPECIALIZE_INT32


#define _SPECIALIZE_UINT32(_i) \
template <> class _uint_t_<_i> : public _bit_t_<_WORD_T, _i> { \
  public: \
	typedef _WORD_T BASE_T; \
	_uint_t_() : _bit_t_<_WORD_T, _i>() {} \
	_uint_t_(const _WORD_T& v) : _bit_t_<_WORD_T, _i>(v) {} \
	_uint_t_(const _bit_t_<_WORD_T, _i>& v) : _bit_t_<_WORD_T, _i>(v) {} \
}; \
template <class _T_> class _uint_cast_<_i,_T_> { \
	public: \
		_uint_t_< _i > operator()(const _T_& _v) { \
			return _uint_t_<_i>((_WORD_T)(_VAL(_v))); \
		} \
};

_SPECIALIZE_UINT32(1);
_SPECIALIZE_UINT32(2);
_SPECIALIZE_UINT32(3);
_SPECIALIZE_UINT32(4);
_SPECIALIZE_UINT32(5);
_SPECIALIZE_UINT32(6);
_SPECIALIZE_UINT32(7);
_SPECIALIZE_UINT32(8);
_SPECIALIZE_UINT32(9);
_SPECIALIZE_UINT32(10);
_SPECIALIZE_UINT32(11);
_SPECIALIZE_UINT32(12);
_SPECIALIZE_UINT32(13);
_SPECIALIZE_UINT32(14);
_SPECIALIZE_UINT32(15);
_SPECIALIZE_UINT32(16);
_SPECIALIZE_UINT32(17);
_SPECIALIZE_UINT32(18);
_SPECIALIZE_UINT32(19);
_SPECIALIZE_UINT32(20);
_SPECIALIZE_UINT32(21);
_SPECIALIZE_UINT32(22);
_SPECIALIZE_UINT32(23);
_SPECIALIZE_UINT32(24);
_SPECIALIZE_UINT32(25);
_SPECIALIZE_UINT32(26);
_SPECIALIZE_UINT32(27);
_SPECIALIZE_UINT32(28);
_SPECIALIZE_UINT32(29);
_SPECIALIZE_UINT32(30);
_SPECIALIZE_UINT32(31);
_SPECIALIZE_UINT32(32);

#undef _SPECIALIZE_UINT32

#define _SPECIALIZE_INT64(_i) \
template <> class _int_t_<_i> : public _bit_t_<_S_DWORD_T, _i> { \
  public: \
	typedef _S_DWORD_T BASE_T; \
	_int_t_() : _bit_t_<_S_DWORD_T, _i>() {} \
	_int_t_(const _S_DWORD_T& v) : _bit_t_<_S_DWORD_T, _i>(v) {} \
	_int_t_(const _bit_t_<_S_DWORD_T, _i>& v) : _bit_t_<_S_DWORD_T, _i>(v) {} \
}; \
template <class _T_> class _int_cast_<_i,_T_> { \
	public: \
		_int_t_< _i > operator()(const _T_& _v) { \
			return _int_t_<_i>((_S_DWORD_T)(_VAL(_v))); \
		} \
};

_SPECIALIZE_INT64(33);
_SPECIALIZE_INT64(34);
_SPECIALIZE_INT64(35);
_SPECIALIZE_INT64(36);
_SPECIALIZE_INT64(37);
_SPECIALIZE_INT64(38);
_SPECIALIZE_INT64(39);
_SPECIALIZE_INT64(40);
_SPECIALIZE_INT64(41);
_SPECIALIZE_INT64(42);
_SPECIALIZE_INT64(43);
_SPECIALIZE_INT64(44);
_SPECIALIZE_INT64(45);
_SPECIALIZE_INT64(46);
_SPECIALIZE_INT64(47);
_SPECIALIZE_INT64(48);
_SPECIALIZE_INT64(49);
_SPECIALIZE_INT64(50);
_SPECIALIZE_INT64(51);
_SPECIALIZE_INT64(52);
_SPECIALIZE_INT64(53);
_SPECIALIZE_INT64(54);
_SPECIALIZE_INT64(55);
_SPECIALIZE_INT64(56);
_SPECIALIZE_INT64(57);
_SPECIALIZE_INT64(58);
_SPECIALIZE_INT64(59);
_SPECIALIZE_INT64(60);
_SPECIALIZE_INT64(61);
_SPECIALIZE_INT64(62);
_SPECIALIZE_INT64(63);
_SPECIALIZE_INT64(64);

#undef _SPECIALIZE_INT64

#define _SPECIALIZE_UINT64(_i) \
template <> class _uint_t_<_i> : public _bit_t_<_DWORD_T, _i> { \
  public: \
	typedef _DWORD_T BASE_T; \
	_uint_t_() : _bit_t_<_DWORD_T, _i>() {} \
	_uint_t_(const _DWORD_T& v) : _bit_t_<_DWORD_T, _i>(v) {} \
	_uint_t_(const _bit_t_<_DWORD_T, _i>& v) : _bit_t_<_DWORD_T, _i>(v) {} \
}; \
template <class _T_> class _uint_cast_<_i,_T_> { \
	public: \
		_uint_t_< _i > operator()(const _T_& _v) { \
			return _uint_t_<_i>((_DWORD_T)(_VAL(_v))); \
		} \
};

_SPECIALIZE_UINT64(33);
_SPECIALIZE_UINT64(34);
_SPECIALIZE_UINT64(35);
_SPECIALIZE_UINT64(36);
_SPECIALIZE_UINT64(37);
_SPECIALIZE_UINT64(38);
_SPECIALIZE_UINT64(39);
_SPECIALIZE_UINT64(40);
_SPECIALIZE_UINT64(41);
_SPECIALIZE_UINT64(42);
_SPECIALIZE_UINT64(43);
_SPECIALIZE_UINT64(44);
_SPECIALIZE_UINT64(45);
_SPECIALIZE_UINT64(46);
_SPECIALIZE_UINT64(47);
_SPECIALIZE_UINT64(48);
_SPECIALIZE_UINT64(49);
_SPECIALIZE_UINT64(50);
_SPECIALIZE_UINT64(51);
_SPECIALIZE_UINT64(52);
_SPECIALIZE_UINT64(53);
_SPECIALIZE_UINT64(54);
_SPECIALIZE_UINT64(55);
_SPECIALIZE_UINT64(56);
_SPECIALIZE_UINT64(57);
_SPECIALIZE_UINT64(58);
_SPECIALIZE_UINT64(59);
_SPECIALIZE_UINT64(60);
_SPECIALIZE_UINT64(61);
_SPECIALIZE_UINT64(62);
_SPECIALIZE_UINT64(63);
_SPECIALIZE_UINT64(64);

#undef _SPECIALIZE_UINT64

/* This wacky (0, exp) is necessary to work around a long-known bug of
 * g++, check out http://gcc.gnu.org/bugs.html
 * for details. Compiler is so hard!
 */
#define _I2I(_i1,_i2,_v) (0, _int_cast_<_i2,_int_t_<_i1> >()(_v))
#define _I2U(_i1,_i2,_v) (0, _uint_cast_<_i2,_int_t_<_i1> >()(_v))
#define _U2I(_i1,_i2,_v) (0, _int_cast_<_i2,_uint_t_<_i1> >()(_v))
#define _U2U(_i1,_i2,_v) (0, _uint_cast_<_i2,_uint_t_<_i1> >()(_v))


#endif
