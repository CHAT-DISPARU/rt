/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:15:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/14 21:56:46 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Plane.hpp"

Plane::Plane(Vec3f point, Vec3f normal, const Mat4f &m, Material *mat)
{
	_point = point;
	_transform = m;
	_inverse = _transform.inverse();
	_mat = mat;
	_normal = normal;
}

bool	Plane::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	Ray		l_ray = ray;
	Vec3f	local_p;
	float	denom;
	float	t;

	l_ray *= _inverse;
	denom = l_ray._dir._y;
	if (fabs(denom) < 1e-6f)
		return (false);
	t = -l_ray._o._y / denom;
	if (t < t_min || t > t_max)
		return (false);
	rec.t = t;
	rec.material = _mat;
	rec.point = ray(t);
	rec.set_face_normal(ray, _normal);
	return (true);
}

bool	Plane::bbox(AABB& output_box) const
{
	output_box = AABB(Vec3f(-10000, -0.01f, -10000), Vec3f(10000, 0.01f, 10000));
	return (true);
}
