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

#ifndef __AGGR_TYPE_H__
#define __AGGR_TYPE_H__

#define _TUPLE_T(_t1,_t2) _tuple_t_<_t1,_t2>
#define _TRIPLE_T(_t1,_t2,_t3) _triple_t_<_t1,_t2,_t3>
#define _QUADRUPLE_T(_t1,_t2,_t3,_t4) _quadruple_t_<_t1,_t2,_t3,_t4>
#define _QUINTUPLE_T(_t1,_t2,_t3,_t4,_t5) _quintuple_t_<_t1,_t2,_t3,_t4,_t5>

#define _TUPLE_CTR(_t1,_t2,_v1,_v2) \
			_tuple_t_<_t1,_t2>(_v1,_v2)
#define _TRIPLE_CTR(_t1,_t2,_t3,_v1,_v2,_v3) \
			_triple_t_<_t1,_t2,_t3>(_v1,_v2,_v3)
#define _QUADRUPLE_CTR(_t1,_t2,_t3,_t4,_v1,_v2,_v3,_v4) \
	   		_quadruple_t_<_t1,_t2,_t3,_t4>(_v1,_v2,_v3,_v4)
#define _QUINTUPLE_CTR(_t1,_t2,_t3,_t4,_t5,_v1,_v2,_v3,_v4,_v5) \
			_quintuple_t_<_t1,_t2,_t3,_t4,_t5>(_v1,_v2,_v3,_v4,_v5)

template <class _R_, class _S_>
struct _tuple_t_ {
	_tuple_t_() {};
	_tuple_t_(_R_ a, _S_ b) : first(a), second(b) {}
	_R_ first;
	_S_ second;
};

template <class _R_, class _S_, class _T_>
struct _triple_t_ {
	_triple_t_() {};
	_triple_t_(_R_ a, _S_ b, _T_ c) : first(a), second(b), third(c) {}
	_R_ first;
	_S_ second;
	_T_ third;
};

template <class _R_, class _S_, class _T_, class _U_>
struct _quadruple_t_ {
	_quadruple_t_() {};
	_quadruple_t_(_R_ a, _S_ b, _T_ c, _U_ d) : 
			first(a), second(b), third(c), fourth(d) {}
	_R_ first;
	_S_ second;
	_T_ third;
	_U_ fourth;
};

template <class _R_, class _S_, class _T_, class _U_, class _V_>
struct _quintuple_t_ {
	_quintuple_t_() {};
	_quintuple_t_(_R_ a, _S_ b, _T_ c, _U_ d, _V_ e) : 
			first(a), second(b), third(c), fourth(d), fifth(e) {}
	_R_ first;
	_S_ second;
	_T_ third;
	_U_ fourth;
	_V_ fifth;
};
#endif
