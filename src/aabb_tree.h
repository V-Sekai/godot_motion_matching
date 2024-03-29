/**************************************************************************/
/*  aabb_tree.h                                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef AABB_TREE_H
#define AABB_TREE_H

#include "../thirdparty/AABB.h"

#include "core/io/resource.h"
#include "core/object/class_db.h"

class AABBTree : public Resource {
	GDCLASS(AABBTree, Resource)
	aabb::Tree bvh{};
public:
	void setup_tree(int dim, float fattening, int64_t nb_particles, bool touching_is_overlap);
	void insert_particle_at_position(int64_t index, PackedFloat32Array position, float radius);
	void insert_particle(int64_t index, PackedFloat32Array lowerbound, PackedFloat32Array upperbound);
	void remove_particle(int64_t index);
	void remove_all();
	void update_particle_at_position(int64_t index, PackedFloat32Array position, float radius, bool always_reinsert);
	void update_particle(int64_t index, PackedFloat32Array lowerbound, PackedFloat32Array upperbound, bool always_reinsert);
	PackedInt64Array query_index(unsigned int i);
	PackedInt64Array query_bounds(PackedFloat32Array lowerbound, PackedFloat32Array upperbound);

protected:
	static void _bind_methods() {
		using namespace godot;
		ClassDB::bind_method(D_METHOD("setup_tree", "dimensionality", "fattening", "nb_particles", "touching_is_overlap"), &AABBTree::setup_tree);
		ClassDB::bind_method(D_METHOD("insert_particle_at_position", "index", "position", "radius"), &AABBTree::insert_particle_at_position);
		ClassDB::bind_method(D_METHOD("insert_particle", "index", "lowerbound", "uppderbound"), &AABBTree::insert_particle);
		ClassDB::bind_method(D_METHOD("remove_particle", "index"), &AABBTree::remove_particle);
		ClassDB::bind_method(D_METHOD("remove_all"), &AABBTree::remove_all);

		ClassDB::bind_method(D_METHOD("update_particle_at_position", "index", "position", "radius", "always_reinsert"), &AABBTree::update_particle_at_position);
		ClassDB::bind_method(D_METHOD("update_particle", "index", "lowerbound", "upperbound", "always_reinsert"), &AABBTree::update_particle);

		ClassDB::bind_method(D_METHOD("query_index", "index"), &AABBTree::query_index);
		ClassDB::bind_method(D_METHOD("query_bounds", "lowerbound", "upperbound"), &AABBTree::query_bounds);
	}
};

#endif // AABB_TREE_H