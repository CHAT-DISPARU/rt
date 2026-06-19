/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 10:34:21 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/19 10:47:08 by gajanvie         ###   ########.fr       */
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
        std::pow(p[0] / 255.0f, 2.2f),
        std::pow(p[1] / 255.0f, 2.2f),
        std::pow(p[2] / 255.0f, 2.2f)
    ));
}
