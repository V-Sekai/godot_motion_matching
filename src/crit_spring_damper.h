#ifndef CRIT_SPRING_DAMPER_H
#define CRIT_SPRING_DAMPER_H

#include "core/object/ref_counted.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"

struct CritDampSpring : public RefCounted {
	GDCLASS(CritDampSpring, RefCounted)

	static constexpr float Ln2 = 0.69314718056;
	static float square(float x);
	static Vector3 damp_adjustment_exact(Vector3 g, float halflife, float dt, float eps = 1e-8);
	static Quaternion damp_adjustment_exact_quat(Quaternion g, float halflife, float dt, float eps = 1e-8);
	static Variant damper_exponential(Variant variable, Variant goal, float damping, float dt);
	static float fast_negexp(float x);
	static Variant damper_exact(Variant variable, Variant goal, float halflife, float dt, float eps = 1e-5f);
	static float halflife_to_damping(float halflife, float eps = 1e-5f);
	static float damping_to_halflife(float damping, float eps = 1e-5f);
	static float frequency_to_stiffness(float frequency);
	static float stiffness_to_frequency(float stiffness);
	static float critical_halflife(float frequency);
	static float critical_frequency(float halflife);
	static float damping_ratio_to_stiffness(float ratio, float damping);
	static float damping_ratio_to_damping(float ratio, float stiffness);
	static float maximum_spring_velocity_to_halflife(float x, float x_goal, float v_max);
	static void _spring_damper_exact(
			float &x,
			float &v,
			float x_goal,
			float v_goal,
			float damping_ratio,
			float halflife,
			float dt,
			float eps = 1e-5f);
	static void _critical_spring_damper_exact(
			float &x,
			float &v,
			float x_goal,
			float v_goal,
			float halflife,
			float dt);
	static PackedFloat32Array critical_spring_damper_exact(float x, float v, float x_goal, float v_goal, float halflife, float dt);
	static void _simple_spring_damper_exact(float &x, float &v, float x_goal, float halflife, float dt);
	static PackedFloat32Array simple_spring_damper_exact(float x, float v, float x_goal, float halflife, float dt);
	static void _decay_spring_damper_exact(float &x, float &v, float halflife, float dt);
	static PackedFloat32Array decay_spring_damper_exact(float x, float v, float halflife, float dt);

	//	Reach the x_goal at timed t_goal in the future
	//	Apprehension parameter controls how far into the future we try to track the linear interpolation
	static void _timed_spring_damper_exact(
			float &x, float &v,
			const float xi,
			const float &x_goal, const float &t_goal,
			const float &halflife, const float &dt,
			float apprehension = 2.0f);
	static PackedFloat32Array timed_spring_damper_exact(float x, float v,
			const float xi,
			const float x_goal, const float t_goal,
			const float halflife, const float dt,
			const float apprehension = 2.0f);
	static Dictionary character_update(
			Vector3 pos,
			Vector3 vel,
			Vector3 acc,
			Quaternion quaternion,
			Vector3 angular_velocity,
			Vector3 v_goal,
			Quaternion q_goal,
			float halflife_vel,
			float halflife_rot,
			float dt);

	static Dictionary character_predict(
			Vector3 x, Vector3 v, Vector3 a,
			Quaternion q, Vector3 angular_v,
			Vector3 v_goal, Quaternion q_goal,
			float halflife_v, float halflife_q,
			const PackedFloat32Array dts);

protected:
	static void _bind_methods();
};

#endif