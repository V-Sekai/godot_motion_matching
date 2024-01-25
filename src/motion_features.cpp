#include "motion_features.h"

void MotionFeature::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_dimension"), &MotionFeature::get_dimension);

	ClassDB::bind_method(D_METHOD("get_weights"), &MotionFeature::get_weights);

	// BIND_VIRTUAL_METHOD(MotionFeature,get_dimension);

	ClassDB::bind_method(D_METHOD("setup_nodes", "character"), &MotionFeature::setup_nodes);

	ClassDB::bind_method(D_METHOD("setup_for_animation", "animation"), &MotionFeature::setup_for_animation);
	ClassDB::bind_method(D_METHOD("bake_animation_pose", "animation", "time"), &MotionFeature::bake_animation_pose);

	ClassDB::bind_method(D_METHOD("broadphase_query_pose", "blackboard", "delta"), &MotionFeature::broadphase_query_pose);
	ClassDB::bind_method(D_METHOD("narrowphase_evaluate_cost", "data_to_evaluate"), &MotionFeature::narrowphase_evaluate_cost);

	ClassDB::bind_method(D_METHOD("debug_pose_gizmo", "gizmo", "data", "root_transform"), &MotionFeature::debug_pose_gizmo);
}

MotionFeature::MotionFeature() {
	set_local_to_scene(true);
}
