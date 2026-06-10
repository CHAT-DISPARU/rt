/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:15:26 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/10 18:36:48 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Triangle.hpp"

#include "Triangle.hpp"
#include <cfloat>
#include <cmath>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat, Vec3f normal)
{
	_v0 = v0;
	_v1 = v1;
	_v2 = v2;
	_mat = mat;
	_normal = normal;
	_box.add_point(_v0);
	_box.add_point(_v1);
	_box.add_point(_v2);
	_box._min = _box._min - Vec3f(0.001f, 0.001f, 0.001f);
	_box._max = _box._max + Vec3f(0.001f, 0.001f, 0.001f);
}

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Material *mat)
{
	_v0 = v0;
	_v1 = v1;
	_v2 = v2;
	_mat = mat;

	Vec3f	edge1 = _v1 - _v0;
	Vec3f	edge2 = _v2 - _v0;

	_normal = Vec3f::cross(edge1, edge2);
	_normal = Vec3f::normalize(_normal);
	_box.add_point(_v0);
	_box.add_point(_v1);
	_box.add_point(_v2);
	_box._min = _box._min - Vec3f(0.001f, 0.001f, 0.001f);
	_box._max = _box._max + Vec3f(0.001f, 0.001f, 0.001f);
}

bool	Triangle::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	Vec3f	edge1 = _v1 - _v0;
	Vec3f	edge2 = _v2 - _v0;
	Vec3f	pvec = Vec3f::cross(ray._dir, edge2);	
	float	det = Vec3f::dot(edge1, pvec);
	

	if (std::abs(det) < 1e-6f)
		return (false);

	float	inv_det = 1.0f / det;
	Vec3f	tvec = ray._o - _v0;
	float	u = Vec3f::dot(tvec, pvec) * inv_det;

	if (u < 0.0f || u > 1.0f)
		return (false);
		
	Vec3f	qvec = Vec3f::cross(tvec, edge1);
	float	v = Vec3f::dot(ray._dir, qvec) * inv_det;

	if (v < 0.0f || u + v > 1.0f)
		return (false);
		
	float	t = Vec3f::dot(edge2, qvec) * inv_det;

	if (t < t_min || t > t_max)
		return (false);
	rec.t = t;
	rec.point = ray(t);
	rec.material = _mat;
	rec.set_face_normal(ray, _normal);
	return (true);
}

bool	Triangle::bbox(AABB& output_box) const
{
	output_box = _box;
	return (true);
}