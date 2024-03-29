/**************************************************************************/
/*  crit_spring_damper.cpp                                                */
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

#include "crit_spring_damper.h"

#include "core/config/project_settings.h"
#include "core/math/math_defs.h"

float CritDampSpring::square(float x) {
	return x * x;
}

Vector3 CritDampSpring::damp_adjustment_exact(Vector3 g, float halflife, float dt, float eps) {
	float factor = 1.0 - fast_negexp((CritDampSpring::Ln2 * dt) / (halflife + eps));
	return g * factor;
}

Quaternion CritDampSpring::damp_adjustment_exact_quat(Quaternion g, float halflife, float dt, float eps) {
	float factor = 1.0 - fast_negexp((CritDampSpring::Ln2 * dt) / (halflife + eps));
	return Quaternion().slerp(g, factor);
}

Variant CritDampSpring::damper_exponential(Variant variable, Variant goal, float damping, float dt) {
	float ft = 1.0f / (float)ProjectSettings::get_singleton()->get("physics/common/physics_ticks_per_second");
	float factor = 1.0f - pow(1.0f / (1.0f - ft * damping), -dt / ft);
	return Math::lerp(variable, goal, factor);
}

float CritDampSpring::fast_negexp(float x) {
	return 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
}

Variant CritDampSpring::damper_exact(Variant variable, Variant goal, float halflife, float dt, float eps) {
	return Math::lerp(variable, goal, 1.0f - fast_negexp((CritDampSpring::Ln2 * dt) / (halflife + eps)));
}

float CritDampSpring::halflife_to_damping(float halflife, float eps) {
	return (4.0f * Ln2) / (halflife + eps);
}

float CritDampSpring::damping_to_halflife(float damping, float eps) {
	return (4.0f * Ln2) / (damping + eps);
}

float CritDampSpring::frequency_to_stiffness(float frequency) {
	return square(2.0f * Math_PI * frequency);
}

float CritDampSpring::stiffness_to_frequency(float stiffness) {
	return sqrt(stiffness) / (2.0f * Math_PI);
}

float CritDampSpring::critical_halflife(float frequency) {
	return damping_to_halflife(sqrt(frequency_to_stiffness(frequency) * 4.0f));
}

float CritDampSpring::critical_frequency(float halflife) {
	return stiffness_to_frequency(square(halflife_to_damping(halflife)) / 4.0f);
}

float CritDampSpring::damping_ratio_to_stiffness(float ratio, float damping) {
	return square(damping / (ratio * 2.0f));
}

float CritDampSpring::damping_ratio_to_damping(float ratio, float stiffness) {
	return ratio * 2.0f * sqrt(stiffness);
}

float CritDampSpring::maximum_spring_velocity_to_halflife(float x, float x_goal, float v_max) {
	return damping_to_halflife(2.0 * ((v_max / (x_goal - x)) * exp(1.0f)));
}

void CritDampSpring::_spring_damper_exact(
		float &x,
		float &v,
		float x_goal,
		float v_goal,
		float damping_ratio,
		float halflife,
		float dt,
		float eps) {
	float g = x_goal;
	float q = v_goal;
	float d = halflife_to_damping(halflife);
	float s = damping_ratio_to_stiffness(damping_ratio, d);
	float c = g + (d * q) / (s + eps);
	float y = d / 2.0;

	if (Math::abs(s - (d * d) / 4.0) < eps) { // Critically Damped
		float j0 = x - c;
		float j1 = v + j0 * y;
		float eydt = Math::exp(-y * dt);
		x = j0 * eydt + dt * j1 * eydt + c;
		v = -y * j0 * eydt - y * dt * j1 * eydt + j1 * eydt;
	} else if (s - (d * d) / 4.0 > 0.0) { // Under Damped
		float w = Math::sqrt(s - (d * d) / 4.0f);
		float j = Math::sqrt(Math::pow(v + y * (x - c), 2) / (Math::pow(w, 2) + eps) + Math::pow(x - c, 2));
		float p = Math::atan((v + (x - c) * y) / (-(x - c) * w + eps));

		// j = (x - c) > 0.0 ? j : -j;
		j = (x - c) > 0.0 ? j : -j;

		float eydt = Math::exp(-y * dt);

		x = j * eydt * Math::cos(w * dt + p) + c;
		v = -y * j * eydt * Math::cos(w * dt + p) - w * j * eydt * Math::sin(w * dt + p);
	} else if (s - (d * d) / 4.0 < 0.0) { // Over Damped
		float y0 = (d + Math::sqrt(Math::pow(d, 2) - 4.0f * s)) / 2.0f;
		float y1 = (d - Math::sqrt(Math::pow(d, 2) - 4.0f * s)) / 2.0f;
		float j1 = (c * y0 - x * y0 - v) / (y1 - y0);
		float j0 = x - j1 - c;

		float ey0dt = Math::exp(-y0 * dt);
		float ey1dt = Math::exp(-y1 * dt);

		x = j0 * ey0dt + j1 * ey1dt + c;
		v = -y0 * j0 * ey0dt - y1 * j1 * ey1dt;
	}
}

void CritDampSpring::_critical_spring_damper_exact(
		float &x,
		float &v,
		float x_goal,
		float v_goal,
		float halflife,
		float dt) {
	float g = x_goal;
	float q = v_goal;
	float d = halflife_to_damping(halflife);
	float c = g + (d * q) / ((d * d) / 4.0f);
	float y = d / 2.0f;
	float j0 = x - c;
	float j1 = v + j0 * y;
	float eydt = fast_negexp(y * dt);
	x = eydt * (j0 + j1 * dt) + c;
	v = eydt * (v - j1 * y * dt);
}

PackedFloat32Array CritDampSpring::critical_spring_damper_exact(float x, float v, float x_goal, float v_goal, float halflife, float dt) {
	_critical_spring_damper_exact(x, v, x_goal, v_goal, halflife, dt);
	PackedFloat32Array result;
	result.append(x);
	result.append(v);
	return result;
}

void CritDampSpring::_simple_spring_damper_exact(float &x, float &v, float x_goal, float halflife, float dt) {
	float y = halflife_to_damping(halflife) / 2.0f;
	float j0 = x - x_goal;
	float j1 = v + j0 * y;
	float eydt = fast_negexp(y * dt);
	x = eydt * (j0 + j1 * dt) + x_goal;
	v = eydt * (v - j1 * y * dt);
}

PackedFloat32Array CritDampSpring::simple_spring_damper_exact(float x, float v, float x_goal, float halflife, float dt) {
	_simple_spring_damper_exact(x, v, x_goal, halflife, dt);
	PackedFloat32Array result;
	result.append(x);
	result.append(v);
	return result;
}

void CritDampSpring::_decay_spring_damper_exact(float &x, float &v, float halflife, float dt) {
	float y = halflife_to_damping(halflife) / 2.0f;
	float j1 = v + x * y;
	float eydt = fast_negexp(y * dt);
	x = eydt * (x + j1 * dt);
	v = eydt * (v - j1 * y * dt);
}

PackedFloat32Array CritDampSpring::decay_spring_damper_exact(float x, float v, float halflife, float dt) {
	PackedFloat32Array result;
	_decay_spring_damper_exact(x, v, halflife, dt);
	result.append(x);
	result.append(v);
	return result;
}

void CritDampSpring::_timed_spring_damper_exact(
		float &x, float &v,
		const float xi,
		const float &x_goal, const float &t_goal,
		const float &halflife, const float &dt,
		float apprehension) {
	const float min_time = t_goal > dt ? t_goal : dt;

	const float v_goal = (x_goal - xi) / min_time;

	const float t_goal_future = dt + apprehension * halflife;
	const float x_goal_future = t_goal_future < t_goal ? xi + v_goal * t_goal_future : x_goal;

	_simple_spring_damper_exact(x, v, x_goal_future, halflife, dt);
	x += v_goal * dt;
}

PackedFloat32Array CritDampSpring::timed_spring_damper_exact(float x, float v,
		const float xi,
		const float x_goal, const float t_goal,
		const float halflife, const float dt,
		const float apprehension) {
	PackedFloat32Array result;
	_timed_spring_damper_exact(x, v, xi, x_goal, t_goal, halflife, dt, apprehension);
	result.append(x);
	result.append(v);
	return result;
}

Dictionary CritDampSpring::character_update(
		Vector3 pos,
		Vector3 vel,
		Vector3 acc,
		Quaternion quaternion,
		Vector3 angular_velocity,
		Vector3 v_goal,
		Quaternion q_goal,
		float halflife_vel,
		float halflife_rot,
		float dt) {
	Dictionary answer;
	{
		float y = halflife_to_damping(halflife_vel) / 2.0f;
		Vector3 j0 = vel - v_goal;
		Vector3 j1 = acc + j0 * y;
		float eydt = fast_negexp(y * dt);

		answer["world_position"] = eydt * ((-j1 / (y * y)) + ((-j0 - j1 * dt) / y)) +
				(j1 / (y * y)) + j0 / y + v_goal * dt + pos;
		answer["velocity"] = eydt * (j0 + j1 * dt) + v_goal;
		answer["acceleration"] = eydt * (acc - j1 * y * dt);
	}
	{
		float y = halflife_to_damping(halflife_rot) / 2.0f;
		Vector3 j0 = (quaternion * q_goal.inverse()).get_euler();
		Vector3 j1 = angular_velocity + j0 * y;
		float eydt = fast_negexp(y * dt);
		answer["quaternion"] = (Quaternion::from_euler(eydt * (j0 + j1 * dt)) * q_goal).normalized();
		answer["angular_velocity"] = eydt * (angular_velocity - j1 * y * dt);
		answer["delta"] = dt;
	}
	return answer;
}

Dictionary CritDampSpring::character_predict(
		Vector3 x, Vector3 v, Vector3 a,
		Quaternion q, Vector3 angular_v,
		Vector3 v_goal, Quaternion q_goal,
		float halflife_v, float halflife_q,
		const PackedFloat32Array dts) {
	Dictionary answer;
	for (int i = 0; i < dts.size(); i++) {
		float dt = dts[i];
		answer[i] = character_update(x, v, a, q, angular_v, v_goal, q_goal, halflife_v, halflife_q, dt);
	}
	return answer;
}

void CritDampSpring::_bind_methods() {
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damper_exponential", "variable", "goal", "damping", "dt"), &CritDampSpring::damper_exponential);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damp_adjustment_exact_quat", "g", "halflife", "dt", "eps"), &CritDampSpring::damp_adjustment_exact_quat, DEFVAL(CMP_EPSILON));
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damp_adjustment_exact", "g", "halflife", "dt", "eps"), &CritDampSpring::damp_adjustment_exact, DEFVAL(CMP_EPSILON));

	ClassDB::bind_static_method("CritDampSpring", D_METHOD("halflife_to_damping", "halflife", "eps"), &CritDampSpring::halflife_to_damping, DEFVAL(CMP_EPSILON));
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damping_to_halflife", "damping", "eps"), &CritDampSpring::damping_to_halflife, DEFVAL(CMP_EPSILON));
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("frequency_to_stiffness", "frequency"), &CritDampSpring::frequency_to_stiffness);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("stiffness_to_frequency", "stiffness"), &CritDampSpring::stiffness_to_frequency);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("critical_halflife", "frequency"), &CritDampSpring::critical_halflife);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("critical_frequency", "halflife"), &CritDampSpring::critical_frequency);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damping_ratio_to_stiffness", "ratio", "damping"), &CritDampSpring::damping_ratio_to_stiffness);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("damping_ratio_to_damping", "ratio", "stiffness"), &CritDampSpring::damping_ratio_to_damping);

	ClassDB::bind_static_method("CritDampSpring", D_METHOD("maximum_spring_velocity_to_halflife", "x", "x_goal", "v_max"), &CritDampSpring::maximum_spring_velocity_to_halflife);

	ClassDB::bind_static_method("CritDampSpring", D_METHOD("timed_spring_damper_exact", "x", "v", "xi", "x_goal", "t_goal", "halflife", "dt", "apprehension"), &CritDampSpring::timed_spring_damper_exact, DEFVAL(2));
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("decay_spring_damper_exact", "pos", "vel", "halflife", "dt"), &CritDampSpring::decay_spring_damper_exact);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("simple_spring_damper_exact", "x", "v", "x_goal", "halflife", "dt"), &CritDampSpring::simple_spring_damper_exact);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("critical_spring_damper_exact", "x", "v", "x_goal", "v_goal", "halflife", "dt"), &CritDampSpring::critical_spring_damper_exact);

	ClassDB::bind_static_method("CritDampSpring", D_METHOD("character_update", "pos", "vel", "acc", "quaternion", "angular_velocity", "v_goal", "q_goal", "halflife_vel", "halflife_rot", "dt"), &CritDampSpring::character_update);
	ClassDB::bind_static_method("CritDampSpring", D_METHOD("character_predict", "x", "v", "a", "q", "angular_v", "v_goal", "q_goal", "halflife_v", "halflife_q", "dts"), &CritDampSpring::character_predict);
}
