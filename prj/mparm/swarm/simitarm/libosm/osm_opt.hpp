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

#ifndef __OSM_OPT_HPP__
#define __OSM_OPT_HPP__

#include "bittypes.h"
#include <string>
#include <cstring>

/* naming convention (to avoid naming clash with user defined variables)
 * function name:  prefix with double underscore,
 * variable name:  prefix and suffix with underscore,
 * class name:     prefix and suffix with underscore,
 * macro name:     prefix and suffix with underscore, all capital.
 */

/** Base class for the OSM machine.
 *  Derived class could define a enum of state,
 *  a static table of state names, as long as the activate and reset.
 *  Also buffers, variables, etc.
 */
class _opt_machine_ {

	public:

		/** Constructor.
		 *  @param sim    The simulator object.
		 *  @param id     ID number of the machine.
		 */  
		_opt_machine_(_SIMULATOR *sim, _MACH_ID_T id) :
			_sim_(sim), _id_(id) {}

		/** Destructor. */
		virtual ~_opt_machine_() {}

		/** Get the class id of the machine. */
		_MACH_ID_T __get_id() const {return _id_;}

		virtual void __init() = 0;

		/** Activate the state machine.
		 *  An OSM should remain if either the activation fails to
		 *  advance state or it advances to an non-initial state.
		 *  @return True if the OSM should remain in list (not retiring).
		 */
		bool __activate() {
			return (*_acts_[_state_])(this);
		}

		/** Reset the state machine.
		 *  Discarding all buffers, reset state, edges?
		 *  The derived class should call this at the end of its retire().
		 */
		virtual void __retire() {}

		/** Get the current state. */
		unsigned int __get_state() const {return _state_;}

		/** Set the current state. */
		void __set_state(unsigned int s) {_state_ = s;}

		/** Get the dynamic id of the operation */
		/** Get the dynamic id of the operation */
		uint64_t __get_dynid() const {return _dynid_;}

		/** Get the dynamic id of the operation */
		void __set_dynid(uint64_t id) {_dynid_ = id;}

		/** The simulator object. */
		_SIMULATOR *__get_simulator() const {return _sim_;}

	public:

		_SIMULATOR *_sim_;

		unsigned int _state_;	// current state

		uint64_t _dynid_;

		_MACH_ID_T _id_; 	// class id of the machine

		typedef bool (*__ACT_FTYPE)(_opt_machine_ *);

		const __ACT_FTYPE *_acts_;

};

/** Token member names will not clash with identifiers, so
 *  the naming convention does not apply here.
 */

/** Token class. 
 *  A token represent a resource.
 *  _M_ -- manager type
 */
template <class _M_>
class _token_ {

	public:

		/** Constructor. */
		_token_(_M_ *m) : _tenant_(NULL), _boss_(m) {}

		/** Destructor. */
		~_token_() {}

		/** Test if a token is free.
		 *	@return True if the token is free.
		 */
		bool is_free() {return _tenant_==NULL;}

		/** Get the object holding the token. */
		_opt_machine_ *get_tenant() {return _tenant_;}

		/** Release the token */
		bool test_release(_opt_machine_ *obj) {
			return _boss_->token_good_to_reclaim(this, obj);
		}

		/** Release the token */
		void release(_opt_machine_ *obj) {
			_boss_->reclaim_token(this, obj);
		}

		/** Discard the token */
		void discard(_opt_machine_ *obj) {
			return _boss_->discard_token(this, obj);
		}

		/** Get the manager */
		_M_ *get_boss() {return _boss_;}

		/** Set the token as used by an object. Used by Manager only.
		 *	@param obj The object to hold the token.
		 */
		void set_used(_opt_machine_ *obj) {
			_tenant_=obj;
		}

		/** Set an object as unused. Used by manager only. */
		void set_unused() {
			_tenant_=NULL;
		}

	private:

		_opt_machine_ *_tenant_;
		_M_ *_boss_;
};

#endif
