/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Metal.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:03 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/07 17:30:33 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Metal : public Material
{
	private:
		Vec3f	_color;
		float	_fuzz;
		
	public:
		Metal(const Vec3f& color, float fuzz) : _color(color), _fuzz(fuzz) {};
		~Metal(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const;
};
