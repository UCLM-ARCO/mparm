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

#ifndef __OBJECT_POOL__
#define __OBJECT_POOL__

#include <vector>

#define OBJ_BATCH_SIZE 4

/* the library allocating classified  objects efficiently.
 * @param T  Object class.
 * @param I  Index type (used for classification).
 * @param S  Simulator type.
 * @param Allocator Function allocates the object. */
template<class T, class I, class S, T *Allocator(S*,I)>
class obj_pool {

	public:

		/** Forbid C++ from generating default copy and assign. */
		obj_pool(const obj_pool&);
		obj_pool& operator=(const obj_pool&);

		/** Constructor.
		 *  @param n The number of objects types.
		 */
		obj_pool(const I& n) : nTotal(n) {

			objs = new std::vector<T *>[n];
			for (I i=0; i<n; i++) {
				objs[i].reserve(OBJ_BATCH_SIZE);
			}
		}

		/** Destructor.
		 *  @warn Memory leaks if not all objects are reclaimed.
		 */
		~obj_pool() {

			/** delete all elements. */
			for (I i=0; i<nTotal; i++) {
				for (I j=0; j<objs[i].size(); j++) {
					delete objs[i][j];
				}
			}
			/** Free the array */
			delete [] objs;
		}

		/** Allocate an object.
		 *  @param sim The simulator object.
		 *  @param id  Class id of the object.
		 */
		T *allocate(S *sim, const I& id) {

			if (objs[id].size()==0)
				return Allocator(sim, id);

			T *ret = objs[id].back();
			objs[id].pop_back();
			return ret;
		}

		/** Release an object.
		 *  @param obj The object to release.
		 */
		void reclaim(T *obj) {
			objs[obj->__get_id()].push_back(obj);
		}

	private:

		I nTotal;				/* total number of object classes. */
		std::vector<T *> *objs;	/* array of vectors */
};

#endif
