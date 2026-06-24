/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 10:51:20 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/24 18:53:48 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Render.hpp"

Vec3f	traceRay(Ray ray, const Render& render)
{
	Vec3f	accumulated_light(0.0f);
	Vec3f	throughput(1.0f);
	float	prev_pdf_scatter = 1.0f;
	bool	prev_was_specular = false;
	int		node_tests = 0;
	int*	counter = (render.bvh_debug.mode == BvhDebugMode::HEATMAP) ? &node_tests : nullptr;

	// debug slice
	if (render.bvh_debug.mode == BvhDebugMode::DEPTH_SLICE)
		return (evaluate_depth_slice(ray, render));

	for (int depth = 0; depth < render.depth_max; ++depth)
	{
		// ray marching black hole
		if (render.black_hole_enabled)
		{
			if (evaluate_blackholes(ray, render, accumulated_light, throughput))
				break ;
		}

		// hit geom
		HitRecord	rec = {};
		if (!render.scene.hit(ray, 1e-3f, FLT_MAX, rec, counter))
		{
			accumulated_light += throughput * calculate_environment(ray, render);
			break ;
		}
		applyNormalMap(rec);
		// light
		Vec3f	emitted = rec.material->emitted(rec.u, rec.v, rec.point);
		if (emitted.length_sq() > 0.0f)
		{
			accumulated_light += throughput * apply_mis_weight(emitted, ray, render, prev_pdf_scatter, prev_was_specular, depth);
			break ;
		}

		// rebond indirect
		float	pdf_mat_scatter = 1.0f;
		Ray		new_ray;
		Vec3f	albedo;
		if (!rec.material->scatter(ray, rec, albedo, new_ray, pdf_mat_scatter, render.seed))
			break ;
	
		// shadow rays /next event simulation
		if (render.shadow_ray && rec.material->isOpaq() && !rec.material->isSpecular())
			accumulated_light += throughput * calculate_direct_lighting(ray, rec, render, albedo, counter);

		throughput *= albedo;

		//roulette russe
		if (render.ru_enabled)
		{
			if (depth > 3 && !apply_russian_roulette(throughput, render.seed))
				break ;
		}

		//next bounce
		prev_was_specular = rec.material->isSpecular();
		prev_pdf_scatter = pdf_mat_scatter;
		ray = new_ray;
	}

	return (render.bvh_debug.mode == BvhDebugMode::HEATMAP) ? (color_heatmap(node_tests, render.bvh_debug.max_tests)) : (accumulated_light);
}

void	render(Render &render_job)
{
	for (size_t y = render_job.start_y; y < render_job.end_y; ++y)
	{
		for (size_t x = render_job.start_x; x < render_job.end_x; ++x)
		{
			float	u, v;

			if (render_job.samples != 1)
			{
				u = ((float)x + Vec3f::randomFloat(render_job.seed)) * render_job.inv_w;
				v = ((float)y + Vec3f::randomFloat(render_job.seed)) * render_job.inv_h;
			}
			else
			{
				u = ((float)x + 0.5f) * render_job.inv_w;
				v = ((float)y + 0.5f) * render_job.inv_h;
			}
			Ray		ray = render_job.cam.getRay(u, v, render_job.seed);
			Vec3f	color = traceRay(ray, render_job);
			size_t	idx = y * render_job.width + x;

			if (render_job.bvh_debug.mode != BvhDebugMode::OFF)
				render_job.accum_buffer[idx] = color;
			else
			{
				if (render_job.frame_count == 1)
					render_job.accum_buffer[idx] = color;
				else
					render_job.accum_buffer[idx] += color;
			}
		}
	}
}
