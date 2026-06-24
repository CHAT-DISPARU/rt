/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 10:34:21 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/24 11:59:20 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

void	applyNormalMap(HitRecord& rec)
{
	if (!rec.material || !rec.material->hasNormal())
		return ;

	SDL_Surface	*normal_map = rec.material->getNormal();
	float		scale = rec.material->getNormalScale();
	float	u = rec.u * scale - std::floor(rec.u * scale);
	float	v = rec.v * scale - std::floor(rec.v * scale);
	int	x = std::min((int)(u * normal_map->w), normal_map->w - 1);
	int	y = std::min((int)((1.0f - v) * normal_map->h), normal_map->h - 1);
	Uint8	*pixels = (Uint8*)normal_map->pixels;
	int		bpp = SDL_BYTESPERPIXEL(normal_map->format);
	Uint8	*p = pixels + y * normal_map->pitch + x * bpp;
	float	strength = rec.material->getNormalStrength();

	Vec3f	tangent_normal(
		p[0] / 255.0f * 2.0f - 1.0f* strength,
		p[1] / 255.0f * 2.0f - 1.0f* strength,
		p[2] / 255.0f * 2.0f - 1.0f
	);
	tangent_normal = Vec3f::normalize(tangent_normal);
	if (rec.tangent.length_sq() < 1e-6f)
	{
		Vec3f	up_guide(0.0f, 1.0f, 0.0f);
		if (std::abs(Vec3f::dot(rec.normal, up_guide)) > 0.99f)
			up_guide = Vec3f(0.0f, 0.0f, 1.0f);
		rec.tangent = Vec3f::normalize(Vec3f::cross(up_guide, rec.normal));
		rec.bitangent = Vec3f::normalize(Vec3f::cross(rec.normal, rec.tangent));
	}
	Vec3f	T = rec.tangent;
	Vec3f	B = rec.bitangent;
	Vec3f	N = rec.normal;

	rec.normal = Vec3f::normalize(T * tangent_normal._x + B * tangent_normal._y + N * tangent_normal._z);
}

Vec3f	sampleTextureFast(SDL_Surface* surface, float u, float v)
{
	u = u - std::floor(u);
	v = v - std::floor(v);
	int	x = std::min((int)(u * surface->w), surface->w - 1);
	int	y = std::min((int)((1.0f - v) * surface->h), surface->h - 1);
	Uint8*	pixels = (Uint8*)surface->pixels;
	Uint8*	p = pixels + y * surface->pitch + x * 4;

	return (Vec3f(
		p[0] / 255.0f,
		p[1] / 255.0f,
		p[2] / 255.0f
	));
}

float	sampleScalarLinear(SDL_Surface* surface, float u, float v, int channel)
{
	if (!surface)
		return (-1.0f);

	u = u - std::floor(u);
	v = v - std::floor(v);
	int		x = std::min((int)(u * surface->w), surface->w - 1);
	int		y = std::min((int)((1.0f - v) * surface->h), surface->h - 1);
	Uint8*	pixels = (Uint8*)surface->pixels;
	int		bpp = SDL_BYTESPERPIXEL(surface->format);
	Uint8*	p = pixels + y * surface->pitch + x * bpp;
	if (bpp == 1)
		return (p[0] / 255.0f);
	int	c = std::min(channel, bpp - 1);
	return (p[c] / 255.0f);
}