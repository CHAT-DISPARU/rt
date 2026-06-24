/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:03:46 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/24 11:16:41 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lambertian.hpp"
#include "rt.hpp"

bool	Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	(void)r_in;
	Vec3f	albedo = sampleAlbedo(rec.u, rec.v);
	float	ao = sampleAO(rec.u, rec.v);
	Vec3f	scatter_direction = rec.normal + Vec3f::randomUnitVector(seed);

	if (scatter_direction.length_sq() < 1e-8f)
		scatter_direction = rec.normal;
	scattered = Ray(rec.point, scatter_direction);
	attenuation = albedo * ao;
	pdf = Vec3f::dot(rec.normal, scattered._dir) / (float)M_PI;
	return (true);
}

float	Lambertian::scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	(void)r_in;
	float cosine = Vec3f::dot(rec.normal, Vec3f::normalize(scattered._dir));
	return (cosine < 0.0f ? 0.0f : cosine / (float)M_PI);
}
