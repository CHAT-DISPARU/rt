/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 10:34:21 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/20 12:05:27 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.hpp"

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