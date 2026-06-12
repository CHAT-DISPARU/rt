/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dielectric.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:13 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/12 15:58:35 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Dielectric : public Material
{
	private:
		float	_ni;
		float	_fuzz;

	public:

		~Dielectric(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, unsigned int* seed) const;
		float	ior() const;
		void setFuzz(float f)
		{
			_fuzz = (f < 1.0f) ? f : 1.0f;
		};
		void setNi(float ni)
		{
			_ni = ni;
		};
		bool	isSpecular() const
		{
			return true;
		};
		bool	isOpaq() const
		{
			return false;
		};
};
