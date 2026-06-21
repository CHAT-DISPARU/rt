/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlackHole.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 12:51:41 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/21 08:57:58 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"

class BlackHole
{
	public:
		Vec3f	center;
		float	event_horizon_radius; // rs = 2GM/c^2
		float	accretion_inner;// rayon interne 
		float	accretion_outer;// rayon externe
		float	disk_thickness;// epaisseur du disque axe y
		float	gravity_strength;// facteur courbure
		int		max_steps;
		float	step_size;
		float	escape_distance;// distance considere rayon sorti

		BlackHole(Vec3f c, float rs)
		{
			center = c;
			event_horizon_radius = rs;
			accretion_inner = rs * 2.2f;
			accretion_outer = rs * 8.0f;
			disk_thickness = rs * 0.05f;
			gravity_strength = rs * 1.5f;
			max_steps = 500;
			step_size = rs * 0.05f;
			escape_distance = rs * 60.0f;
		}

		//true si rayon consome ou false si rayon sechape 
		bool	march(const Ray& ray_in, Vec3f& out_color, Ray& out_ray) const
		{
			Vec3f	pos = ray_in._o;
			Vec3f	dir = Vec3f::normalize(ray_in._dir);
			float	r0 = (center - pos).length();

			if (r0 < event_horizon_radius)
			{
				out_color = Vec3f(0.0f);
				return (true);
			}

			for (int i = 0; i < max_steps; i++)
			{
				Vec3f	to_center = center - pos;
				float	r = to_center.length();
				if (r < event_horizon_radius)
				{
					out_color = Vec3f(0.0f);
					return (true);
				}

				// Intersection avec le disquea acretion
				float	probe_step = std::fmax(step_size * 0.05f,
					std::fmin(step_size, step_size * (r / (event_horizon_radius * 3.0f))));
				float	y_rel = pos._y - center._y;
				float	y_next = (pos._y + dir._y * probe_step) - center._y;
				bool	crosses_plane = (y_rel > 0.0f && y_next <= 0.0f) || (y_rel < 0.0f && y_next >= 0.0f);

				if (crosses_plane && std::fabs(y_rel) < disk_thickness * 4.0f)
				{
					float	r_xz = std::sqrt(
						(pos._x - center._x) * (pos._x - center._x) +
						(pos._z - center._z) * (pos._z - center._z)
					);
					if (r_xz > accretion_inner && r_xz < accretion_outer)
					{
						out_color = sampleAccretionDisk(r_xz, pos);
						return (true);
					}
				}

				// rayon sorti de la zone influente
				if (r > escape_distance)
				{
					out_ray = Ray(pos, dir);
					return (false);
				}
				// Courbure de la trajectoire acceleration centripete vers le centre,
				// intensite en 1/r^2, integree en Euler simple pas exact
				Vec3f	accel_dir = Vec3f::normalize(to_center);
				float	accel_mag = gravity_strength / (r * r);
				float	adaptive_step = std::fmin(step_size, step_size * (r / (event_horizon_radius * 3.0f)));
				adaptive_step = std::fmax(adaptive_step, step_size * 0.05f);

				dir = Vec3f::normalize(dir + accel_dir * accel_mag * adaptive_step);
				pos = pos + dir * adaptive_step;
			}

			// nombre max d'iterations
			out_ray = Ray(pos, dir);
			return (false);
		}

	private:
		Vec3f	sampleAccretionDisk(float r_xz, const Vec3f& pos) const
		{
			float	t = (r_xz - accretion_inner) / (accretion_outer - accretion_inner);
			t = std::fmin(std::fmax(t, 0.0f), 1.0f);
			Vec3f	hot(1.6f, 1.4f, 2.2f);
			Vec3f	cold(2.0f, 0.5f, 0.1f);
			Vec3f	base_color = hot * (1.0f - t) + cold * t;
			float	angle = std::atan2(pos._z - center._z, pos._x - center._x);
			float	doppler = 0.5f + 0.5f * std::sin(angle + (float)M_PI * 0.25f);
			float	intensity = 0.4f + doppler * 1.6f;
			float	noise = 0.85f + 0.15f * std::sin(angle * 9.0f + r_xz * 14.0f);

			return (base_color * intensity * noise);
		}
};
