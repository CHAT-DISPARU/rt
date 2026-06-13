/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DiffuseLight.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:32:19 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/13 11:13:56 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DiffuseLight.hpp"

bool	DiffuseLight::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const
{
	(void)r_in;
	(void)rec;
	(void)attenuation;
	(void)scattered;
	(void)seed;
	return (false);
}

Vec3f	DiffuseLight::emitted(float u, float v, const Vec3f& p) const
{
	(void)u;
	(void)v;
	(void)p;
	return (_color * _intensity);
}