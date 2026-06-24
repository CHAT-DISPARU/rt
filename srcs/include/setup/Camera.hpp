/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:55:06 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/23 20:13:22 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"
#include "Quaternion.hpp"
#include <cmath>

class	Camera
{
	public:

		Camera(){};
		Camera(Vec3f lookfrom, Vec3f lookat, Vec3f vup, float vfov, float aspect_ratio)
		{
			(void)vup;
			m_lookfrom = lookfrom;
			m_vfov = vfov;
			m_aspect = aspect_ratio;
			m_has_moved = true;

			// calcul yam et pitch
			Vec3f	forward = Vec3f::normalize(lookat - lookfrom);
			float	yaw = std::atan2(-forward._x, -forward._z);
			float	pitch = std::asin(forward._y);
			m_orientation = Quatf::fromAxisAngle(yaw, Vec3f(0.0f, 1.0f, 0.0f))
						  * Quatf::fromAxisAngle(pitch, Vec3f(1.0f, 0.0f, 0.0f));
			update();
		}
		
		float	get_fov() const
		{
			return (m_vfov);
		}

		void	set_fov(float fov)
		{
			m_vfov = fov;
			update();
		}

		Vec3f	get_pos() const
		{
			return (m_lookfrom);
		}

		Vec3f	get_forward() const
		{
			return (m_orientation * Vec3f(0.0f, 0.0f, -1.0f));
		}

		Vec3f	get_right() const
		{
			return (m_orientation * Vec3f(1.0f, 0.0f, 0.0f));
		}

		Vec3f	get_up() const
		{
			return (m_orientation * Vec3f(0.0f, 1.0f, 0.0f));
		}

		void	move_forward(float d)
		{
			m_lookfrom += get_forward() * d;
			m_has_moved = true;
			update();
		}

		void	move_right(float d)
		{
			m_lookfrom += get_right() * d;
			m_has_moved = true;
			update();
		}

		void	move_up_world(float d)
		{
			m_lookfrom._y += d;
			m_has_moved = true;
			update();
		}
		void	pitch(float angle)
		{
			Quatf	q = Quatf::fromAxisAngle(angle, Vec3f(1.0f, 0.0f, 0.0f));
			m_orientation = (m_orientation * q).normalize();
			m_has_moved = true;
			update();
		}
		void	yaw(float angle)
		{
			Quatf	q = Quatf::fromAxisAngle(angle, Vec3f(0.0f, 1.0f, 0.0f));
			m_orientation = (q * m_orientation).normalize();
			m_has_moved = true;
			update();
		}
		void	roll(float angle)
		{
			Quatf	q = Quatf::fromAxisAngle(angle, Vec3f(0.0f, 0.0f, -1.0f));
			m_orientation = (m_orientation * q).normalize();
			m_has_moved = true;
			update();
		}
		bool	hasMoved() const
		{
			return (m_has_moved);
		}

		void	resetMovedFlag()
		{
			m_has_moved = false;
		}

		Ray	getRay(float u_coord, float v_coord, unsigned int* seed) const
		{
			float	ndc_x = (2.0f * u_coord) - 1.0f;
			float	ndc_y = 1.0f - (2.0f * v_coord);
			float	half_h = std::tan((m_vfov * (float)M_PI / 180.0f) * 0.5f);
			float	half_w = half_h * m_aspect;
			Vec3f	forward = get_forward();
			Vec3f	right = get_right();
			Vec3f	up = get_up();
			//dir classique
			Vec3f	direction = forward + right * (ndc_x * half_w) + up * (ndc_y * half_h);
			//focal plane
			Vec3f	focal_point = m_lookfrom + direction * m_focus_dist;
			Vec3f	origin = m_lookfrom;
			
			if (m_lens_radius > 0.0f)
			{
				Vec3f	rd = m_lens_radius * Vec3f::randomInUnitDisk(seed);
				origin += right * rd._x + up * rd._y;
			}
			return (Ray(origin, Vec3f::normalize(focal_point - origin)));
		}
		void	set_dof(float lens_radius, float focus_dist)
		{
			m_lens_radius = lens_radius;
			m_focus_dist = focus_dist;
		}
		float	get_lens_radius() const
		{
			return (m_lens_radius);
		}
		float	get_focus_dist() const
		{
			return (m_focus_dist);
		}

	private:

		void	update()
		{
			m_view = m_orientation.conjugate().toMat4() * Mat4f::translate(-m_lookfrom);
			m_inverseView = m_view.inverse();
			
			float	fov_rad = m_vfov * (float)M_PI / 180.0f;
			m_proj = Mat4f::perspective(fov_rad, m_aspect, 0.1f, 100.0f);
			m_inverseProj = m_proj.inverse();
		}

		Vec3f	m_lookfrom;
		Quatf	m_orientation;
		float	m_vfov;
		float	m_aspect;
		bool	m_has_moved;
		Mat4f	m_view;
		Mat4f	m_proj;
		Mat4f	m_inverseView;
		Mat4f	m_inverseProj;
		float	m_lens_radius = 0.0f;
		float	m_focus_dist = 10.0f;
};
