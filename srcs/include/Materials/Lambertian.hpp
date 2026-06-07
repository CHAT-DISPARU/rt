/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lambertian.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:05 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/07 15:31:15 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Lambertian : public Material
{
	private:
		Vec3f	_color;
	public:

		~Lambertian(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const;
};
