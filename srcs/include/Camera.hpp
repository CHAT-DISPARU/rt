/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:26:01 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 12:57:31 by gajanvie         ###   ########.fr       */
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
			m_vup = vup;
			m_vfov = vfov;
			m_aspect_ratio = aspect_ratio;
			m_has_moved = true;
			update();
		}

		void	move(const Vec3f& offset)
		{
			m_lookfrom = m_lookfrom + offset;
			m_lookat = m_lookat + offset;
			m_has_moved = true;
			update();
		}

		void	moveLocal(float right, float up, float forward)
		{
			Vec3f	offset_local(right, up, -forward);
			Vec3f	offset_world = m_inverseView * offset_local;

			m_lookfrom = m_lookfrom + offset_world;
			m_lookat = m_lookat + offset_world;
			m_has_moved = true;
			update();
		}

		void	rotate(const Mat4f& rotationMatrix)
		{
			Vec3f	dir = m_lookat - m_lookfrom;

			dir = rotationMatrix * dir;
			m_lookat = m_lookfrom + dir;
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

			float	cam_x = ndc_x * m_inverseProj[0][0];
			float	cam_y = ndc_y * m_inverseProj[1][1];

			Vec3f	dir_camera(cam_x, cam_y, -1.0f);
			Vec3f	direction = m_inverseView * dir_camera;

			return (Ray(m_lookfrom, direction.normalize()));
		}

	private:

		void	update()
		{
			float	fov_radians = m_vfov * 3.14159265f / 180.0f;

			m_view = Mat4f::lookAt(m_lookfrom, m_lookat, m_vup);
			m_proj = Mat4f::perspective(fov_radians, m_aspect_ratio, 0.1f, 100.0f);

			m_inverseView = m_view.inverse();
			m_inverseProj = m_proj.inverse();
		}

		Vec3f	m_lookfrom;
		Vec3f	m_lookat;
		Vec3f	m_vup;
		float	m_vfov;
		float	m_aspect_ratio;
		bool	m_has_moved;
		Mat4f	m_view;
		Mat4f	m_proj;
		Mat4f	m_inverseView;
		Mat4f	m_inverseProj;
};