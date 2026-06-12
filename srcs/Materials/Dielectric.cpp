/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dielectric.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:38:58 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/11 21:24:54 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dielectric.hpp"

static float	schlick(float cosine, float ior_in, float ior_out)
{
	float	r0 = (ior_in - ior_out) / (ior_in + ior_out);

	r0 = r0 * r0;
	return (r0 + (1.0f - r0) * pow(1.0f - cosine, 5.0f));
}

bool	Dielectric::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const
{
	attenuation = _color;

	float	ratio = rec.front_face ? (rec.ni_from / _ni) : (_ni / rec.ni_from);
	Vec3f	unitDir = Vec3f::normalize(r_in._dir);
	float	cosTheta = std::fmin(Vec3f::dot(-unitDir, rec.normal), 1.0f);
	float	sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	bool	noRefract = ratio * sinTheta > 1.0f;
	Vec3f	direction;

	if (noRefract || schlick(cosTheta, rec.ni_from, _ni) > Vec3f::randomFloat(seed))
		direction = Vec3f::reflect(unitDir, rec.normal);
	else
		direction = Vec3f::refract(unitDir, rec.normal, ratio);
	
	Vec3f finalDir = Vec3f::normalize(direction + _fuzz * Vec3f::randomInUnitSphere(seed));
	scattered = Ray(rec.point, finalDir);
	return (true);
}

float Dielectric::ior() const
{
	return (_ni);
}
