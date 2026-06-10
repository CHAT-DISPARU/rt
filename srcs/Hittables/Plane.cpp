/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:15:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 01:26:15 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Plane.hpp"

Plane::Plane(float point, const Mat4f &m, Material *mat)
{
	_point = point;
	_transform = m;
	_inverse = _transform.inverse();
	_mat = mat;
}

bool	Plane::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	Ray		l_ray;
	Vec3f	local_p;
	float	denom;
	float	t;

	l_ray *= _inverse;
	denom = l_ray._dir._y;
	if (fabs(denom) < t_min)
		return (false);
	t = -l_ray._o._y / denom;
	if (t < t_min || t > t_max)
		return (false);
	rec.t = t;
	rec.material = _mat;
	rec.point = l_ray(t);
	rec.set_face_normal(ray, Vec3f::normalize(_transform * Vec3f::normalize(rec.point)));
	return (true);
}

bool	Plane::bbox(AABB& output_box) const
{
	(void)output_box;
	return (false);
}
