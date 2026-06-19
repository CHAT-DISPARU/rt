/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Metal.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:39:21 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/19 12:23:02 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Metal.hpp"
#include "rt.hpp"

bool	Metal::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	Vec3f reflected = Vec3f::reflect(Vec3f::normalize(r_in._dir), rec.normal);

	scattered = Ray(rec.point, reflected + _fuzz * Vec3f::randomInUnitSphere(seed));
	if (_hasTexture == false || !tex)
		attenuation = _color;
	else
		attenuation = sampleTextureFast(getTexture(), rec.u, rec.v);
	pdf = 1.0f;
	return (Vec3f::dot(scattered._dir, rec.normal) > 0.0f);
}