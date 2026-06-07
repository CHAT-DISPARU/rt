/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Metal.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:39:21 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/07 16:31:52 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Metal.hpp"

bool	Metal::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const
{
	Vec3f reflected = Vec3f::reflect(Vec3f::normalize(r_in._dir), rec.normal);

	scattered = Ray(rec.point, reflected + _fuzz * Vec3f::randomInUnitSphere(seed));
	attenuation = _color;
	return (Vec3f::dot(scattered._dir, rec.normal) > 0.0f);
}