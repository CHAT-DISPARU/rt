/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:03:46 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/14 23:03:06 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lambertian.hpp"

bool	Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	Vec3f	scatterDir = rec.normal + Vec3f::randomUnitVector(seed);

	(void)r_in;
	if (scatterDir.nearZero())
		scatterDir = rec.normal;
	scattered = Ray(rec.point, scatterDir);
	attenuation = _color;
	float	cosine = Vec3f::dot(rec.normal, Vec3f::normalize(scatterDir));
	pdf = cosine < 0.0f ? 0.0f : cosine / (float)M_PI;
	return (true);
}

float Lambertian::scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	(void)r_in;
	float cosine = Vec3f::dot(rec.normal, Vec3f::normalize(scattered._dir));
	return (cosine < 0.0f ? 0.0f : cosine / (float)M_PI);
}
