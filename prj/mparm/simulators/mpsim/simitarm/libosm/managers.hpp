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

#ifndef __MANAGERS_H__
#define __MANAGERS_H__

#include <string>
#ifdef _OPTIMIZE_OSM
#include "osm_opt.hpp"
#else
#include "osm.hpp"
#endif

/* some basic manager examples */

/** manager controlling one resource.
 * T -- token value type
 */
template<class T>
class unit_manager {

	public:

		/** type of the token */
		typedef class _token_<unit_manager<T> > TOKEN_T;

		/** constructor. */
		unit_manager(const std::string& name) :
			name(name), token(new TOKEN_T(this)), temp_flag(false), value() {
		}

		/** destructor. */
		~unit_manager() {
			delete token;
		}

		/** See if token is available for allocation.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if token is available for inquire.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return token->is_free() || obj==token->get_tenant();
		}

		/** See if token is available for release.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return true;
		}

		/** Allocate the token.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token(_BASE_MACHINE *obj) {
			token->set_used(obj);
			return token;
		}

		/** Inquire about the token.
		 *  @param obj The OSM sending the request.
		 */
		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** Allocate the token temporarily.
		 * This is equivalent to an allocation followed by
		 * a successful release one cycle later.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token_temp(_BASE_MACHINE *obj) {
			temp_flag = true;
			token->set_used(obj);
			return token;
		}

		/** Release the token.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Discard the token.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Access the value of the token.
		 * This corresponds to val = *m[] in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj) {
			return value;
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj) {
			allocate_token_temp(obj);
			value = val;
		}

		/** Access the value of the token.
		 * This corresponds to *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj, TOKEN_T *token) {
			return value;
		}

		/** Update the value of the token.
		 * This function is called from the token. This corresponds
		 * to *token = val in OSM expression.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 * @param token The token to be updated.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj, TOKEN_T *token) {
			value = val;
		}

		/** Release the one allocated by alloc_temp */
		void update_on_clock() {
			if (temp_flag) {token->set_unused(); temp_flag = false;}
		}


		/** Get the value of the token, called by other managers or the
		 *  simulator.
		 */
		T get_value() {return value;}

		/** Update the value of the token, called by other managers or the
		 *  simulator.
		 */
		void set_value(const T& val) {value=val;}

		/** reset the manager. */
		void reset() {
			temp_flag =  false;
			token->set_unused();
		}

	private:

		std::string name;
		TOKEN_T *token;
		bool temp_flag;
		T value;
};


/* manager controlling an array of exclusive resources.
 * I -- index type
 * T -- token value type
 */
template<class I, class T>
class array_manager {

	public:

		/** The token type. */
		typedef class _token_<array_manager<I, T> > TOKEN_T;

		/** constructor.
		 * @param name Name of the manager.
		 * @param n    number of tokens.
		 */
		array_manager(const std::string& name, int n) :
			nTotal(n), name(name), or_temps(false) {

			temp_flags = new bool[n];
			tokens = new TOKEN_T[n](this);
			values = new T[n];

			// g++ does not perform initialization for temp_flags
			reset(); 
		}

		/** destructor. */
		~array_manager() {
			delete [] temp_flags;
			delete [] tokens;
			delete [] values;
		}

		/** See if token is available for allocation.
		 * We assume that the index is within the range.
		 * @param ind The index of the token.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_alloc(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()].is_free();
		}

		/** See if token is available for inquire.
		 * We assume that the index is within the range.
		 * @param ind The index of the token.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_inquire(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()].is_free() ||
				   obj==tokens[ind.val()].get_tenant();
		}

		/** See if token is ok to release.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return true;
		}

		/** Allocate a token.
		 * @param ind The index tof the token.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token(const I& ind, _BASE_MACHINE *obj) {
			tokens[ind.val()].set_used(obj);
			return tokens+ind.val();
		}

		/** Allocate a token temporarily.
		 * This is equivalent to an allocation followed by
		 * a successful release one cycle later.
		 * @param ind The index tof the token.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token_temp(const I& ind, _BASE_MACHINE *obj) {
			temp_flags[ind.val()] = or_temps = true;
			tokens[ind.val()].set_used(obj);
			return tokens+ind.val();
		}

		/** Inquire about the token.
		 *  @param ind The index to the token.
		 *  @param obj The OSM sending the request.
		 */
		void inquire_token(const I& ind, _BASE_MACHINE *obj) {
		}

		/** Release the token.
		 * @param token The token to be released.
		 * @param obj   The OSM sending the request.
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Discard the token.
		 * @param token The token to be discarded.
		 * @param obj   The OSM sending the request.
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Access the value of the token.
		 * This corresponds to val = *m[ind] in OSM expression.
		 * @param ind The index of the token.
		 * @param obj The OSM reading the value.
		 */
		T read_token(const I& ind, _BASE_MACHINE *obj) {
			return values[ind.val()];
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[ind] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param ind The index of the token.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const I& ind, const T& val, _BASE_MACHINE *obj) {
			allocate_token_temp(ind, obj);	// prevent double write
			values[ind.val()] = val;
		}

		/** Access the value of the token.
		 * This corresponds to val = *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj, TOKEN_T *token) {
			return values[token-tokens];
		}

		/** Update the value of the token.
		 * This function is called from the token. This corresponds
		 * to *token = val in OSM expression.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 * @param token The token to be updated.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj, TOKEN_T *token) {
			allocate_token_temp(token-tokens, obj);	// prevent double write
			values[token-tokens] = val;
		}

		/** Release the token(s) allocated by alloc_temp */
		void update_on_clock() {

			if (or_temps) {
				for (int i=0; i<nTotal; i++) {
					if (temp_flags[i])  {
						tokens[i].set_unused();
					   	temp_flags[i] = false;
					}
				}
				or_temps = false;
			}
		}


		/** Get the value of the token, called by other managers or the
		 *  simulator.
		 */
		T get_value(const I& ind) const {
			return values[ind.val()];
		}

		/** Update the value of the token, called by other managers or the
		 *  simulator.
		 */
		void set_value(const I& ind, const T& val) {
			values[ind.val()] = val;
		}

		/** reset the manager. */
		void reset() {
			or_temps = false;
			for (int i=0; i<nTotal; i++) {
				tokens[i].set_unused();
				temp_flags[i] = false;
			}
		}

	protected:

		const int nTotal;
		std::string name;

		bool or_temps;	// or-ed result of temp flags
		bool *temp_flags;
		TOKEN_T *tokens;
		T *values;
};


/* manager controlling one non-exclusive resource.
 * Suitable be used as a data port.
 * T -- token value type
 */
template<class T>
class unit_value_manager {

	public:

		/** type of the token */
		typedef class _token_<unit_value_manager<T> > TOKEN_T;

		/** constructor.
		 * @param name Name of the manager.
		 */
		unit_value_manager(const std::string& name) :
			name(name), token(new TOKEN_T(this)), value() {
			}

		/**  destructor. */
		~unit_value_manager() {
			delete token;
		}

		/** non-exclusive resource -> always available for allocation. */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> always available for inquire. */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> always ok to release. */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> no need to set tenant field. */
		TOKEN_T *allocate_token(_BASE_MACHINE *obj) {
			return token;
		}

		/** non-exclusive resource -> no need to set tenant field. */
		TOKEN_T *allocate_token_temp(_BASE_MACHINE *obj) {
			return token;
		}

		/** Inquire about the token.
		 *  @param obj The OSM sending the request.
		 */
		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** release the token, do nothing. */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
		}

		/** discard the token, do nothing. */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
		}


		/** get the name of the token. */
		const std::string& get_name() {
			return name;
		}

		/** Access the value of the token.
		 * This corresponds to val = *m[] in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj) {
			return value;
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj) {
			value = val;
		}

		/** Access the value of the token.
		 * This corresponds to val = *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj, TOKEN_T *token) {
			return value;
		}

		/** Update the value of the token.
		 * This function is called from the token. This corresponds
		 * to *token = val in OSM expression.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 * @param token The token to be updated.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj, TOKEN_T *token) {
			value = val;
		}

		/** Get the value of the token, called by other managers or the
		 *  simulator.
		 */
		T get_value() const {return value;}

		/** Update the value of the token, called by other managers or the
		 *  simulator.
		 */
		void set_value(const T& val) {value=val;}

		/** Do nothing. */
		void update_on_clock() {}

		/** reset the manager. */
		void reset() {
			token->set_unused();
		}

	private:

		std::string name;
		TOKEN_T *token;
		T value;
};


/* manager controlling several non-exclusive resource.
 * Suitable be used as a data port.
 * I -- token index type
 * T -- token value type
 */
template<class I, class T>
class array_value_manager {

	public:

		/** type of the token */
		typedef class _token_<array_value_manager<I, T> > TOKEN_T;

		/** constructor.
		 * @param name Name of the manager.
		 * @param n    Number of tokens.
		 */
		array_value_manager(const std::string& name, int n) : 
				nTotal(n), name(name) {
			tokens = new TOKEN_T[n](this);
			values = new T[n];
		}

		/**  destructor. */
		~array_value_manager() {
			delete [] tokens;
			delete [] values;
		}

		/** non-exclusive resource -> always available for allocation. */
		bool token_free_for_alloc(const I& ind, _BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> always available for inquire. */
		bool token_free_for_inquire(const I& ind, _BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> always ok to release. */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return true;
		}

		/** non-exclusive resource -> no need to set tenant field. */
		TOKEN_T *allocate_token(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()];
		}

		/** non-exclusive resource -> no need to set tenant field. */
		TOKEN_T *allocate_token_temp(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()];
		}

		/** Inquire about the token.
		 *  @param ind The index to the token.
		 *  @param obj The OSM sending the request.
		 */
		void inquire_token(const I& ind, _BASE_MACHINE *obj) {
		}

		/** release the token, do nothing. */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
		}

		/** discard the token, do nothing. */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
		}


		/** get the name of the token. */
		const std::string& get_name() {
			return name;
		}

		/** Access the value of the token.
		 * This corresponds to val = *m[ind] in OSM expression.
		 * @param ind The index to token.
		 * @param obj The OSM reading the value.
		 */
		T read_token(const I& ind, _BASE_MACHINE *obj) {
			return values[ind.val()];
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[ind] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param ind The index to token.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const I& ind, const T& val, _BASE_MACHINE *obj) {
			values[ind.val()] = val;
		}

		/** Access the value of the token.
		 * This corresponds to val = *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj, TOKEN_T *token) {
			return values[token-tokens];
		}

		/** Update the value of the token.
		 * This function is called from the token. This corresponds
		 * to *token = val in OSM expression.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 * @param token The token to be updated.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj, TOKEN_T *token) {
			values[token-tokens] = val;
		}

		/** Get the value of the token, called by other managers or the
		 *  simulator.
		 */
		T get_value(const I& ind) const {return values[ind.val()];}

		/** Update the value of the token, called by other managers or the
		 *  simulator.
		 */
		void set_value(const I& ind, const T& val) {values[ind.val()]=val;}

		/** Do nothing. */
		void update_on_clock() {}

		/** reset the manager. */
		void reset() {
			for (int i=0; i<nTotal; i++)
				tokens[i].set_unused();
		}

	private:

		const int nTotal;
		std::string name;
		TOKEN_T *tokens;
		T *values;
};

/** A class to model pipeline latency, has no value, no read/write methods. */
template <class I>
class latency_manager {

	public:

		/** type of the token */
		typedef class _token_<latency_manager<I> > TOKEN_T;

		/** constructor. */
		latency_manager(const std::string& name) :
			name(name), token(new TOKEN_T(this)),
	   		temp_flag(false), latency(0) {}

		/** destructor. */
		~latency_manager() {
			delete token;
		}

		/** See if token is available for allocation.
		 * @param ind The index to the token.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_alloc(const I& ind, _BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if token is available for inquire.
		 * @param ind The index to the token.
		 * @param obj The OSM sending the request.
		 */
		bool token_free_for_inquire(const I& ind, _BASE_MACHINE *obj) {
			return token->is_free() || obj==token->get_tenant();
		}

		/** See if token is available for release.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return latency==0;
		}

		/** Allocate the token.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token(const I& ind, _BASE_MACHINE *obj) {
			latency = ind.val();
			token->set_used(obj);
			return token;
		}

		/** Allocate the token temporarily.
		 * This is equivalent to an allocation followed by
		 * a successful release one cycle later.
		 * @param obj The OSM sending the request.
		 */
		TOKEN_T *allocate_token_temp(const I& ind, _BASE_MACHINE *obj) {
			temp_flag = true;
			token->set_used(obj);
			return token;
		}

		/** Inquire about a token.
		 *  @param ind The index to the token.
		 *  @param obj The OSM sending the request.
		 */
		void inquire_token(const I& ind, _BASE_MACHINE *obj) {
		}

		/** Release the token.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Discard the token.
		 * @param token The token to release.
		 * @param obj   The OSM sending the request.
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Release the one allocated by alloc_temp */
		void update_on_clock() {
			if (latency) latency--;
			if (temp_flag) {token->set_unused(); temp_flag = false;}
		}


		/** reset the manager. */
		void reset() {
			temp_flag =  false;
			token->set_unused();
			latency = 0;
		}

	private:

		std::string name;
		TOKEN_T *token;
		bool temp_flag;
		unsigned int latency;
};

#endif
