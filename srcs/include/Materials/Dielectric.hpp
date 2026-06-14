/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dielectric.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:13 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/14 22:28:54 by CHAT-DISPAR      ###   ########.fr       */
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

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const;
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
