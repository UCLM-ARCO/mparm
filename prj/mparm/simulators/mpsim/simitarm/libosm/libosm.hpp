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

#ifndef __LIB_OSM_H__
#define __LIB_OSM_H__

/* user of this file must provide macro
 * _MACH_ID_T and _OPER_ID_T
 */
#ifndef _MACH_ID_T
#error "_MACH_ID_T undefined!"
#else
#ifndef _OPER_ID_T
#error "_OPER_ID_T undefined!"
#else
#ifndef _SIMULATOR
#error "_SIMULATOR undefined!"
#endif
#endif
#endif


/* blocks rel_op operators for g++2 */
#define __SGI_STL_INTERNAL_RELOPS

#include "lib_types.hpp"

#ifndef _OPTIMIZE_OSM

/* Dummy cast to adjust the type of the two branches so that
 * they are always identical (for bit type problem: int_t and bit_t). */
#define _TOF(_t,_a,_b,_c) _a?(_t)_b:(_t)_c

#define _FUN_REF(_fname, _args...) _fname(_args)
#define _E_FUN_REF(_fname, _args...) _fname(_args)
#define _DIRTY_FUN_REF(_fname, _args...) _fname(_args)
#define _DIRTY_E_FUN_REF(_fname, _args...) \
			_fname(_machine_->__get_simulator(),_args)

#define _DIRTY_E_FUN_DEC(_fname, _args...) \
			extern void _fname(_SIMULATOR *, _args)
#define _E_FUN_DEC(_fname, _args...) \
			extern void _fname(_args)

#define _DIRTY_FUN_DEC(_fname, _args...) \
			static void _fname(_SIMULATOR *_sim_, _args)
#define _FUN_DEC(_fname, _args...) \
			static void _fname(_args)

#if 0 /* no longer used */
#define _VAR_REF(_a) _a
#define _ARG_REF(_a) _a
#define _DEF_REF(_a) _a
#define _MAN_REF(_a) _a
#endif
#define _MVR(_a) _machine_->_a
#define _BUF(_a) _machine_->_a

#ifndef _BASE_OPERATION
#define _BASE_OPERATION _base_operation_
#endif

#ifndef _BASE_MACHINE
#define _BASE_MACHINE _base_machine_
#endif

#define _OPER_CLASS(_a) OC_ ## _a
#define _MACH_CLASS(_a) MC_ ## _a
#define _MANG_CLASS(_a) _a

#define _REGISTER_OPER_ON_EDGE(_a,_b,_c) \
		_machine_->__register_oper_on_edge(this, _a::_b, _c)
#define _OPER_INIT(_a,_b,_c) __init(_a,_b,_c)

#define _TEST_RELEASE(_a) _BUF(_a)->test_release(_machine_)

#define _RELEASE(_a) _BUF(_a)->release(_machine_), _BUF(_a)=NULL

#define _TEST_ALLOCATE(_a) \
		_machine_->__get_simulator()->_a\
			->token_free_for_alloc(_machine_)

#define _TEST_ALLOCATE_ID(_a,_b) \
		_machine_->__get_simulator()->_a\
			->token_free_for_alloc(_b,_machine_)

#define _ALLOCATE(_a) \
		_machine_->__get_simulator()->_a\
			->allocate_token(_machine_)

#define _ALLOCATE_ID(_a,_b) \
		_machine_->__get_simulator()->_a\
			->allocate_token(_b,_machine_)

#define _ALLOCATE_TEMP(_a) \
		_machine_->__get_simulator()->_a\
			->allocate_token_temp(_machine_)

#define _ALLOCATE_TEMP_ID(_a,_b) \
		_machine_->__get_simulator()->_a\
			->allocate_token_temp(_b,_machine_)


#define _DISCARD(_a) _BUF(_a)->discard(_machine_), _BUF(_a)=NULL

#define _TEST_INQUIRE(_a) \
		_machine_->__get_simulator()->_a\
			->token_free_for_inquire(_machine_)

#define _TEST_INQUIRE_ID(_a,_b) \
		_machine_->__get_simulator()->_a\
			->token_free_for_inquire(_b,_machine_)

#define _INQUIRE(_a) \
		_machine_->__get_simulator()->_a\
			->inquire_token(_machine_)

#define _INQUIRE_ID(_a,_b) \
		_machine_->__get_simulator()->_a\
			->inquire_token(_b,_machine_)

#define _READ_TOKEN(_a,_b) \
		_a = _machine_->__get_simulator()->_b\
			->read_token(_machine_)

#define _READ_TOKEN_ID(_a,_b,_c) \
		_a = _machine_->__get_simulator()->_b\
			->read_token(_c,_machine_)

#define _READ(_a, _b) _a = _BUF(_b)->get_boss()->read_token(_machine_,_BUF(_b))

#define _WRITE_TOKEN(_a,_b) \
		_machine_->__get_simulator()->_a\
			->write_token(_b,_machine_)

#define _WRITE_TOKEN_ID(_a,_b,_c) \
		_machine_->__get_simulator()->_a\
			->write_token(_b,_c,_machine_)

#define _WRITE(_a,_b) _BUF(_a)->get_boss()->write_token(_b,_machine_,_BUF(_a))

#define _FIRE_EDGE(_id) _registries_[_id] && \
		 ((_singles_[_id] && _singles_[_id]->__run_all(_id)) || \
			(!_singles_[_id] && __fire(_id)))

#ifdef _GEN_DISASSEMBLER
#define _STRING_EVAL(_a) _a 
#else
#define _STRING_EVAL(_a)
#endif

#define _GET_OPERATION(_a) _machine_->__get_simulator()->allocate_operation(_a);

#define _DECODE(_a,_b) _a(_VAL(_b))

#define _ACT_OPERATION(_a, _b, _c) \
		_machine_->__get_simulator()->activate_operation(_a, _b, _c);

#define _COMPARE(_a,_b,_c) _VAL(((_a) _b (_c)))

#define _RETIRE_BUFFER(_a) /*assert(_a==NULL)*/

#define _FLUSH_BUFFER(_a)  if (_a!=NULL) {\
		_a->discard(this); \
		_a = NULL; \
	}

#include "managers.hpp"
#include "osm.hpp"

#else // OPTIMIZE_OSM

/* Dummy cast to adjust the type of the two branches so that
 * they are always identical (for bit type problem: int_t and bit_t). */
#define _TOF(_t,_a,_b,_c) _a?(_t)_b:(_t)_c

#define _FUN_REF(_fname, _args...) _fname(_args)
#define _E_FUN_REF(_fname, _args...) _fname(_args)
#define _DIRTY_FUN_REF(_fname, _args...) _fname(_args)
#define _DIRTY_E_FUN_REF(_fname, _args...) \
			_fname(_sim_,_args)

#define _DIRTY_E_FUN_DEC(_fname, _args...) \
			extern void _fname(_SIMULATOR *, _args)
#define _E_FUN_DEC(_fname, _args...) \
			extern void _fname(_args)

#define _DIRTY_FUN_DEC(_fname, _args...) \
			static void _fname(_SIMULATOR *_sim_, _args)
#define _FUN_DEC(_fname, _args...) \
			static void _fname(_args)

#define _MVR(_a) _a
#define _BUF(_a) _a

#ifndef _BASE_OPERATION
#define _BASE_OPERATION _base_operation_
#endif

#ifndef _BASE_MACHINE
#define _BASE_MACHINE _opt_machine_
#endif

#define _OPER_CLASS(_a) OC_ ## _a
#define _MACH_CLASS(_a) MC_ ## _a
#define _MANG_CLASS(_a) _a

#define _TEST_RELEASE(_a) _BUF(_a)->test_release(this)

#define _RELEASE(_a) _BUF(_a)->release(this), _BUF(_a)=NULL

#define _TEST_ALLOCATE(_a) \
		_sim_->_a->token_free_for_alloc(this)

#define _TEST_ALLOCATE_ID(_a,_b) \
		_sim_->_a->token_free_for_alloc(_b,this)

#define _ALLOCATE(_a) \
		_sim_->_a->allocate_token(this)

#define _ALLOCATE_ID(_a,_b) \
		_sim_->_a->allocate_token(_b,this)

#define _ALLOCATE_TEMP(_a) \
		_sim_->_a->allocate_token_temp(this)

#define _ALLOCATE_TEMP_ID(_a,_b) \
		_sim_->_a->allocate_token_temp(_b,this)


#define _DISCARD(_a) _BUF(_a)->discard(this), _BUF(_a)=NULL

#define _TEST_INQUIRE(_a) \
		_sim_->_a->token_free_for_inquire(this)

#define _TEST_INQUIRE_ID(_a,_b) \
		_sim_->_a->token_free_for_inquire(_b,this)

#define _INQUIRE(_a) \
		_sim_->_a->inquire_token(this)

#define _INQUIRE_ID(_a,_b) \
		_sim_->_a->inquire_token(_b,this)

#define _READ_TOKEN(_a,_b) \
		_a = _sim_->_b->read_token(this)

#define _READ_TOKEN_ID(_a,_b,_c) \
		_a = _sim_->_b->read_token(_c,this)

#define _READ(_a, _b) _a = _BUF(_b)->get_boss()->read_token(this,_BUF(_b))

#define _WRITE_TOKEN(_a,_b) \
		_sim_->_a->write_token(_b,this)

#define _WRITE_TOKEN_ID(_a,_b,_c) \
		_sim_->_a->write_token(_b,_c,this)

#define _WRITE(_a,_b) _BUF(_a)->get_boss()->write_token(_b,this,_BUF(_a))

#define _FIRE_EDGE(_id) _tmach->_id()

#ifdef _GEN_DISASSEMBLER
#define _STRING_EVAL(_a) _a 
#else
#define _STRING_EVAL(_a)
#endif

#define _GET_OPERATION(_a) _sim_->allocate_operation(_a);

#define _DECODE(_a,_b) _a(_VAL(_b))

#define _ACT_MACHINE(_a) \
		_sim_->activate_machine(_a)

#define _COMPARE(_a,_b,_c) _VAL(((_a) _b (_c)))

#define _RETIRE_BUFFER(_a) /*assert(_a==NULL)*/

#define _FLUSH_BUFFER(_a)  if (_a!=NULL) {\
		_a->discard(this); \
		_a = NULL; \
	}

#include "managers.hpp"
#include "osm_opt.hpp"

#endif // OPTIMIZE_OSM

#endif
