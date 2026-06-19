/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sphere.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 13:14:43 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 10:27:46 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sphere.hpp"

Sphere::Sphere(float d, Vec3f center, Vec3f normal, const Mat4f &m, Material *mat)
{
	_diameter = d;
	_transform = m;
	_inverse = _transform.inverse();
	_center = center;
	_box._min = Vec3f(-1, -1, -1);
	_box._max = Vec3f(1, 1, 1);
	_box *= _transform;
	_mat = mat;
	_normal = normal;
	_radius = _diameter / 2.0f;
}

/*
	D direction O origine t inconue

	Ray(t) = o + t * D;

	tous les point f d'une sphere

	||f - c|| = r²; longeur vecteur au carre
	est strictement egual au produit scalire de lui meme
	(f - c) * (f - c) = r²
	((O + t * D) - C) * ((O + t * D) - C) = r²
	(OC + TD) * (OC + TD) = r²

	(a + b)² = a² + 2ab + b²

	(OC * OC) + 2(OC * TD) + TD² = r²
	(OC * OC) + 2t(OC * D) + t²(D * D) = r²
	(OC * OC) + 2t(OC * D) + t²(D * D) - r² = 0

	b sera tjr egual a 2 * un vecteur
	le vecteur g par exemple
	deter plus facile a calculer

	−b±√b²-4ac
		2a
	−(−2g)±√4(g)²−4ac
			2a

	2g±2√g²−ac  ->   g±√g²−ac
		2a				a


		
*/
bool	Sphere::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec, int* node_tests) const
{
	(void)node_tests;
	Ray		l_ray = ray;
	float	a;
	float	b;
	float	c;
	
	l_ray *= _inverse;
	a = Vec3f::dot(l_ray._dir, l_ray._dir);
	b = Vec3f::dot(l_ray._o, l_ray._dir);
	c = Vec3f::dot(l_ray._o, l_ray._o) - 1.0f;

	float	delta;
	float	t;
	
	// b²-ac
	delta = b * b - a * c;
	if (delta < 0.0f)
		return (false);
	//g±√g²−ac
	//	2a
	float	sqrtd = std::sqrt(delta);

	t = (-b - sqrtd) / a;
	
	if (t < tMin || t > tMax)
	{
		t = (-b + sqrtd) / a;
		if (t < tMin || t > tMax)
			return (false);
	}
	rec.t = t;
	rec.material = _mat;
	rec.point = ray(t);
	Vec3f	out_normal = Vec3f::normalize(rec.point - _center);
	rec.set_face_normal(ray, out_normal);
	Vec3f local_p = l_ray(t);
	float	phi;
	float	theta;

	phi = atan2(local_p._z, local_p._x);
	float	y_clamped = std::fmax(-1.0f, std::fmin(1.0f, local_p._y));
	theta = asin(y_clamped);
	rec.u = 1.0 - ((phi + M_PI) / (2.0 * M_PI));
	rec.v = (theta + M_PI / 2.0) / M_PI;
	return (true);
}

bool	Sphere::bbox(AABB& output_box) const
{
	output_box = _box;
	return (true);
}

Vec3f	Sphere::sample(const Vec3f& origin, uint32_t* seed) const
{
	Vec3f	oc = _center - origin;
	float	dist2 = oc.length_sq();
	float	dist = std::sqrt(dist2);

	//dans sphere
	if (dist2 <= _radius * _radius)
		return (_center + Vec3f::randomUnitVector(seed) * _radius);

	// tire cone fisible
	float	cos_max = std::sqrt(1.0f - (_radius * _radius) / dist2);
	float	r1 = Vec3f::randomFloat(seed);
	float	r2 = Vec3f::randomFloat(seed);
	float	cos_t = 1.0f - r1 * (1.0f - cos_max);
	float	sin_t = std::sqrt(std::fmax(0.0f, 1.0f - cos_t * cos_t));
	float	phi = 2.0f * (float)M_PI * r2;
	//base ortonorme aolign sur oc
	Vec3f	w = oc / dist;
	Vec3f	up = (std::fabs(w._x) > 0.9f)
				? Vec3f(0.0f, 1.0f, 0.0f) : Vec3f(1.0f, 0.0f, 0.0f);
	Vec3f	u = Vec3f::normalize(Vec3f::cross(up, w));
	Vec3f	v = Vec3f::cross(w, u);

	//dir -> cone
	Vec3f	dir = u * (std::cos(phi) * sin_t)
				+ v * (std::sin(phi) * sin_t) + w * cos_t;
	return (origin + Vec3f::normalize(dir) * dist);
}

float Sphere::pdf_value(const Vec3f& origin, const Vec3f& dir) const
{
	HitRecord	rec;
	if (!this->hit(Ray(origin, dir), 1e-4f, FLT_MAX, rec))
		return (0.0f);
	Vec3f	oc = _center - origin;
	float	dist2 = oc.length_sq();

	if (dist2 <= _radius * _radius)
		return (1.0f / (4.0f * (float)M_PI * _radius * _radius));

	float	cos_max = std::sqrt(1.0f - (_radius * _radius) / dist2);
	float	solid_angle = 2.0f * (float)M_PI * (1.0f - cos_max);
	if (solid_angle < 1e-6f)
		return (0.0f);
	return (solid_angle < 1e-8f) ? 0.0f : (1.0f / solid_angle);
}