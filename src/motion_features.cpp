/**************************************************************************/
/*  motion_features.cpp                                                   */
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
