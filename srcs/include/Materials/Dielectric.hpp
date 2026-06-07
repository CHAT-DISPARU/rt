/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dielectric.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:13 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/07 16:17:48 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Dielectric : public Material
{
	private:
		Vec3f	_color;
		float	_ni;
		float	_fuzz;

	public:

		~Dielectric(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const;
		float	ior() const;
};
