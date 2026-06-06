/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:05 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 18:41:58 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Lambertian : public Material
{
	public:

		~Lambertian(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered) const;
		Vec3f	emitted(float u, float v, const Vec3f& p) const;
};
