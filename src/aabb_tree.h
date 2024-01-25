#ifndef AABB_TREE_HPP
#define AABB_TREE_HPP

#include "../thirdparty/AABB.h"

#include "core/io/resource.h"
#include "core/object/class_db.h"

struct AABBTree : public Resource {
	GDCLASS(AABBTree, Resource)
	aabb::Tree bvh{};

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
#endif