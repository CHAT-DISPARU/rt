/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:55:06 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/11 15:43:39 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#pragma once

#include "Math3D.hpp"
#include "Ray.hpp"
#include <cmath>

class	Camera
{
	public:

		Camera(){};
		Camera(Vec3f lookfrom, Vec3f lookat, Vec3f vup, float vfov, float aspect_ratio)
		{
			m_lookfrom = lookfrom;
			m_lookat = lookat;
			m_vup = Vec3f::normalize(vup);
			m_vfov = vfov;
			m_aspect = aspect_ratio;
			m_has_moved = true;
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
			return (Vec3f::normalize(m_lookat - m_lookfrom));
		}

		Vec3f	get_right() const
		{
			return (Vec3f::normalize(Vec3f::cross(get_forward(), m_vup)));
		}

		Vec3f	get_up() const
		{
			return (m_vup);
		}

		void	move_forward(float d)
		{
			Vec3f	step = get_forward() * d;

			m_lookfrom = m_lookfrom + step;
			m_lookat = m_lookat + step;
			m_has_moved = true;
			update();
		}

		void	move_right(float d)
		{
			Vec3f	step = get_right() * d;

			m_lookfrom = m_lookfrom + step;
			m_lookat = m_lookat + step;
			m_has_moved = true;
			update();
		}

		void	move_up_world(float d)
		{
			m_lookfrom._y += d;
			m_lookat._y += d;
			m_has_moved = true;
			update();
		}

		void	pitch(float angle)
		{
			Vec3f	axis = get_right();
			Mat4f	rot = Mat4f::rotate(angle, axis);

			Vec3f	dir = m_lookat - m_lookfrom;

			dir = rot * dir;
			m_lookat = m_lookfrom + dir;
			m_has_moved = true;
			update();
		}

		void	yaw(float angle)
		{
			Mat4f	rot = Mat4f::rotateY(angle);

			Vec3f	dir = m_lookat - m_lookfrom;

			dir = rot * dir;
			m_lookat = m_lookfrom + dir;
			m_has_moved = true;
			update();
		}

		void	roll(float angle)
		{
			Mat4f	rot = Mat4f::rotate(angle, get_forward());

			m_vup = Vec3f::normalize(rot * m_vup);
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

		Ray	getRay(float u_coord, float v_coord) const
		{
			float	ndc_x = (2.0f * u_coord) - 1.0f;
			float	ndc_y = 1.0f - (2.0f * v_coord);
			float half_h = std::tan((m_vfov * M_PI / 180.0f) * 0.5f);
			float half_w = half_h * m_aspect;
			Vec3f	forward = Vec3f::normalize(m_lookat - m_lookfrom);
			Vec3f	right = Vec3f::normalize(Vec3f::cross(forward, m_vup));
			Vec3f	up = Vec3f::cross(right, forward);
			Vec3f	direction = forward + right * (ndc_x * half_w) + up    * (ndc_y * half_h);

			return (Ray(m_lookfrom, direction.normalize()));
		}

	private:

		void	update()
		{
			float	fov_rad = m_vfov * 3.14159265f / 180.0f;
			m_view = Mat4f::lookAt(m_lookfrom, m_lookat, m_vup);
			m_proj = Mat4f::perspective(fov_rad, m_aspect, 0.1f, 100.0f);
			m_inverseView = m_view.inverse();
			m_inverseProj = m_proj.inverse();
		}

		Vec3f	m_lookfrom;
		Vec3f	m_lookat;
		Vec3f	m_vup;
		float	m_vfov;
		float	m_aspect;
		bool	m_has_moved;
		Mat4f	m_view;
		Mat4f	m_proj;
		Mat4f	m_inverseView;
		Mat4f	m_inverseProj;
};
