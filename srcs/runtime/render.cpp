/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:53:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 16:14:21 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"

Vec3f	shadow_ray(const Scene &scene, HitRecord &rec, Vec3f target_pos)
{
	Ray		shadow_ray;
	Vec3f	light_dir;
	float	light_dist;

	light_dir = target_pos + rec.point;
	light_dist = std::sqrt(Vec3f::dot(light_dir, light_dir));
	shadow_ray._dir = Vec3f::normalize(light_dir);
	shadow_ray._o = rec.point + (rec.normal * FLT_EPSILON);
	return (scene.hit_shadow(shadow_ray, 0.001f, light_dist, rec)); 
	
}

Vec3f	traceRay(Ray ray, const Scene &scene, int max_depth, unsigned int *seed, const SunLight sun)
{
	Vec3f	accumulated_light(0.0f);
	Vec3f	throughput(1.0f);
	(void)sun;
	for (int depth = 0; depth < max_depth; depth++)
	{
		HitRecord	rec;
		HitRecord	shadow_rec;

		if (scene.hit(ray, 0.001f, FLT_MAX, rec))
		{
			Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);
			Ray		new_ray;
			Vec3f	albedo;
	
			accumulated_light += throughput * emitted;
			if (!rec.material->scatter(ray, rec, albedo, new_ray, seed))
				break ;
			
			throughput *= albedo;
			
			ray = new_ray;
		}
		else if (sun.enabled)
		{
			//sky
			Vec3f	unit_dir = Vec3f::normalize(ray._dir);
			float	y = unit_dir._y;
			Vec3f	sky_color;
			if (y >= 0.0f)
			{
				float	t = y;
				float	t_sq = t * t;
				float	h = std::exp(-t * 8.0f);
				Vec3f	horizon_color(0.52f, 0.83f, 1.0f);
				Vec3f	low_color(0.40f, 0.65f, 0.98f);
				Vec3f	zenith_color(0.05f, 0.25f, 0.85f);
	
				sky_color = horizon_color * h + low_color * (1.0f - h)
							* (1.0f - t_sq) + zenith_color * t_sq;
			}
			else
			{
				float	t = -y;
				Vec3f	horizon_color(0.52f, 0.83f, 1.0f);
				Vec3f	ground_color(0.05f, 0.25f, 0.85f);

				sky_color = horizon_color * (1.0f - t) + ground_color * t;
			}

			float	sky_intensity = 0.5f; 
			sky_color = sky_color * sky_intensity;
			//sun
			float	alignment = std::fmax(0.0f, std::fmin(Vec3f::dot(unit_dir, sun.direction), 1.0f));
			float	disk = std::pow(alignment, sun.size) * sun.intensity;
			float	glow = std::pow(alignment, sun.glow_size) * sun.glow_intensity;
			Vec3f	sun_final_color = (sun.color * disk) + (sun.glow_color * glow);

			sky_color += sun_final_color;
			accumulated_light += throughput * sky_color;
			break ;
		}
		else
		{
			Vec3f sky_color(0.0f);
			accumulated_light += throughput * sky_color;
			break;
		}
	}
	return (accumulated_light);
}


void	render(Render &render)
{
	for (size_t y = render.start_y; y < render.end_y; y++)
	{
		for (size_t x = render.start_x; x < render.end_x; x++)
		{
			float	u, v;
			if (render.samples != 1)
			{
				u = ((float)x + Vec3f::randomFloat(render.seed)) * render.inv_w;
				v = ((float)y + Vec3f::randomFloat(render.seed)) * render.inv_h;
			}
			else
			{
				u = ((float)x + 0.5f) * render.inv_w;
				v = ((float)y + 0.5f) * render.inv_h;
			}

			Ray		ray = render.cam.getRay(u, v);
			Vec3f	color = traceRay(ray, render.scene, render.depth_max, render.seed, render.sun_light);

			size_t	pixel_idx = y * render.width + x;
			if (render.frame_count == 1)
				render.accum_buffer[pixel_idx] = color;
			else
				render.accum_buffer[pixel_idx] += color;

			//moyenne
			Vec3f	final_color = render.accum_buffer[pixel_idx] / (float)render.frame_count;

			final_color._x = final_color._x / (final_color._x + 1.0f);
			final_color._y = final_color._y / (final_color._y + 1.0f);
			final_color._z = final_color._z / (final_color._z + 1.0f);
			final_color._x = pow(final_color._x, 0.45454545454);
			final_color._y = pow(final_color._y, 0.45454545454);
			final_color._z = pow(final_color._z, 0.45454545454);
			int	ir, ig, ib;
			ir = (int)(255.999 * final_color._x);
			ig = (int)(255.999 * final_color._y);
			ib = (int)(255.999 * final_color._z);
			render.definitive[pixel_idx] = (ir << 24) | (ig << 16) | (ib << 8) | 0xFF;
		}
	}
}
