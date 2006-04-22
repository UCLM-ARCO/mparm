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

#ifndef __FLOAT_TYPE_H__
#define __FLOAT_TYPE_H__

#include <string>
#include <cstdio>
#include "bit_type.hpp"

#define _FLT_T _float_t_<float>
#define _DBL_T _float_t_<double>

#define _FLT_CTR(_v) _float_t_<float>(_v)
#define _DBL_CTR(_v) _float_t_<double>(_v)

#define _BIT2FLT(_t1,_i1,_v) _FLT_T((float)(_VAL((_v))))
#define _BIT2DBL(_t1,_i1,_v) _DBL_T((double)(_VAL((_v))))

#define _DBL2FLT(_v) _FLT_T((float)(_VAL((_v))))
#define _FLT2DBL(_v) _DBL_T((double)(_VAL((_v))))

#define _DBL2BIT(_t1,_i1,_v) _BIT_CTR(_t1,_i1,(_t1)(_VAL((_v))))
#define _FLT2BIT(_t1,_i1,_v) _BIT_CTR(_t1,_i1,(_t1)(_VAL((_v))))

#define _VAL(_a) (_a).val()

/* This wacky (0, exp) is necessary to work around a long-known bug of
 * g++, check out http://gcc.gnu.org/bugs.html
 * for details. Compiler is so hard!
 */
#define _FLT2I(_i,_v) (0,_int_cast_<_i,_FLT_T >()(_v))
#define _FLT2U(_i,_v) (0,_uint_cast_<_i,_FLT_T >()(_v))
#define _DBL2I(_i,_v) (0,_int_cast_<_i,_DBL_T >()(_v))
#define _DBL2U(_i,_v) (0,_uint_cast_<_i,_DBL_T >()(_v))

#define _U2FLT(_i,_v) _FLT_T((float)(_VAL((_v))))
#define _U2DBL(_i,_v) _DBL_T((double)(_VAL((_v))))
#define _I2FLT(_i,_v) _FLT_T((float)(_VAL((_v))))
#define _I2DBL(_i,_v) _DBL_T((double)(_VAL((_v))))


#define _U_TO_F(_v) _uint_to_float(_v)
#define _F_TO_U(_v) _float_to_uint(_v)


/* short bit class template, for i<=32 */
template <class T> class _float_t_ {

	public:

		/* Constructor 1 */
		_float_t_() : _value_(0.0) {}

		/* Constructor 2 */
		_float_t_(const T& v) {
			_value_ =  v;
		}

		/* Assignment operator */
		_float_t_<T>& operator = (const T& v) {
			_value_ = v;
			return *this;
		}

		/* Get the _value_ */
		T val() const {return _value_;}

		std::string sci() {
			char buf[128];
			sprintf(buf, "%e", _value_);
			return buf;
		}

		std::string fix() {
			char buf[128];
			sprintf(buf, "%f", _value_);
			return buf;
		}

	private:
		T _value_;
};

template <class T> 
inline const _float_t_<T> operator + (const _float_t_<T>& x,
			   						const _float_t_<T>& y) {
	_float_t_<T> _result_(x.val() + y.val());
	return _result_;
}

template <class T> 
inline const _float_t_<T> operator - (const _float_t_<T>& x,
			   						const _float_t_<T>& y) {
	_float_t_<T> _result_(x.val() - y.val());
	return _result_;
}

template <class T> 
inline const _float_t_<T> operator * (const _float_t_<T>& x,
			   						const _float_t_<T>& y) {
	_float_t_<T> _result_(x.val() * y.val());
	return _result_;
}

template <class T> 
inline const _float_t_<T> operator / (const _float_t_<T>& x,
			   						const _float_t_<T>& y) {
	_float_t_<T> _result_(x.val() / y.val());
	return _result_;
}

template <class T>
inline const _float_t_<T> operator - (const _float_t_<T>&x)
{
	_float_t_<T> _result_(-x.val());
	return _result_;
}

template <class T> 
inline const _bit_t_<unsigned, 1> operator == (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() == y.val();
}

template <class T> 
inline const _bit_t_<unsigned, 1> operator != (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() != y.val();
}

template <class T> 
inline const _bit_t_<unsigned, 1> operator <  (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() < y.val();
}

template <class T>
inline const _bit_t_<unsigned, 1> operator >  (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() > y.val();
}

template <class T> 
inline const _bit_t_<unsigned, 1> operator <=  (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() <= y.val();
}

template <class T>
inline const _bit_t_<unsigned, 1> operator >=  (const _float_t_<T>& x,
											const _float_t_<T>& y) {
	return x.val() >= y.val();
}


inline const _uint_t_<32> _float_to_uint(const _float_t_<float>& x) {
	union {
		float a;
		_WORD_T b;
	} u;
	u.a = x.val();
	return u.b;
}

inline const _uint_t_<64> _float_to_uint(const _float_t_<double>& x) {
	union {
		double a;
		_DWORD_T b;
	} u;
	u.a = x.val();
	return u.b;
}

inline const _float_t_<float> _uint_to_float(const _uint_t_<32>& x) {
	union {
		float a;
		_WORD_T b;
	} u;
	u.b = x.val();
	return u.a;
}

inline const _float_t_<double> _uint_to_float(const _uint_t_<64>& x) {
	union {
		double a;
		_DWORD_T b;
	} u;
	u.b = x.val();
	return u.a;
}

#endif
