/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Triangle.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:15:26 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 18:37:55 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Triangle.hpp"

Triangle::Triangle(Vec3f points, Material *mat, Vec3f normal)
{
	_points = points;
	_mat = mat;
	_box.add_point(_points._x);
	_box.add_point(_points._y);
	_box.add_point(_points._z);
	_box._min = _box._min - Vec3f(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);
	_box._max = _box._max + Vec3f(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);
	_normal = normal;
}

Triangle::Triangle(Vec3f points, Material *mat)
{
	_points = points;
	_mat = mat;
	_box.add_point(_points._x);
	_box.add_point(_points._y);
	_box.add_point(_points._z);
	_box._min = _box._min - Vec3f(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);
	_box._max = _box._max + Vec3f(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);
}

/*
	R(t) = O + t * D
	O: origin of the ray
	D: direction
	t: distance along the ray

	E1 = P2 - P1
	E2 = P3 - P1

	Then we perform some cross products to solve the system:
		H = D x E2
		a = E1 . H

	If a is close to 0, the ray is parallel to the triangle → no intersection.
	Otherwise we continue:
		f = 1 / a
		S = O - P1
		u = f * (S . H)
		if u < 0 or u > 1 → no intersection
		Q = S x E1
		v = f * (D . Q)
		if v < 0 or u + v > 1 → no intersection

	We finally compute t, the distance along the ray:
		t = f * (E2 . Q)

	If t > 0, the ray hits the triangle at the point:
		P = O + t * D

	Recovered barycentric coordinates:
		u, v already computed
		w = 1 - u - v
*/
bool	Triangle::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	Vec3f	edge1;
	Vec3f	edge2;
	Vec3f	pvec;
	Vec3f	tvec;
	Vec3f	qvec;
	float	det;
	float	u;
	float	v;
	float	t;

	edge1 = _points._y - _points._x;
	edge2 = _points._z - _points._x;
	pvec = Vec3f::cross(ray._dir, edge2);
	det = Vec3f::dot(edge1, edge2);
	if (det > -t_min && det < t_min)
		return (false);
	det = 1.0f / det;
	tvec = ray._o - _points._x;
	u = Vec3f::dot(tvec, pvec) * det;
	if (u < 0.0f || u > 1.0f)
		return (false);
	qvec = Vec3f::cross(tvec, edge1);
	v = Vec3f::dot(ray._dir, qvec) * det;
	if (v < 0.0f || u + v > 1.0f)
		return (false);
	t = Vec3f::dot(edge2, qvec) * det;
	if (t < t_min || t > t_max)
		return (false);
	rec.t = t;
	rec.point = ray(t);
	rec.set_face_normal(ray, _normal);
}

bool	Triangle::bbox(AABB& output_box) const
{
	output_box = _box;
	return (true);
}
