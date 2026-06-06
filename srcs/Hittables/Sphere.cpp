/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sphere.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 13:14:43 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 17:15:07 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sphere.hpp"

Sphere::Sphere(float d, Vec3f center, const Mat4f &m, Material *mat)
{
	_diameter = d;
	_transform = m;
	_inverse = _transform.inverse();
	_center = center;
	_box._min = Vec3f(-1, -1, -1);
	_box._max = Vec3f(1, 1, 1);
	_box *= _transform;
	_mat = mat;
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
bool	Sphere::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	Ray		l_ray;
	Vec3f	local_normal;
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
	t = ((b - std::sqrt(delta)) / a);
	if (t < t_min || t > t_max)
	{
		t = ((b + std::sqrt(delta)) / a);
		if (t < t_min || t > t_max)
			return (false);
	}
	rec.t = t;
	rec.material = _mat;
	rec.point = ray(t);
	rec.set_face_normal(ray, Vec3f::normalize(_transform * Vec3f::normalize(rec.point)));
	
}

bool	Sphere::bbox(AABB& output_box) const
{
	output_box = _box;
	return (true);
}
