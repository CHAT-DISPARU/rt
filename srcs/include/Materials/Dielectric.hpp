/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dielectric.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:13 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 18:42:06 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Dielectric : public Material
{
	public:

		~Dielectric(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered) const;
		Vec3f	emitted(float u, float v, const Vec3f& p) const;
};
