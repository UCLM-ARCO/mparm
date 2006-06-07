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
#ifndef _MPZ_BASE_H_
#define _MPZ_BASE_H_

/* gmp is hard to use for our purpose
 * The function mpz_get_ui returns absolute value.
 * The function mpz_get_si returns weird things when overflow.
 * The function mpz_mod always returns non-negative value, in contrast to
 *		 C operator %.
 */
// wrapper class for GNU multiprecision library
#include "config.h"
#include "bittypes.h"
#include <string>

extern void _mpz_base_set(uint32_t *dest, unsigned dwidth, bool dsign,
				const uint32_t *src, unsigned swidth, bool ssign);
extern void _mpz_base_set_str(uint32_t *dest, unsigned dw,
				bool ds, const char *str);
extern void _mpz_base_set_d(uint32_t *dest, unsigned dw, bool ds, double v);
extern double _mpz_base_get_d(const uint32_t *src, unsigned width, bool sign);
extern void _mpz_base_add(uint32_t *dest, const uint32_t *src,	   
					unsigned width, bool sign);
extern void _mpz_base_sub(uint32_t *dest, const uint32_t *src,	   
					unsigned width, bool sign);
extern int  _mpz_base_cmp(const uint32_t *src1, const uint32_t *src2,	   
					unsigned width, bool sign);
extern void _mpz_base_mul(uint32_t *dest, const uint32_t *src,	   
					unsigned width, bool sign);
extern void _mpz_base_div(uint32_t *dest, const uint32_t *src,	   
					unsigned width, bool sign);
extern void _mpz_base_mod(uint32_t *dest, const uint32_t *src,	   
					unsigned width, bool sign);
extern void _mpz_base_lsh(uint32_t *dest, unsigned op2,
					unsigned width, bool sign);
extern void _mpz_base_rsh(uint32_t *dest, unsigned op2,
					unsigned width, bool sign);
extern void _mpz_base_set_slice(uint32_t *dest, unsigned width, bool sign,
					const uint32_t *src, unsigned nbits, unsigned rmb);
extern void _mpz_base_get_slice(uint32_t *dest, unsigned width,	  
					const uint32_t *src, unsigned nbits, unsigned rmb);
extern void _mpz_base_get_str(std::string& v, unsigned base,			  
					const uint32_t *src, unsigned width, bool sign);

/** This will work only when width >= 64 */
template <unsigned width, bool sign>
class _mpz_base_ {

private:

	/* the storage for long integer. */
	uint32_t value[(width+31)>>5];

public:

	_mpz_base_() {}

	_mpz_base_(const _mpz_base_& v) {
		for (unsigned i=0; i<(width+31)>>5; i++) value[i]=v.value[i];
	}

	/* This is equivalent to a cast */
	template <unsigned w, bool s>
	_mpz_base_(const _mpz_base_<w,s>& v) {
		_mpz_base_set(value, width, sign, v.get_value(), w, s);
	}

	_mpz_base_(const char *valuestr) {
		_mpz_base_set_str(value, width, sign, valuestr);
	}

	_mpz_base_(int32_t v) {
		uint32_t sbits = v>>31;
		for (unsigned i=1; i<(width+31)>>5; i++) value[i]=sbits;
		value[0] = v;
	}

	_mpz_base_(uint32_t v) {
		for (unsigned i=1; i<(width+31)>>5; i++) value[i]=0;;
		value[0] = v;
	}

	_mpz_base_(int64_t v) {
		int32_t x = (int32_t)(v>>32);
		uint32_t sbits = x>>31;
		for (unsigned i=2; i<(width+31)>>5; i++) value[i]=sbits;
		value[0] = (uint32_t)v;
		value[1] = (uint32_t)x;
	}

	_mpz_base_(uint64_t v) {
		for (unsigned i=2; i<(width+31)>>5; i++) value[i]=0;;
		value[0] = (uint32_t)v;
		value[1] = (uint32_t)(v>>32);
	}

	_mpz_base_(double v) {
		_mpz_base_set_d(value, width, sign, v);
	}

	_mpz_base_(float v) {
		_mpz_base_set_d(value, width, sign, (double)v);
	}

	_mpz_base_& operator = (const _mpz_base_ &	v) {
		for (unsigned i=0; i<(width+31)>>5; i++) value[i]=v.value[i];
		return *this;
	}

	operator int32_t() const {
		return (int32_t)value[0];
	}

	operator uint32_t () const {
		return value[0];
	}

	operator int64_t() const {
		return ((int64_t)value[1]<<32) | value[0];
	}

	operator uint64_t() const {
		return ((uint64_t)value[1]<<32) | value[0];
	}

	operator double () const {
		return _mpz_base_get_d(value, width, sign);
	}

	operator float () const {
		return (float)_mpz_base_get_d(value, width, sign);
	}

	~_mpz_base_() {}

	const uint32_t *get_value() const {return value;}
	unsigned get_width() const {return width;}
	bool get_sign() const {return sign;}

	/* Arithmetic operators */
	_mpz_base_& operator += (const _mpz_base_ &o2) {
		_mpz_base_add(value, o2.value, width, sign);
		return *this;
	}

	_mpz_base_& operator -= (const _mpz_base_ &o2) {
		_mpz_base_sub(value, o2.value, width, sign);
		return *this;
	}

	_mpz_base_& operator *= (const _mpz_base_ &o2) {
		_mpz_base_mul(value, o2.value, width, sign);
		return *this;
	}

	_mpz_base_& operator /= (const _mpz_base_ &o2) {
		_mpz_base_div(value, o2.value, width, sign);
		return *this;
	}

	_mpz_base_& operator %= (const _mpz_base_ &o2) {
		_mpz_base_mod(value, o2.value, width, sign);
		return *this;
	}

	_mpz_base_& operator <<= (unsigned o2) {
		_mpz_base_lsh(value, o2, width, sign);
		return *this;
	}

	_mpz_base_& operator <<= (const _mpz_base_ &o2) {
		_mpz_base_lsh(value, o2.value[0], width, sign);
		return *this;
	}

	_mpz_base_& operator >>= (unsigned o2) {
		_mpz_base_rsh(value, o2, width, sign);
		return *this;
	}

	_mpz_base_& operator >>= (const _mpz_base_ &o2) {
		_mpz_base_rsh(value, o2.value()[0], width, sign);
		return *this;
	}

	_mpz_base_& operator &= (const _mpz_base_ &o2) {
		for (unsigned i=0; i<(width+31)>>5; i++) value[i]&=o2.value[i];
		return *this;
	}

	_mpz_base_& operator |= (const _mpz_base_ &o2) {
		for (unsigned i=0; i<(width+31)>>5; i++) value[i]|=o2.value[i];
		return *this;
	}

	_mpz_base_& operator ^= (const _mpz_base_ &o2) {
		for (unsigned i=0; i<(width+31)>>5; i++) value[i]^=o2.value[i];
		return *this;
	}

	friend const _mpz_base_ operator ~<> (const _mpz_base_& x);

	// MISCELLANEOUS OPERATORS
	bool tst_bit(unsigned bitnum) const {
		return (value[bitnum>>5]>>(bitnum&31)) & 1;
	}

	void set_bit(unsigned bitnum) {
		value[bitnum>>5] |= 1<<(bitnum&31);
	}

	void clr_bit(unsigned bitnum) {
		value[bitnum>>5] &= ~((uint32_t)1<<(bitnum&31));
	}

	void set_bit_as(unsigned bitnum, uint32_t v) {
		value[bitnum>>5] &= ~((uint32_t)1<<(bitnum&31));
		value[bitnum>>5] |= (v&1)<<(bitnum&31);
	}

	void set_slice(uint32_t o2, unsigned nbits, unsigned rmb) {
		_mpz_base_set_slice(value, width, sign, &o2, nbits, rmb);
	}

	void set_slice(uint64_t o2, unsigned nbits, unsigned rmb) {
		uint32_t v[2] = {(uint32_t)o2, (uint32_t)(o2>>32)};
		_mpz_base_set_slice(value, width, sign, v, nbits, rmb);
	}

	/* to deposit o2 into the word from bit start */
	template <unsigned w, bool s>
	void set_slice(const _mpz_base_<w,s>& o2, unsigned nbits, unsigned rmb) {
		_mpz_base_set_slice(value, width, sign, o2.get_value(), nbits, rmb);
	}

	/* to extract a portion from bit start of length count */
	const _mpz_base_ get_slice(unsigned start, unsigned count) const {
		_mpz_base_ x;
		_mpz_base_get_slice(x.value, width, value, count, start);
		return x;
	}

	// I/O
	void get_str(std::string &v, unsigned base) const {
		_mpz_base_get_str(v, base, value, width, sign);
	}
};

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator - (const _mpz_base_<w,s>& x) {
	_mpz_base_<w,s> m(0u);
	m -= x;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator + (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m += y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator - (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m -= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator * (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m *= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator / (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m /= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator % (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m %= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator << (const _mpz_base_<w,s> & x,
											const unsigned int o2) {
	_mpz_base_<w,s> m(x);
	m <<= o2;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator << (const _mpz_base_<w,s>& x,
											const _mpz_base_<w,s>& o2) {
	_mpz_base_<w,s> m(x);
	m <<= o2;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator >> (const _mpz_base_<w,s>& x,
											const unsigned int o2) {
	_mpz_base_<w,s> m(x);
	m >>= o2;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator >> (const _mpz_base_<w,s>& x,
											const _mpz_base_<w,s>& o2) {
	_mpz_base_<w,s> m(x);
	m >>= o2;
	return m;
}

template <unsigned w, bool s>
inline bool operator == (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) == 0);
}

template <unsigned w, bool s>
inline bool operator != (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) != 0);
}

template <unsigned w, bool s>
inline bool operator <= (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) <= 0);
}

template <unsigned w, bool s>
inline bool operator >= (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) >= 0);
}

template <unsigned w, bool s>
inline bool operator < (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) < 0);
}

template <unsigned w, bool s>
inline bool operator > (const _mpz_base_<w,s>& x, const _mpz_base_<w,s>& y) {
	return (_mpz_base_cmp(x.get_value(), y.get_value(), w, s) > 0);
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator & (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m &= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator | (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m |= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator ^ (const _mpz_base_<w,s>& x,
										 const _mpz_base_<w,s>& y) {
	_mpz_base_<w,s> m(x);
	m ^= y;
	return m;
}

template <unsigned w, bool s>
inline const _mpz_base_<w,s> operator ~ (const _mpz_base_<w,s>& x) {
	_mpz_base_<w,s> m;
	unsigned len = (w+31)>>5;
	for (unsigned i=0; i<len; i++) m.value[i]=~x.value[i];
	/* if unsigned, the highest order bits should get cleared */
	if (!s) m.value[len-1] = (m.value[len-1]<<(w&31)) >> (w&31);
	return m;
}

#endif
