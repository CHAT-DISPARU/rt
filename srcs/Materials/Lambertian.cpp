/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:03:46 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/19 12:22:56 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lambertian.hpp"
#include "rt.hpp"

bool	Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	Vec3f	scatterDir = rec.normal + Vec3f::randomUnitVector(seed);

	(void)r_in;
	if (scatterDir.nearZero())
		scatterDir = rec.normal;
	scattered = Ray(rec.point, scatterDir);
	if (_hasTexture == false || !tex)
		attenuation = _color;
	else
		attenuation = sampleTextureFast(getTexture(), rec.u, rec.v);
	float	cosine = Vec3f::dot(rec.normal, Vec3f::normalize(scatterDir));
	pdf = cosine < 0.0f ? 0.0f : cosine / (float)M_PI;
	return (true);
}

float	Lambertian::scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	(void)r_in;
	float cosine = Vec3f::dot(rec.normal, Vec3f::normalize(scattered._dir));
	return (cosine < 0.0f ? 0.0f : cosine / (float)M_PI);
}
