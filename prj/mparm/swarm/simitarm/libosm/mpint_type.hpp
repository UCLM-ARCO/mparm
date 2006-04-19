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

#include "mpz_base.hpp"

/* The general int type of width >64 is implemented here. */
template <unsigned _Len_, bool _Sign_> class _mpint_t_ {

	public:
		_mpint_t_() : m() {}
		_mpint_t_(const _mpz_base_<_Len_,_Sign_>& v) : m(v) {}

		_mpint_t_(char *v) : m(v) {}

		_mpint_t_(int32_t v)  : m(v) {}
		_mpint_t_(uint32_t v) : m(v) {}

		_mpint_t_(int64_t v)  : m(v) {}
		_mpint_t_(uint64_t v) : m(v) {}

		_mpint_t_(float v)  : m((double)v) {}
		_mpint_t_(double v) : m(v) {}

		~_mpint_t_() {}

		const _mpz_base_<_Len_,_Sign_> val() const {return m;}

#if 0
/* this is useful and sound (I believe) code.
   Somehow, some c++ compilers don't like it. Since it is not
   used in this package, I commented it out. */
		template <unsigned _L, bool _S>
		void deposit(unsigned lmb, unsigned rmb, const _mpz_base_<_L,_S>& x) {
			m.set_slice(x, lmb-rmb+1, rmb);
		}
#endif

		void deposit(unsigned lmb, unsigned rmb, uint32_t x) {
			m.set_slice(x, lmb-rmb+1, rmb);
		}

		void deposit(unsigned lmb, unsigned rmb, uint64_t x) {
			m.set_slice(x, lmb-rmb+1, rmb);
		}

		void deposit(unsigned lmb, unsigned rmb, int32_t x) {
			m.set_slice((uint32_t)x, lmb-rmb+1, rmb);
		}

		void deposit(unsigned lmb, unsigned rmb, int64_t x) {
			m.set_slice((uint64_t)x, lmb-rmb+1, rmb);
		}

		const _mpz_base_<_Len_,_Sign_> extract(unsigned lmb) const {
			_mpz_base_<_Len_,_Sign_> m1((uint32_t)(m.tst_bit(lmb)));
			return m1;
		}

		const _mpz_base_<_Len_,_Sign_> 
			extract(unsigned lmb, unsigned rmb) const {
			return m.get_slice(rmb, lmb-rmb+1);
		}

		/* conversion to string */
		std::string hex() const {
			std::string v;
			m.get_str(v, 16);
			return v;
		}

		std::string dec() const {
			std::string v;
			m.get_str(v, 10);
			return v;
		}

		std::string oct() const {
			std::string v;
			m.get_str(v, 8);
			return v;
		}

		std::string bin() const {
			std::string v;
			m.get_str(v, 2);
			return v;
		}

	protected:
		_mpz_base_<_Len_, _Sign_> m;
};

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator == (const _mpint_t_<_L_,_S_>& x,
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() == y.val();
}															 

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator != (const _mpint_t_<_L_,_S_>& x,
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() != y.val();								
}															 

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator < (const _mpint_t_<_L_,_S_>& x,
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() < y.val();								 
}															 

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator > (const _mpint_t_<_L_,_S_>& x,
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() > y.val();								 
}															 

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator <= (const _mpint_t_<_L_,_S_>& x, 
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() <= y.val();								
}															 

template <unsigned _L_, bool _S_>
inline const _bit_t_<_WORD_T, 1> operator >= (const _mpint_t_<_L_,_S_>& x, 
											const _mpint_t_<_L_,_S_>& y) {	
	return x.val() >= y.val();								
}															 

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator + (const _mpint_t_<_L_,_S_>& x,
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() + y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator - (const _mpint_t_<_L_,_S_>& x,
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() - y.val();
}															 

template <unsigned _L_, bool _S_>							
inline const _mpint_t_<_L_,_S_> operator * (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() * y.val();
}															 

template <unsigned _L_, bool _S_>							
inline const _mpint_t_<_L_,_S_> operator / (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() / y.val();
}															 

template <unsigned _L_, bool _S_>							
inline const _mpint_t_<_L_,_S_> operator % (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() % y.val();
}

template <unsigned _L_, bool _S_>							
inline const _mpint_t_<_L_,_S_> operator | (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() | y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator & (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() & y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator ^ (const _mpint_t_<_L_,_S_>& x,		
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() ^ y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator ~ (const _mpint_t_<_L_,_S_>& x) {
	return ~x.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator - (const _mpint_t_<_L_,_S_>& x) {
	return -x.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator >> (const _mpint_t_<_L_,_S_>& x,
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() >> y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator >> (const _mpint_t_<_L_,_S_>& x,
									unsigned y) {
	return x.val() >> y;
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator << (const _mpint_t_<_L_,_S_>& x,
									const _mpint_t_<_L_,_S_>& y) {
	return x.val() << y.val();
}

template <unsigned _L_, bool _S_>
inline const _mpint_t_<_L_,_S_> operator << (const _mpint_t_<_L_,_S_>& x,
									unsigned y) {
	return x.val() << y;
}

template <unsigned _Len_> 
class _int_t_ : public _mpint_t_<_Len_, true> {
	public :
		_int_t_() : _mpint_t_<_Len_,true>() {}
		_int_t_(const _int_t_& v) : _mpint_t_<_Len_,true>(v.val()) {}
		_int_t_(const char *v) : _mpint_t_<_Len_,true>(v) {}
		_int_t_(int32_t v)  : _mpint_t_<_Len_,true>(v) {}
		_int_t_(int64_t v)  : _mpint_t_<_Len_,true>(v) {}
		_int_t_(uint32_t v) : _mpint_t_<_Len_,true>(v) {}
		_int_t_(uint64_t v) : _mpint_t_<_Len_,true>(v) {}
		_int_t_(const _mpint_t_<_Len_,true> &v) : _mpint_t_<_Len_,true>(v) {}
		_int_t_(const _mpz_base_<_Len_,true> &v) : _mpint_t_<_Len_,true>(v) {}
		~_int_t_() {}
};

template <unsigned _Len_>
class _uint_t_ : public _mpint_t_<_Len_, false> {
	public :
		_uint_t_() : _mpint_t_<_Len_,false>() {}
		_uint_t_(const _uint_t_& v) : _mpint_t_<_Len_,false>(v.val()) {}
		_uint_t_(const char *v)   : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(int32_t v)  : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(int64_t v)  : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(uint32_t v)  : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(uint64_t v)  : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(const _mpint_t_<_Len_,false> &v) : _mpint_t_<_Len_,false>(v) {}
		_uint_t_(const _mpz_base_<_Len_,false> &v): _mpint_t_<_Len_,false>(v) {}
		~_uint_t_() {}
};

template <unsigned _i, class _T_> class _uint_cast_ {
	public: 
		_uint_t_< _i > operator()(const _T_& _v) { 
			return _uint_t_<_i>(_mpz_base_<_i,false>(_VAL(_v))); 
		} 
};

template <unsigned _i, class _T_> class _int_cast_ {
	public: 
		_int_t_< _i > operator()(const _T_& _v) { 
			return _int_t_<_i>(_mpz_base_<_i,true>(_VAL(_v))); 
		} 
};
