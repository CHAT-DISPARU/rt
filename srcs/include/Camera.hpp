/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:26:01 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 10:51:25 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Math3D.hpp"
#include "Ray.hpp"
#include <cmath>

class	Camera
{
	public:
		Camera(Vec3f lookfrom, Vec3f lookat, Vec3f vup, float vfov, float aspect_ratio)
		{
			m_lookfrom = lookfrom;
			m_lookat = lookat;
			m_vup = vup;
			m_vfov = vfov;
			m_aspect_ratio = aspect_ratio;
			update();
		}

		void	move(const Vec3f& offset)
		{
			m_lookfrom = m_lookfrom + offset;
			m_lookat = m_lookat + offset;
			update();
		}

		void	moveLocal(float right, float up, float forward)
		{
			Vec3f offset = (u * right) + (v * up) - (w * forward);

			m_lookfrom = m_lookfrom + offset;
			m_lookat = m_lookat + offset;
			update();
		}

		void	rotate(const Mat4f& rotationMatrix)
		{
			Vec3f	dir = m_lookat - m_lookfrom;

			dir = rotationMatrix * dir;
			m_lookat = m_lookfrom + dir;
			update();
		}

		Ray	getRay(float u_coord, float v_coord) const
		{
			Vec3f	pixel_pos = lower_left_corner + (u_coord * horizontal) + (v_coord * vertical);
			Vec3f	direction = pixel_pos - m_lookfrom;

			return (Ray(m_lookfrom, direction));
		}

	private:

		void	update()
		{
			float	theta = m_vfov * 3.14159265f / 180.0f;
			float	h = std::tan(theta / 2.0f);
			float	viewport_height = 2.0f * h;
			float	viewport_width = m_aspect_ratio * viewport_height;

			w = (m_lookfrom - m_lookat).normalize();
			u = Vec3f::cross(m_vup, w).normalize();
			v = Vec3f::cross(w, u);

			horizontal = viewport_width * u;
			vertical = viewport_height * v;
			lower_left_corner = m_lookfrom - (horizontal / 2.0f) - (vertical / 2.0f) - w;
		}

		Vec3f	m_lookfrom;
		Vec3f	m_lookat;
		Vec3f	m_vup;
		float	m_vfov;
		float	m_aspect_ratio;
		Vec3f	u;
		Vec3f	v;
		Vec3f	w;
		Vec3f	horizontal;
		Vec3f	vertical;
		Vec3f	lower_left_corner;
};
