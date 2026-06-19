/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Plane.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:15:29 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 12:14:52 by gajanvie         ###   ########.fr       */
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

bool	Plane::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec, int* node_tests) const
{
	(void)node_tests;
	Ray		l_ray = ray;
	Vec3f	local_p;
	float	denom;
	float	t;

	l_ray *= _inverse;
	denom = l_ray._dir._y;
	if (fabs(denom) < tMin)
		return (false);
	t = -l_ray._o._y / denom;
	if (t < tMin || t > tMax)
		return (false);
	rec.t = t;
	rec.material = _mat;
	rec.point = ray(t);
	rec.set_face_normal(ray, _normal);
	local_p = l_ray._o + (l_ray._dir * t);
	float	scale = _mat->getTexScale();
	rec.u = std::fmod(local_p._x / scale, 1.0f);
	rec.v = std::fmod(local_p._z / scale, 1.0f);
	if (rec.u < 0.0f)
		rec.u += 1.0f;
	if (rec.v < 0.0f)
		rec.v += 1.0f;
	return (true);
}

bool	Plane::bbox(AABB& output_box) const
{
	output_box = AABB(Vec3f(-10000, -10000, -10000), Vec3f(10000, 10000, 10000));
	return (true);
}
