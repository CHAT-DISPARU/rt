/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quad.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:33:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/24 11:46:33 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Quad.hpp"

Quad::Quad(Vec3f center, Vec3f normal, const Mat4f &m, Material *mat, float w, float h)
{
	_w = w;
	_h = h;
	_transform = m;
	_inverse = _transform.inverse();
	_center = center;
	float	thick = 0.001f;
	_box._min = Vec3f(-1, -thick, -1);
	_box._max = Vec3f(1, thick, 1);
	_box *= _transform;
	_mat = mat;
	_normal = normal;
}

bool	Quad::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec, int* node_tests) const
{
	(void)node_tests;
	Ray	l_ray = ray;
	float	t;
	float	denom;
	Vec3f	p;

	l_ray *= _inverse;
	denom = l_ray._dir._y;
	if (fabs(denom) < 1e-6f)
		return (false);
	t = -l_ray._o._y / denom;
	if (t < tMin || t > tMax)
		return (false);
	p = l_ray._o + (l_ray._dir * t);
	if (fabs(p._x) > 1.0 || fabs(p._z) > 1.0)
		return (false);
	rec.t = t;
	rec.material = _mat;
	rec.point = ray(t);
	rec.set_face_normal(ray, _normal);
	rec.u = ((p._x + 1.0f) * 0.5f) * _w;
	rec.v = ((p._z + 1.0f) * 0.5f) * _h;
	return (true);
}

bool	Quad::bbox(AABB& output_box) const
{
	output_box = _box;
	return (true);
}

Vec3f Quad::sample(const Vec3f& origin, uint32_t* seed) const
{
	(void)origin;
	float	r1 = Vec3f::randomFloat(seed);
	float	r2 = Vec3f::randomFloat(seed);
	float	local_x = r1 * 2.0f - 1.0f;
	float	local_y = 0.0f;
	float	local_z = r2 * 2.0f - 1.0f;
	Vec3f	local_p(local_x, local_y, local_z);

	return (_transform * local_p);
}

float	Quad::pdf_value(const Vec3f& origin, const Vec3f& dir) const
{
	HitRecord	rec;
	if (!this->hit(Ray(origin, dir), 1e-4f, FLT_MAX, rec))
		return (0.0f);
	float	area = _w * _h;
	float	distance_squared = rec.t * rec.t;
	float	cosine = std::fmax(std::fabs(Vec3f::dot(dir, rec.normal)), 1e-3f);

	if (cosine < 1e-6f || area < 1e-6f)
		return (0.0f);
	return (distance_squared / (cosine * area));
}