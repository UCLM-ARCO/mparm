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

#ifndef __OSM_HPP__
#define __OSM_HPP__

#include "bittypes.h"
#include <string>
#include <cstring>

/* naming convention (to avoid naming clash with user defined variables)
 * function name:  prefix with double underscore,
 * variable name:  prefix and suffix with underscore,
 * class name:     prefix and suffix with underscore,
 * macro name:     prefix and suffix with underscore, all capital.
 */

/* forward declaration */
class _base_machine_;

/** Base operation class. */
class _base_operation_ {

	public:

		/** Constructor. */
		_base_operation_(_OPER_ID_T _id, _MACH_ID_T _mid) :
			_id_(_id), _mid_(_mid) {};

		/** Destructor, do nothing. */
		virtual ~_base_operation_() {}

		/** Initialize the operation.
		 *  Decode subopers, the EVAL section, register edges.
		 *  @param mach The machine associated with the operation.
		 *  @param dtab The decoding table.
		 *  @param cdng The coding.
		 */
		virtual void __init(_base_machine_ *mach,
			_OPER_ID_T *dtab, void *cdng) = 0;

		/** Get the class id of the operation. */
		_OPER_ID_T __get_id() const {return _id_;}

		/** Get the class id of the machine. */
		_MACH_ID_T __get_mach_id() const {return _mid_;}

		/** Run the OSM expressions on an edge.
		 *  @param id  The edge's index.
		 *  @return True if the OSM expressions are successful.
		 */
		virtual bool __test_osm(unsigned int id) = 0;

		/** Run the OSM expressions on an edge.
		 *  @param id  The edge's index.
		 *  @return True if the OSM expressions are successful.
		 */
		virtual void __run_osm_pre(unsigned int id) = 0;

		/** Run the OSM expressions on an edge.
		 *  @param id  The edge's index.
		 *  @return True if the OSM expressions are successful.
		 */
		virtual void __run_osm_post(unsigned int id) = 0;

		/** Run the expressions associated on an edge.
		 *  @param id The edge's index.
		 */
		virtual void __run_exp(unsigned int id) = 0;

		/** Run everything associated on the edge.
		 *  This is the combination of above four.
		 *  @param id The edge's index.
		 */
		virtual bool __run_all(unsigned int id) = 0;

		/** Get the assembly syntax of the operation. */
		virtual std::string __get_syntax() {return "";}

		/** Use void * as an efficient polymorphic type. */
		virtual const void *__get_coding() {return NULL;}

	protected:

		_OPER_ID_T _id_;		//class id of the operation
		_MACH_ID_T _mid_;		//class id of the required machine

};

/** Base class for the OSM machine.
 *  Derived class could define a enum of state,
 *  a static table of state names, as long as the activate and reset.
 *  Also buffers, variables, etc.
 */
class _base_machine_ {

	public:

		/** Constructor.
		 *  @param sim    The simulator object.
		 *  @param id     ID number of the machine.
		 *  @param n_edge Number of edges.
		 */  
		_base_machine_(_SIMULATOR *sim, _MACH_ID_T id, unsigned int n_edge) :
			_sim_(sim), _ocount_(0), _n_edge_(n_edge), _id_(id) {

			_opers_ = new (_base_operation_ *)[16];
			_registries_ = new uint32_t[n_edge];
			_singles_ = new (_base_operation_ *)[n_edge];

			/** g++ cannot initialize new array so far!!, do this instead */
			memset(_registries_, 0, _n_edge_*sizeof(uint32_t));
		}

		/** Destructor. */
		virtual ~_base_machine_() {

			delete [] _opers_;
			delete [] _registries_;
			delete [] _singles_;
		}

		/** Get the class id of the machine. */
		_MACH_ID_T __get_id() const {return _id_;}

		/** Activate the state machine.
		 *  An OSM should remain if either the activation fails to
		 *  advance state or it advances to an non-initial state.
		 *  @return True if the OSM should remain in list (not retiring).
		 */
		virtual bool __activate() = 0;

		/** Reset the state machine.
		 *  Discarding all buffers, reset state, edges?
		 *  The derived class should call this at the end of its retire().
		 */
		virtual void __retire() {
			_ocount_ = 0;
			memset(_registries_, 0, _n_edge_*sizeof(uint32_t));
		}

		/** Reset the state machine.
		 *  Discarding all buffers, reset state, edges?
		 *  The derived class should call this at the end of its flush().
		 */
		virtual void __flush() {
			_ocount_ = 0;
			memset(_registries_, 0, _n_edge_*sizeof(uint32_t));
		}


		/** Get the current state. */
		unsigned int __get_state() const {return _state_;}

		/** Get the dynamic id of the operation */
		uint64_t __get_dynid() const {return _dynid_;}

		/** Get the dynamic id of the operation */
		void __set_dynid(uint64_t id) {_dynid_ = id;}

		/** Get the operations associated with the machine. */
		unsigned int __get_opers(_base_operation_** &opers) {
			opers = _opers_;
			return _ocount_;
		}

		/** Register an operation on the machine. */
		void __register_oper(_base_operation_ *oper) {
			_opers_[_ocount_++] = oper;
		}

		/** Register an operation on an edge. */
		void __register_oper_on_edge(_base_operation_ *oper,
			unsigned int id, unsigned int regval) {

			if (!_registries_[id]) _singles_[id] = oper;
			else _singles_[id] = NULL;

			_registries_[id] |= regval << (2*(_ocount_-1));
		}

		/** Fire the transactions on the edge.
		 *  @return True if successful.
		 */
		bool __fire(unsigned int id) {

			int i;
			uint32_t reg;

			uint32_t registry = _registries_[id];

			/* since g++ may not inline __fire, we took these common and
			 * simple cases of __fire.
			 * Profiling shows that about 66% fails * the registry==0 test,
			 * and 11% of these (7% of total) fails the _singles_[id] test.
			 * So moving these out could save %93 function calling overhead
			 */
			/* empty edge, not to execute. */
			//if (registry==0) return false;

			//if (_singles_[id]) return _singles_[id]->__run_all(id);

			/* test the conditions. */
			for (reg=registry, i=0; reg!=0; reg=reg>>2, i++) {
				if (reg&2 && !(_opers_[i]->__test_osm)(id)) return false;
			}

			/* run the OSM expressions that gets resources. */
			for (reg=registry, i=0; reg!=0; reg=reg>>2, i++) {
				if (reg&2) (_opers_[i]->__run_osm_pre)(id);
			}

			/* run the expressions. */
			for (reg=registry, i=0; reg!=0; reg=reg>>2, i++) {
				if (reg&1) (_opers_[i]->__run_exp)(id);
			}

			/* run the OSM expressions that releases resources. */
			for (reg=registry, i=0; reg!=0; reg=reg>>2, i++) {
				if (reg&2) (_opers_[i]->__run_osm_post)(id);
			}

			return true;
		}

		/** The simulator object. */
		_SIMULATOR *__get_simulator() const {return _sim_;}

	protected:

		_SIMULATOR *_sim_;

		unsigned int _state_;	// current state

		_base_operation_ **_opers_;		// associated operations
		unsigned int _ocount_;			// counter of operations

		uint32_t *_registries_;			// table of operation registry
		_base_operation_ **_singles_;	// single oper on edge

		unsigned int _n_edge_;

		uint64_t _dynid_;

		_MACH_ID_T _id_; 	// class id of the machine
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
		_base_machine_ *get_tenant() {return _tenant_;}

		/** Release the token */
		bool test_release(_base_machine_ *obj) {
			return _boss_->token_good_to_reclaim(this, obj);
		}

		/** Release the token */
		void release(_base_machine_ *obj) {
			_boss_->reclaim_token(this, obj);
		}

		/** Discard the token */
		void discard(_base_machine_ *obj) {
			return _boss_->discard_token(this, obj);
		}

		/** Get the manager */
		_M_ *get_boss() {return _boss_;}

		/** Set the token as used by an object. Used by Manager only.
		 *	@param obj The object to hold the token.
		 */
		void set_used(_base_machine_ *obj) {
			_tenant_=obj;
		}

		/** Set an object as unused. Used by manager only. */
		void set_unused() {
			_tenant_=NULL;
		}

	private:

		_base_machine_ *_tenant_;
		_M_ *_boss_;
};

#endif
