/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Metal.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:39:21 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/24 11:16:15 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Metal.hpp"
#include "rt.hpp"

bool	Metal::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	Vec3f	albedo = sampleAlbedo(rec.u, rec.v);
	float	fuzz = sampleRoughness(rec.u, rec.v, _fuzz);
	Vec3f	reflected = Vec3f::reflect(Vec3f::normalize(r_in._dir), rec.normal);
	scattered = Ray(rec.point, reflected + Vec3f::randomInUnitSphere(seed) * fuzz);
	
	attenuation = albedo;
	pdf = 1.0f;
	return (Vec3f::dot(scattered._dir, rec.normal) > 0.0f);
}
