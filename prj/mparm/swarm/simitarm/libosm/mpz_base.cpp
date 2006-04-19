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

#include <gmp.h>
#include "mpz_base.hpp"

#if __GNU_MP_VERSION >= 4

using std::string;

static uint32_t _mpz_base_rescale(uint32_t val, unsigned msb, bool sign)
{
	/* msb has the range from 1 to 31 */

	/* if signed integer, test highest order bit */
	uint32_t shft = val << (31-msb);
	if (sign)
		return ((int32_t)shft >> (31-msb));
	/* zero extend padding bits */
	else
		return (shft >> (31-msb));
}


/** A local structure used to interfacing gmp library. */
class _gmp_translator {

	public:

		/** Constructor. */
		_gmp_translator() {
			mpz_init2(val, 256);
			mpz_init2(val2, 256);
		}

		/** Destructor. */
		~_gmp_translator() {
			mpz_clear(val);
			mpz_clear(val2);
		}

		_gmp_translator& operator *= (const _gmp_translator& op2) {
			mpz_mul(val, val, op2.val);
			return *this;
		}

		_gmp_translator& operator /= (const _gmp_translator& op2) {
			mpz_tdiv_q(val, val, op2.val);
			return *this;
		}

		_gmp_translator& operator %= (const _gmp_translator& op2) {

			if (mpz_cmp_ui(val, 0L) < 0) {
				mpz_mod(val, val, op2.val); 
				// mpz_mod result always non-negative
				// for c native types, mod result has the same sign as divident
				// so we adjust the result here
				if (mpz_cmp_ui(val, 0L) > 0) { 
					if (mpz_cmp_ui(op2.val, 0L) > 0)
						mpz_sub(val, val, op2.val);
					else
						mpz_add(val, val, op2.val);
				}
			}
			else
				mpz_mod(val, val, op2.val);

			return *this;
		}

		void set_str(const char *str);

		void set_double(double v) {mpz_init_set_d(val, v);}

		void set_value(const uint32_t *v, unsigned width, bool sign);

		/* destructive read, will destroy val. */
		void get_value(uint32_t *v, unsigned width, bool sign);

		void get_double(double& v);

		void get_str(string& v, unsigned base, unsigned width);


	private:
		mpz_t val, val2;
};

void _gmp_translator::get_str(string& v, unsigned base, unsigned width)
{
	v = "";
	if (mpz_cmp_ui(val, 0L) < 0) {
		if (base==10) {
			mpz_neg(val, val);
			v += '-';
		}
		else {
			mpz_set_ui(val2, 0L); //clear the value
			mpz_setbit(val2, width);
			mpz_sub_ui(val2, val2, 1L);
			mpz_and(val, val, val2);
		}
	}

	if (base==2) v = "0b";
	else if (base==8) v = "0";
	else if (base==16) v=  "0x";

	int n = mpz_sizeinbase(val, base);
	if (n>1000) {
		char *b = new char[n+2];
		mpz_get_str(b, base, val);
		v += b;
		delete [] b;
	}
	else {
		char b[1024];
		mpz_get_str(b, base, val);
		v += b;
	}
}

void _gmp_translator::get_double(double& v)
{
	v = mpz_get_d(val);
}

void _gmp_translator::set_str(const char *bv)
{
	
	// create unsigned mpzcl with wordlength as require to represen bv
	// which is in one of the following formats:
	// - decimal:		 [0-9]+
	// - hexadecimal: 0[xX][0-9a-fA-F]+
	// - octal:			 0[0-7]+
	// - binary:			0[bB][01]+

	// empty string
	if (bv[0] == 0) {
		mpz_set_ui(val, 0);
		return;
	}

	bool neg = false;
	if (bv[0] == '-') {
		bv++;
		neg = true;
	}

	// check base
	if (bv[0] == '0') {
		if ((bv[1] == 'x') || (bv[1] == 'X')) //hexadecimal
			mpz_init_set_str(val, bv+2, 16);
		else if ((bv[1] == 'b') || (bv[1] == 'B')) // binary
			mpz_init_set_str(val, bv+2, 2);
		else	// is octal string
			mpz_init_set_str(val, bv+1, 8);
	} else // is decimal string
		mpz_init_set_str(val, bv, 10);

	if (neg) mpz_neg(val, val);
}

void _gmp_translator::set_value(const uint32_t *v, unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;
	if (sign)
		//assume that already sign extended!!
		mpz_set_si(val, (signed long)v[len-1]);
	else 
		mpz_set_ui(val, v[len-1]);

	for (unsigned ii=1; ii<len; ii++) {
		mpz_mul_2exp(val, val, 32);
		mpz_add_ui(val, val, v[len-ii-1]);
	}
}

void _gmp_translator::get_value(uint32_t *v, unsigned width, bool sign) 
{
	// Since the function mpz_get_ui returns absolute value,
	// we negate negative value first.
	bool is_neg = mpz_cmp_ui(val, 0L)<0;
	if (is_neg) mpz_neg(val, val);

	unsigned len = (width+31) >> 5;
	for (unsigned ii=0; ii<len; ii++) {
		v[ii] = mpz_get_ui(val);
		mpz_tdiv_q_2exp(val, val, 32);
	}

	/* negate the value if negative, complement then +1 */
	if (is_neg) {
		uint32_t carry = 1;
		for (unsigned ii=0; ii<len; ii++) {
			uint32_t temp = (~v[ii]) + carry;
			carry = (temp<carry)?1:0;	// see if carry generated
			v[ii] = temp;	// this should not generate carry any more
		}
	}

	if (width&31) 
		v[len-1] = _mpz_base_rescale(v[len-1], width&31, sign);
}

static _gmp_translator _gmp_op1, _gmp_op2;

void _mpz_base_set(uint32_t *dest, unsigned dwidth, bool dsign,
				   const uint32_t *src, unsigned swidth, bool ssign)
{
	unsigned dlen = (dwidth+31) >> 5;
	unsigned slen = (swidth+31) >> 5;
	unsigned ii;

	for (ii=0; ii<dlen && ii<slen; ii++)
		dest[ii] = src[ii];

	/* sign extend negative value */
	unsigned sbits = ssign?((int32_t)src[slen-1]>>31):0;
	for (; ii<dlen; ii++) dest[ii] = sbits;

	if (dwidth&31)
		dest[dlen-1] = _mpz_base_rescale(dest[dlen-1], dwidth&31, dsign);
}

void _mpz_base_set_str(uint32_t *dest, unsigned dw, bool ds, const char *str)
{
	_gmp_op1.set_str(str);
	_gmp_op1.get_value(dest, dw, ds);
}

void _mpz_base_set_d(uint32_t *dest, unsigned dw, bool ds, double v)
{
	_gmp_op1.set_double(v);
	_gmp_op1.get_value(dest, dw, ds);
}

void _mpz_base_add(uint32_t *dest, const uint32_t *src,
					unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;
	uint32_t carry = 0;

	for (unsigned ii=0; ii<len; ii++) {

		uint32_t temp = dest[ii]+src[ii];

		if (temp<dest[ii]) { 		// carry generated
			dest[ii] = temp + carry;// this should not generate carry any more
									// since temp is smaller than max
			carry  = 1;
		}
		else {
			dest[ii] = temp + carry;
			carry = (dest[ii]<carry)?1:0;	// see if carry generated
		}
	}

	if (width&31) 
		dest[len-1] = _mpz_base_rescale(dest[len-1], width&31, sign);
}

void _mpz_base_sub(uint32_t *dest, const uint32_t *src,
					unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;
	uint32_t carry = 0;

	for (unsigned ii=0; ii<len; ii++) {

		uint32_t temp = dest[ii] - src[ii];
		if (temp > dest[ii]) {		// borrow generated
			dest[ii] = temp - carry;//this should not generate borrow any more
									//since temp is non-zero
			carry  = 1;
		}
		else {
			dest[ii] = temp - carry;
			carry = (dest[ii]>temp)?1:0;	// see if carry generated
		}
	}

	if (width&31) 
		dest[len-1] = _mpz_base_rescale(dest[len-1], width&31, sign);
}

int	_mpz_base_cmp(const uint32_t *src1, const uint32_t *src2, 
				unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;
	if (sign) {
		if ((int32_t)src1[len-1]>=0) {
			if ((int32_t)src2[len-1]<0)
				return 1;
		}
		else if ((int32_t)src2[len-1]>=0)
			return -1;
	}

	/* same-sign comparison simple */
	for (unsigned ii=0; ii<len; ii++) {
		if (src1[len-1-ii]>src2[len-1-ii]) return 1;
		else if (src1[len-1-ii]<src2[len-1-ii]) return -1;
	}
	return 0;
}

void _mpz_base_mul(uint32_t *dest, const uint32_t *src, 
					unsigned width, bool sign)
{
	_gmp_op1.set_value(dest, width, sign);
	_gmp_op2.set_value(src, width, sign);
	_gmp_op1 *= _gmp_op2;
	_gmp_op1.get_value(dest, width, sign);
}

void _mpz_base_div(uint32_t *dest, const uint32_t *src, 
					unsigned width, bool sign)
{
	_gmp_op1.set_value(dest, width, sign);
	_gmp_op2.set_value(src, width, sign);
	_gmp_op1 /= _gmp_op2;
	_gmp_op1.get_value(dest, width, sign);
}

void _mpz_base_mod(uint32_t *dest, const uint32_t *src, 
					unsigned width, bool sign)
{
	_gmp_op1.set_value(dest, width, sign);
	_gmp_op2.set_value(src, width, sign);
	_gmp_op1 %= _gmp_op2;
	_gmp_op1.get_value(dest, width, sign);
}

void _mpz_base_lsh(uint32_t *dest, unsigned op2, unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;

	unsigned chunk = op2 >> 5;
	unsigned piece = op2 & 31;
	unsigned ii;

	if (piece==0) {
		for (ii=len; ii>chunk; ii--)
			dest[ii-1] = dest[ii-chunk-1];

		for (ii=0; ii<chunk && ii<len; ii++)
			dest[ii] = 0;
	}
	else {
		for (ii=len-1; ii>chunk; ii--) {
			dest[ii] = dest[ii-chunk] << piece;
			dest[ii] |= dest[ii-chunk-1] >> (32-piece);
		}
		if (ii==chunk) {	// ii may be less than chunk if chunk is large
			dest[ii] = dest[0] << piece;
		}
		for (ii=0; ii<chunk && ii<len; ii++)
			dest[ii] = 0;
	}

	if (width&31) 
		dest[len-1] = _mpz_base_rescale(dest[len-1], width&31, sign);
}

void _mpz_base_rsh(uint32_t *dest, unsigned op2, unsigned width, bool sign)
{
	unsigned len = (width+31) >> 5;
	uint32_t sbits = sign?((int32_t)dest[len-1]>>31):0;

	unsigned chunk = op2 >> 5;
	unsigned piece = op2 & 31;
	unsigned ii;

	if (piece==0) {
		for (ii=0; ii+chunk<len; ii++)
			dest[ii] = dest[ii+chunk];

		for (; ii<len; ii++)
			dest[ii] = sbits;
	}
	else {
		for (ii=0; ii+chunk+1<len; ii++) {
			dest[ii] = dest[ii+chunk] >> piece;
			dest[ii] |= dest[ii+chunk+1] << (32-piece);
		}
		if (ii+chunk+1==len) {
			dest[ii++] = (dest[len-1] >> piece) | (sbits << (32-piece));
		}
		for (; ii<len; ii++) dest[ii] = sbits;
	}

	/* no need to rescale in this case. */
}

/* set a value from rmb bit to rmb+dwidth-1 */
void _mpz_base_set_slice(uint32_t *dest, unsigned width, bool sign,
						 const uint32_t *src, unsigned nbits, unsigned rmb)
{
	// the user should assure this
	assert(nbits+rmb<=width);

	unsigned len = (width+31) >> 5;
	if (nbits==width) {
		for (unsigned ii=0; ii<len; ii++) dest[ii] = src[ii];	
		return;
	}

	unsigned nwords = nbits >> 5;
	unsigned nleft  = nbits & 31;

	unsigned chunk = rmb >> 5;
	unsigned piece = rmb & 31;

	unsigned head = (nbits+rmb-1) >> 5; //msb
	unsigned hoff = (nbits+rmb-1) & 31; //msb

	unsigned ii;

	if (piece==0) {
		for (ii=0; ii<nwords; ii++)
			dest[ii+chunk] = src[ii];
		if (nleft) {
			uint32_t mask = (uint32_t)-1<<nleft;
			dest[ii+chunk] &= mask;
			dest[ii+chunk] |= src[ii]&(~mask);
		}
	}
	else if (head == chunk) { //if everything occurs in one word
		uint32_t mask = (uint32_t)-1<<piece;
		uint32_t mask1 = ((uint32_t)-1<<hoff)<<1;
		dest[chunk] &= ~mask | mask1;
		dest[chunk] |= (src[0]<<piece) & (~mask1);
	}
	else { //spans more than one word
		uint32_t mask = (uint32_t)-1<<piece;
		dest[chunk] &= ~mask;
		dest[chunk] |= src[0]<<piece;
		for (ii=chunk+1; ii<head; ii++) {
			dest[ii] = src[ii-chunk] << piece;
			dest[ii] |= src[ii-1-chunk] >> (32-piece);
		}
		uint32_t mask1 = ((uint32_t)-1<<hoff)<<1;
		dest[head] &= mask1;
		uint32_t temp = src[head-chunk-1] >> (32-piece);
		if (piece<hoff+1) temp |= src[head-chunk] << piece;
		dest[ii+chunk] |= temp & (~mask1);
	}

	/* if the highest order bit affected, rescale */
	if (nbits+rmb==width && (width&31))
		dest[len-1] = _mpz_base_rescale(dest[len-1], width&31, sign);
}


void _mpz_base_get_slice(uint32_t *dest, unsigned width,
						 const uint32_t *src, unsigned nbits, unsigned rmb)
{
	// the user should assure this
	assert(nbits+rmb<=width);

	unsigned len = (width+31) >> 5;

	if (nbits==width) {
		for (unsigned ii=0; ii<len; ii++) dest[ii] = src[ii];	
		return;
	}

	unsigned nwords = nbits >> 5;
	unsigned nleft  = nbits & 31;

	unsigned chunk = rmb >> 5;
	unsigned piece = rmb & 31;

	unsigned ii;

	if (piece==0) {
		for (ii=0; ii<nwords; ii++)
			dest[ii] = src[ii+chunk];
		if (nleft) {
			uint32_t mask = (uint32_t)-1<<nleft;
			dest[ii] = src[ii+chunk] & (~mask);
			ii++;
		}
		for (; ii<len; ii++) dest[ii] = 0;
	}
	else {
		for (ii=0; ii<nwords; ii++) {
			dest[ii] = src[ii+chunk] >> piece;
			dest[ii] |= src[ii+chunk+1] << (32-piece);
		}
		if (nleft) {
			uint32_t mask = (uint32_t)-1<<nleft;
			uint32_t temp = src[ii+chunk] >> piece;
			if (nleft>(32-piece)) temp |= src[ii+chunk+1] << (32-piece);
			dest[ii] = temp & (~mask);
			ii++;
		}
		for (; ii<len; ii++) dest[ii] = 0;
	}
	/* no need to scale since nbits<width here */
}

double _mpz_base_get_d(const uint32_t *src, unsigned width, bool sign)
{
	double ret;
	_gmp_op1.set_value(src, width, sign);
	_gmp_op1.get_double(ret);
	return ret;
}

void _mpz_base_get_str(string& v, unsigned base,
						const uint32_t *src, unsigned width, bool sign)
{
	_gmp_op1.set_value(src, width, sign);
	_gmp_op1.get_str(v, base, width);
}


void _strvalue(string& s, uint32_t v, unsigned base)
{
	_mpz_base_<32,false> x(v);
	x.get_str(s, base);
}

void _strvalue(string& s, uint64_t v, unsigned base)
{
	_mpz_base_<64,false> x(v);
	x.get_str(s, base);
}

void _strvalue(string& s, int32_t v, unsigned base)
{
	_mpz_base_<32,true> x(v);
	x.get_str(s, base);
}

void _strvalue(string& s, int64_t v, unsigned base)
{
	_mpz_base_<64,true> x(v);
	x.get_str(s, base);
}

#endif

