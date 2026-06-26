/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Metal.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:34:03 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/25 17:34:09 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Material.hpp"

class Metal : public Material
{
	private:
		float	_fuzz;
		
	public:
		~Metal(){};

		bool	scatter(const Ray& r_in, const HitRecord& rec, Vec3f& attenuation, Ray& scattered, float& pdf, unsigned int* seed) const;
		void	setFuzz(float f)
		{
			_fuzz = (f < 1.0f) ? f : 1.0f;
		};
		bool	isSpecular() const
		{
			return true;
		};
		float	getFuzz() const { return _fuzz; }
};
