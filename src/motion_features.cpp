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
#include "core/object/ref_counted.h"
#include "scene/3d/skeleton_3d.h"
#include <cstdint>

#ifdef TOOLS_ENABLED
#include "editor/plugins/node_3d_editor_gizmos.h"
#endif

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

void BonePositionVelocityMotionFeature::set_to_skeleton(NodePath path) {
	if (is_local_to_scene() && get_local_scene() != nullptr) {
		auto mp = get_local_scene()->get_node_or_null(NodePath("MotionPlayer"));
		skeleton = cast_to<Skeleton3D>(mp->get_node(path));
		to_skeleton = get_local_scene()->get_path_to(skeleton);
	}
}

NodePath BonePositionVelocityMotionFeature::get_to_skeleton() { return to_skeleton; }

void BonePositionVelocityMotionFeature::set_bone_names(PackedStringArray value) {
	bone_names = value;
}

PackedStringArray BonePositionVelocityMotionFeature::get_bone_names() { return bone_names; }

int BonePositionVelocityMotionFeature::get_dimension() {
	return bone_names.size() * 3 * 2;
}

void BonePositionVelocityMotionFeature::setup_nodes(Variant character) {
	the_char = Object::cast_to<CharacterBody3D>(character);
	skeleton = cast_to<Skeleton3D>(the_char->get_node(NodePath("Armature/GeneralSkeleton")));

	bones_id.clear();
	if (skeleton != nullptr) {
		for (int64_t i = 0; i < bone_names.size(); ++i) {
			const size_t id = skeleton->find_bone(bone_names[i]);
			if (id >= 0) {
				bones_id.push_back(id);
			}
		}
		print_line(vformat("Bones id %s %s", bone_names, bones_id));
	}

	last_known_positions.resize(bones_id.size());
	last_known_positions.fill({});
	last_known_velocities.resize(bones_id.size());
	last_known_velocities.fill({});
	last_known_result.resize(bones_id.size() * 2 * 3);
	last_known_velocities.fill({});
}

void BonePositionVelocityMotionFeature::setup_for_animation(Ref<Animation> animation) {
	skeleton->reset_bone_poses();
	bone_tracks.clear();
	bones_id.clear();
	for (int64_t i = 0; i < bone_names.size(); ++i) {
		const BoneId id = skeleton->find_bone(bone_names[i]);
		if (id >= 0) {
			bones_id.push_back(id);
		}
	}
	for (int64_t bone_id = 0; bone_id < skeleton->get_bone_count(); ++bone_id) {
		const String bone_name = "%GeneralSkeleton:" + skeleton->get_bone_name(bone_id);
		PackedInt32Array tracks{};
		tracks.push_back(animation->find_track(NodePath(bone_name), Animation::TrackType::TYPE_POSITION_3D));
		tracks.push_back(animation->find_track(NodePath(bone_name), Animation::TrackType::TYPE_ROTATION_3D));
		//tracks.push_back(animation->find_track(NodePath(bone_name),Animation::TrackType::TYPE_SCALE_3D));
		bone_tracks[bone_id] = tracks;
	}
}

void BonePositionVelocityMotionFeature::set_skeleton_to_animation_timestamp(Ref<Animation> anim, float time) {
	// UtilityFunctions::print((skeleton == nullptr)?"Skeleton error, path not found":"Skeleton set");
	if (anim == nullptr || skeleton == nullptr) {
		return;
	}
	for (int64_t bone_id = 0; bone_id < skeleton->get_bone_count(); ++bone_id) {
		if (!bone_tracks.has(bone_id)) {
			continue;
		}
		const int64_t pos = bone_tracks[bone_id][0];
		const int64_t quat = bone_tracks[bone_id][1];
		// const auto scale = bone_tracks[bone_id][2];
		if (pos >= 0) {
			const Vector3 position = anim->position_track_interpolate(pos, time);
			skeleton->set_bone_pose_position(bone_id, position);
		}

		if (quat >= 0) {
			const Quaternion rotation = anim->rotation_track_interpolate(quat, time);
			skeleton->set_bone_pose_rotation(bone_id, rotation);
		}

		// const Vector3 scaling = anim->scale_track_interpolate(scale,time);

		// skeleton->set_bone_pose_scale(bone_id,scaling);
	}
}

PackedFloat32Array BonePositionVelocityMotionFeature::bake_animation_pose(Ref<Animation> animation, float time) {
	PackedVector3Array prev_pos{}, curr_pos{};
	PackedFloat32Array result{};
	set_skeleton_to_animation_timestamp(animation, time - 0.1);
	for (int64_t index = 0; index < bones_id.size(); ++index) {
		const auto bone_id = bones_id[index];
		prev_pos.push_back(skeleton->get_bone_global_pose(bone_id).get_origin() * skeleton->get_motion_scale());
	}
	set_skeleton_to_animation_timestamp(animation, time);
	for (int64_t index = 0; index < bones_id.size(); ++index) {
		const auto bone_id = bones_id[index];
		curr_pos.push_back(skeleton->get_bone_global_pose(bone_id).get_origin() * skeleton->get_motion_scale());
	}
	const int64_t root_id = 0;
	const Transform3D root = skeleton->get_bone_global_pose(root_id) * skeleton->get_motion_scale();

	for (int64_t index = 0; index < bones_id.size(); ++index) {
		const auto pos = root.basis.xform_inv(curr_pos[index] - root.get_origin());
		result.push_back(pos.x);
		result.push_back(pos.y);
		result.push_back(pos.z);
		const auto vel = root.basis.xform_inv(curr_pos[index] - prev_pos[index]) / 0.1;
		result.push_back(vel.x);
		result.push_back(vel.y);
		result.push_back(vel.z);
	}
	return result;
}

PackedFloat32Array BonePositionVelocityMotionFeature::broadphase_query_pose(Dictionary blackboard, float p_delta) {
	PackedVector3Array current_positions{}, current_velocities{};
	last_known_result.resize(bones_id.size() * 2 * 3);
	current_positions.resize(bones_id.size());
	current_velocities.resize(bones_id.size());

	float curr_time = Time::get_singleton()->get_ticks_msec();

	for (int64_t index = 0; index < bones_id.size(); ++index) {
		Vector3 pos = skeleton->get_bone_global_pose(bones_id[index]).origin;
		Vector3 vel = (pos - last_known_positions[index]) / delta;
		current_positions.write[index] = pos;
		current_velocities.write[index] = vel;
	}

	last_time_queried = curr_time;
	last_known_positions = current_positions.duplicate();
	last_known_velocities = current_velocities.duplicate();

	const size_t size = 3;
	for (int64_t i = 0; i < bones_id.size(); ++i) {
		Vector3 pos = current_positions[i], vel = current_velocities[i];

		last_known_result.write[i * size * 2] = pos.x;
		last_known_result.write[i * size * 2 + 1] = pos.y;
		last_known_result.write[i * size * 2 + 2] = pos.z;
		last_known_result.write[i * size * 2 + size] = vel.x;
		last_known_result.write[i * size * 2 + size + 1] = vel.y;
		last_known_result.write[i * size * 2 + size + 2] = vel.z;
	}
	return last_known_result;
}

float BonePositionVelocityMotionFeature::narrowphase_evaluate_cost(PackedFloat32Array to_convert) {
	if (to_convert.size() != last_known_result.size()) {
		return std::numeric_limits<float>::max();
	}
	float cost = 0.0;
	for (auto i = 0; i < to_convert.size(); ++i) {
		cost += abs(to_convert[i] * to_convert[i] - last_known_result[i] * last_known_result[i]);
	}

	return cost;
}

PackedFloat32Array BonePositionVelocityMotionFeature::get_weights() {
	PackedFloat32Array result{};
	for (auto i = 0; i < 3 * bone_names.size(); ++i) {
		result.append(weight_bone_pos);
	}
	for (auto i = 0; i < 3 * bone_names.size(); ++i) {
		result.append(weight_bone_vel);
	}
	return result;
}

void BonePositionVelocityMotionFeature::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_to_skeleton", "value"), &BonePositionVelocityMotionFeature::set_to_skeleton);
	ClassDB::bind_method(D_METHOD("get_to_skeleton"), &BonePositionVelocityMotionFeature::get_to_skeleton);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "skeleton", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Skeleton3D"), "set_to_skeleton", "get_to_skeleton");

	ClassDB::bind_method(D_METHOD("set_bone_names", "value"), &BonePositionVelocityMotionFeature::set_bone_names);
	ClassDB::bind_method(D_METHOD("get_bone_names"), &BonePositionVelocityMotionFeature::get_bone_names);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "bones"), "set_bone_names", "get_bone_names");
}

void BonePositionVelocityMotionFeature::debug_pose_gizmo(Ref<RefCounted> p_gizmo, const PackedFloat32Array p_data, Transform3D p_tr) {
	// if (data.size() == get_dimension())
	{
#ifdef TOOLS_ENABLED
		Ref<EditorNode3DGizmo> gizmo = p_gizmo;
		constexpr int s = 3;
		for (int64_t index = 0; index < bone_names.size(); ++index) {
			//i*size*2+size+2
			Vector3 pos = Vector3(p_data[index * s * 2 + 0], p_data[index * s * 2 + 1], p_data[index * s * 2 + 2]);
			Vector3 vel = Vector3(p_data[index * s * 2 + s + 0], p_data[index * s * 2 + s + 1], p_data[index * s * 2 + s + 2]);
			pos = p_tr.xform(pos);
			vel = p_tr.xform(vel);
			auto white = gizmo->get_plugin()->get_material("white", gizmo);
			auto blue = gizmo->get_plugin()->get_material("blue", gizmo);
			gizmo->add_lines(PackedVector3Array{ pos, pos + vel }, blue);
			auto box = Ref<BoxMesh>();
			box.instantiate();
			box->set_size(Vector3(0.05f, 0.05f, 0.05f));
			Transform3D tr = Transform3D(Basis(), pos);
			gizmo->add_mesh(box, white, tr);
		}
#endif
	}
}

PackedFloat32Array PredictionMotionFeature::get_weights() {
	PackedFloat32Array result{};
	for (auto i = 0; i < 2 * past_time_dt.size(); ++i) {
		result.append(weight_history_pos);
	}
	for (auto i = 0; i < 2 * future_time_dt.size(); ++i) {
		result.append(weight_prediction_pos);
	}
	for (auto i = 0; i < 1 * future_time_dt.size(); ++i) {
		result.append(weight_prediction_angle);
	}
	return result;
}

PredictionMotionFeature::PredictionMotionFeature() {
	set_local_to_scene(true);
}

void PredictionMotionFeature::create_default_dt() {
	past_time_dt.clear();
	future_time_dt.clear();
	float time = past_delta;
	for (int count = 0; count < past_count; ++count, time += past_delta) {
		past_time_dt.push_back(-time);
	}
	time = future_delta;
	for (int count = 0; count < future_count; ++count, time += future_delta) {
		future_time_dt.push_back(time);
	}
}

int PredictionMotionFeature::get_dimension() {
	// Offset for each
	const size_t past_pos = 2 * past_time_dt.size();
	const size_t future_pos = 2 * future_time_dt.size();
	const size_t future_rot_angle = future_time_dt.size();
	return past_pos + future_pos + future_rot_angle;
}

void PredictionMotionFeature::setup_nodes(Variant character) {
	auto n = Object::cast_to<CharacterBody3D>(character);
	skeleton = cast_to<Skeleton3D>(n->get_node(NodePath("Armature/GeneralSkeleton")));
}

void PredictionMotionFeature::setup_for_animation(Ref<Animation> animation) {
	start_time = 0.1f;
	end_time = std::floor(animation->get_length() * 10) / 10.0f;
	root_tracks[0] = animation->find_track(root_bone_name, Animation::TrackType::TYPE_POSITION_3D);
	root_tracks[1] = animation->find_track(root_bone_name, Animation::TrackType::TYPE_ROTATION_3D);
	root_tracks[2] = animation->find_track(root_bone_name, Animation::TrackType::TYPE_SCALE_3D);
	{
		start_pos = animation->position_track_interpolate(root_tracks[0], 0.0);
		start_rot = animation->rotation_track_interpolate(root_tracks[1], 0.0);
		start_vel = (animation->position_track_interpolate(root_tracks[0], 0.1) - start_pos) / 0.1;
	}
	{
		end_pos = animation->position_track_interpolate(root_tracks[0], end_time);
		end_rot = animation->rotation_track_interpolate(root_tracks[1], end_time);
		end_vel = (end_pos - animation->position_track_interpolate(root_tracks[0], end_time - 0.1)) / 0.1;

		end_ang_vel = animation->rotation_track_interpolate(root_tracks[1], animation->get_length() - delta - 0.1).inverse() * animation->rotation_track_interpolate(root_tracks[1], animation->get_length() - delta);
	}
}

PackedFloat32Array PredictionMotionFeature::bake_animation_pose(Ref<Animation> animation, float time) {
	PackedFloat32Array result{};
	Vector3 curr_pos = animation->position_track_interpolate(root_tracks[0], time);
	Quaternion curr_rot = animation->rotation_track_interpolate(root_tracks[1], time);

	for (int64_t index = 0; index < past_time_dt.size(); ++index) {
		const float t = time - abs(past_time_dt[index]);
		Vector3 pos{};
		Quaternion rot{};
		if (t >= 0.0f) { // The offset can be accessed through the anim data
			pos = animation->position_track_interpolate(root_tracks[0], t) - curr_pos;
			rot = animation->rotation_track_interpolate(root_tracks[1], t);
			pos = curr_rot.xform_inv(pos);
		} else { // The offset must be calculated using the starting velocity and extrapoling
			pos = start_pos + (start_vel * t) - curr_pos;
			pos = curr_rot.xform_inv(pos);
		}
		result.push_back(pos.x);
		result.push_back(pos.z);
	}
	for (int64_t index = 0; index < future_time_dt.size(); ++index) {
		const float t = time + abs(future_time_dt[index]);
		Vector3 pos{};
		Quaternion rot{};
		if (t <= end_time) { // The offset can be accessed through the anim data
			pos = animation->position_track_interpolate(root_tracks[0], t) - curr_pos;
			rot = animation->rotation_track_interpolate(root_tracks[1], t);
			pos = curr_rot.xform_inv(pos);
		} else { // The offset must be calculated using the end velocity and extrapoling
			pos = end_pos + end_vel * (t - end_time) - curr_pos;
			pos = curr_rot.xform_inv(pos);
		}

		result.push_back(pos.x);
		result.push_back(pos.z);
	}
	for (int64_t index = 0; index < future_time_dt.size(); ++index) {
		const float t = time + abs(future_time_dt[index]);
		Vector3 pos{};
		Quaternion rot{};
		if (t <= end_time) { // The offset can be accessed through the anim data
			rot = animation->rotation_track_interpolate(root_tracks[1], t) * curr_rot.inverse();
		} else { // The offset must be calculated using the end velocity and extrapoling
			rot = animation->rotation_track_interpolate(root_tracks[1], animation->get_length() - delta) * curr_rot.inverse();
		}

		result.push_back(rot.get_euler(EulerOrder::XYZ).y);
	}
	return result;
}

PackedFloat32Array PredictionMotionFeature::broadphase_query_pose(Dictionary p_blackboard, float p_delta) {
	PackedFloat32Array result{};
	Array blackboard_array;
	blackboard_array.resize(3);
	blackboard_array[0] = "history";
	blackboard_array[1] = "prediction";
	blackboard_array[2] = "pred_dir";
	if (!p_blackboard.has_all(blackboard_array)) {
		return result;
	}

	PackedVector3Array history = PackedVector3Array(p_blackboard["history"]);
	PackedVector3Array prediction = PackedVector3Array(p_blackboard["prediction"]);
	PackedFloat32Array direction = PackedFloat32Array(p_blackboard["pred_dir"]);
	{
		for (Vector3 elem : history) {
			result.append(elem.x);
			result.append(elem.z);
		}
		for (Vector3 elem : prediction) {
			result.append(elem.x);
			result.append(elem.z);
		}
		for (float elem : direction) {
			result.append(elem);
		}
	}
	return result;
}

void PredictionMotionFeature::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_weight_history_pos", "position"), &PredictionMotionFeature::set_weight_history_pos);
	ClassDB::bind_method(D_METHOD("get_weight_history_pos"), &PredictionMotionFeature::get_weight_history_pos);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight_history_pos"), "set_weight_history_pos", "get_weight_history_pos");
	ClassDB::bind_method(D_METHOD("set_weight_prediction_pos", "position"), &PredictionMotionFeature::set_weight_prediction_pos);
	ClassDB::bind_method(D_METHOD("get_weight_prediction_pos"), &PredictionMotionFeature::get_weight_prediction_pos);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight_prediction_pos"), "set_weight_prediction_pos", "get_weight_prediction_pos");
	ClassDB::bind_method(D_METHOD("set_weight_prediction_angle", "angle"), &PredictionMotionFeature::set_weight_prediction_angle);
	ClassDB::bind_method(D_METHOD("get_weight_prediction_angle"), &PredictionMotionFeature::get_weight_prediction_angle);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight_prediction_angle"), "set_weight_prediction_angle", "get_weight_prediction_angle");

	ClassDB::bind_method(D_METHOD("set_root_bone_name", "root_bone_name"), &PredictionMotionFeature::set_root_bone_name, DEFVAL("%GeneralSkeleton:Root"));
	ClassDB::bind_method(D_METHOD("get_root_bone_name"), &PredictionMotionFeature::get_root_bone_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "root_bone_name"), "set_root_bone_name", "get_root_bone_name");
	ClassDB::bind_method(D_METHOD("set_past_time_dt", "past_time_delta"), &PredictionMotionFeature::set_past_time_dt);
	ClassDB::bind_method(D_METHOD("get_past_time_dt"), &PredictionMotionFeature::get_past_time_dt);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "past_time_dt"), "set_past_time_dt", "get_past_time_dt");
	ClassDB::bind_method(D_METHOD("set_future_time_dt", "future_time_delta"), &PredictionMotionFeature::set_future_time_dt);
	ClassDB::bind_method(D_METHOD("get_future_time_dt"), &PredictionMotionFeature::get_future_time_dt);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "future_time_dt"), "set_future_time_dt", "get_future_time_dt");
}

void PredictionMotionFeature::debug_pose_gizmo(Ref<RefCounted> p_gizmo, const PackedFloat32Array p_data, Transform3D p_transform) {
#ifdef TOOLS_ENABLED
	{
		Ref<EditorNode3DGizmo> gizmo = p_gizmo;
		if (gizmo.is_null()) {
			return;
		}
		auto white = gizmo->get_plugin()->get_material("white", gizmo);
		auto green = gizmo->get_plugin()->get_material("green", gizmo);
		auto orange = gizmo->get_plugin()->get_material("orange", gizmo);
		for (int64_t i = 0; i < past_time_dt.size(); ++i) {
			const int64_t offset = i * 2;
			Vector3 pos = Vector3(p_data[offset + 0], 0, p_data[offset + 1]);
			pos = p_transform.xform(pos);
			gizmo->add_lines(PackedVector3Array{ pos, pos + Vector3(0, 1, 0) }, green);
		}
		const int64_t pos_offset = past_time_dt.size();
		const int64_t traj_offset = past_time_dt.size() * 2 + future_time_dt.size() * 2;
		for (int64_t i = 0; i < future_time_dt.size(); ++i) {
			const int64_t offset = (pos_offset + i) * 2;
			Vector3 pos = Vector3(p_data[offset + 0], 0, p_data[offset + 1]);
			Vector3 traj = p_transform.xform(Vector3(0, 0, 1)).rotated(Vector3(0, 1, 0), p_data[traj_offset + i]);
			pos = p_transform.xform(pos);
			// traj = tr.xform(traj);
			gizmo->add_lines(PackedVector3Array{ pos, pos + traj }, orange);
		}
	}
#endif
}

void RootVelocityMotionFeature::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_weight", "value"), &RootVelocityMotionFeature::set_weight, DEFVAL(1.0f));
	ClassDB::bind_method(D_METHOD("get_weight"), &RootVelocityMotionFeature::get_weight);
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::FLOAT, "weight"), "set_weight", "get_weight");

	// ClassDB::bind_method( D_METHOD("set_body","value"), &RootVelocityMotionFeature::set_body);
	// ClassDB::bind_method( D_METHOD("get_body"), &RootVelocityMotionFeature::get_body);
	// ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,"Character",godot::PROPERTY_HINT_NODE_PATH_VALID_TYPES,"CharacterBody3D"),"set_body","get_body");

	ClassDB::bind_method(D_METHOD("set_root_bone_name", "value"), &RootVelocityMotionFeature::set_root_bone_name, DEFVAL("%GeneralSkeleton:Root"));
	ClassDB::bind_method(D_METHOD("get_root_bone_name"), &RootVelocityMotionFeature::get_root_bone_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "root_bone"), "set_root_bone_name", "get_root_bone_name");
	// ClassDB::bind_method( D_METHOD("setup_for_animation","animation"), &RootVelocityMotionFeature::setup_for_animation);
	// ClassDB::bind_method( D_METHOD("bake_animation_pose","animation","time"), &RootVelocityMotionFeature::bake_animation_pose);
	// ClassDB::bind_method( D_METHOD("narrowphase_evaluate_cost","data_to_evaluate"), &RootVelocityMotionFeature::narrowphase_evaluate_cost);
}
