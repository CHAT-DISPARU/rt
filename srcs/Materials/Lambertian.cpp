/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:03:46 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/07 16:31:54 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lambertian.hpp"

bool	Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const
{
	Vec3f	scatterDir = rec.normal + Vec3f::randomUnitVector(seed);

	if (scatterDir.nearZero())
		scatterDir = rec.normal;
	scattered = Ray(rec.point, scatterDir);
	attenuation = _color;
	return (true);
}
