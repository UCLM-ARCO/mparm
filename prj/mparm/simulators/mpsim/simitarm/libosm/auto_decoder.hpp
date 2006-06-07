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

#undef _STUB_DEC
#undef _TABLE_DEF_BEGIN
#undef _TABLE_DEF_END
#undef _STUB_ENTRY
#undef _FUNC_CALL
#undef _FUNC_NAME
#undef _STUB_NAME
#undef _TABLE_JUMP
#undef _PATTERN_TRUE

#ifndef _OPTIMIZE_OSM

#define _STUB_DEC(_a) static _OPER_ID_T *_stub_##_a(_INST_T);

#define _TABLE_DEF_BEGIN(_a,_b) \
	static _OPER_ID_T *(*_table_## _a [_b])(_INST_T) = {

#define _TABLE_DEF_END };

#define _STUB_ENTRY(_a) static _OPER_ID_T *_stub_##_a(_INST_T inst)

#define _FUNC_CALL(_a) return _a(inst)
#define _FUNC_NAME(_a) _a
#define _STUB_NAME(_a) _stub_##_a

#define _TABLE_JUMP(_a,_b,_c) return _table_##_a[(inst>>_c)&_b](inst)
#define _PATTERN_TRUE(_a,_b) ((inst&_a)==_b)

#ifndef _FUNC_DEFAULT
#define _FUNC_DEFAULT _func_default
static _OPER_ID_T *_func_default(_INST_T inst) {
	enum {
		_DECODE_EXCEPTION = 0,
	};
	assert(_DECODE_EXCEPTION);
}
#endif

#else

#define _STUB_DEC(_a) static unsigned _stub_##_a(_INST_T);

#define _TABLE_DEF_BEGIN(_a,_b) \
	static unsigned (*_table_## _a [_b])(_INST_T) = {

#define _TABLE_DEF_END };

#define _STUB_ENTRY(_a) static unsigned _stub_##_a(_INST_T inst)

#define _FUNC_CALL(_a) return _a(inst)
#define _FUNC_NAME(_a) _a
#define _STUB_NAME(_a) _stub_##_a

#define _TABLE_JUMP(_a,_b,_c) return _table_##_a[(inst>>_c)&_b](inst)
#define _PATTERN_TRUE(_a,_b) ((inst&_a)==_b)

#ifndef _FUNC_DEFAULT
#define _FUNC_DEFAULT _func_default
static unsigned _func_default(_INST_T inst) {
	enum {
		_DECODE_EXCEPTION = 0,
	};
	assert(_DECODE_EXCEPTION);
}
#endif

#endif

