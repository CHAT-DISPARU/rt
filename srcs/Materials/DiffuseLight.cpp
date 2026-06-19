/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DiffuseLight.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:32:19 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/19 12:23:06 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DiffuseLight.hpp"
#include "rt.hpp"

bool	DiffuseLight::scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const
{
	(void)r_in;
	(void)rec;
	(void)attenuation;
	(void)scattered;
	(void)pdf;
	(void)seed;
	return (false);
}

Vec3f	DiffuseLight::emitted(float u, float v, const Vec3f& p) const
{
	(void)u;
	(void)v;
	(void)p;
	Vec3f color;
	if (_hasTexture == false || !tex)
		color = _color;
	else
		color = sampleTextureFast(getTexture(), u, v);
	return (color * _intensity);
}