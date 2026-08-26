
#ifndef CAMERA_GLSL
#define CAMERA_GLSL

Ray		camera_get_ray(float u_coord, float v_coord, inout uint seed)
{
	float	ndc_x = (2.0 * u_coord) - 1.0;
	float	ndc_y = 1.0 - (2.0 * v_coord);

	float	aspect = float(pc.w_w) / float(pc.w_h);
	float	half_h = tan((pc.fov * PI / 180.0) * 0.5);
	float	half_w = half_h * aspect;

	vec3	forward = pc.cam_forward;
	vec3	right = pc.cam_right;
	vec3	up = pc.cam_up;

	vec3	direction = forward + right * (ndc_x * half_w) + up * (ndc_y * half_h);

	vec3	focal_point = pc.cam_origin + direction * pc.m_focus_dist;
	vec3	origin = pc.cam_origin;
	if (pc.m_lens_radius > 0.0f)
	{
		vec3	rd = pc.m_lens_radius * random_in_unit_disk(seed);
		origin += right * rd.x + up * rd.y;
	}
	Ray		r;
	r.origin = origin;
	r.dir = normalize(focal_point - origin);
	return r;
}

#endif