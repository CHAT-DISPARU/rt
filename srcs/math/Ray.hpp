/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ray.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 21:27:28 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 13:44:41 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include <iostream>

struct Ray
{
	Vec3f	_o;
	Vec3f	_dir;

	Ray() : _o(Vec3f()), _dir(Vec3f()){};
	Ray(Vec3f o, Vec3f dir) : _o(o), _dir(dir){};
	Ray(const Ray& other)
	{
		_o = other._o;
		_dir = other._dir;
	};

	Ray&	operator=(const Ray& other)
	{
		_o = other._o;
		_dir = other._dir;
		return (*this);
	};

	Vec3f	operator()(float t) const
	{
		return ((_dir * t) + _o);
	};

	Ray	&operator*=(const Mat4f &m)
	{
		_o = Vec3f::Point_Mult_mat4(_o, m);
		_dir *= m;
		return (*this);	
	};

	Ray	operator*(const Mat4f &m)
	{
		return (Ray(*this) *= m);	
	};
};
