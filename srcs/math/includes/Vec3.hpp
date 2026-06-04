/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec3.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 15:25:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/04 15:31:53 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Mat4.hpp"
#include <iostream>
#include <math.h>

struct Vec3
{
	float _x, _y, _z;

	Vec3();
	Vec3(float scalar) : _x(scalar), _y(scalar), _z(scalar){};
	Vec3(float x, float y, float z) : _x(x), _y(y), _z(z){};
	Vec3(const Vec3& other)
	{
		_x = other._x;
		_y = other._y;
		_z = other._z;
	};

	Vec3&	operator=(const Vec3& other)
	{
		_x = other._x;
		_y = other._y;
		_z = other._z;
		return (*this);
	};
	Vec3&	operator+=(const Vec3& v)
	{
		_x += v._x;
		_y += v._y;
		_z += v._z;
		return (*this);
	};
	Vec3&	operator-=(const Vec3& v)
	{
		_x -= v._x;
		_y -= v._y;
		_z -= v._z;
		return (*this);
	};
	Vec3&	operator*=(const Vec3& v)
	{
		_x *= v._x;
		_y *= v._y;
		_z *= v._z;
		return (*this);
	};
	Vec3&	operator*=(float t)
	{
		_x *= t;
		_y *= t;
		_z *= t;
		return (*this);
	};
	Vec3&	operator/=(float t)
	{
		_x /= t;
		_y /= t;
		_z /= t;
		return (*this);
	};

	Vec3	operator-() const
	{
		Vec3	tmp(*this);
		tmp._x = -_x;
		tmp._y = -_y;
		tmp._z = -_z;
		return (tmp);
	};

	Vec3	operator+(const Vec3& v) const
	{
		Vec3	tmp(*this);
		tmp._x = _x + v._x;
		tmp._y = _y + v._y;
		tmp._z = _z + v._z;
		return (tmp);
	};
	Vec3	operator-(const Vec3& v) const
	{
		Vec3	tmp(*this);
		tmp._x = _x - v._x;
		tmp._y = _y - v._y;
		tmp._z = _z - v._z;
		return (tmp);
	};
	Vec3	operator*(const Vec3& v) const
	{
		Vec3	tmp(*this);
		tmp._x = _x * v._x;
		tmp._y = _y * v._y;
		tmp._z = _z * v._z;
		return (tmp);
	};
	
	Vec3	operator*(float t) const
	{
		Vec3	tmp(*this);
		tmp._x = _x * t;
		tmp._y = _y * t;
		tmp._z = _z * t;
		return (tmp);
	};
	Vec3	operator/(float t) const
	{
		Vec3	tmp(*this);
		tmp._x = _x / t;
		tmp._y = _y / t;
		tmp._z = _z / t;
		return (tmp);
	};

	Vec3	operator*(const Mat4& m) const
	{
		Vec3	res;
		res._x = _x * m._m[0][0]
			+ _y * m._m[0][1] + _z * m._m[0][2];
		res._y = _x * m._m[1][0]
			+ _y * m._m[1][1] + _z * m._m[1][2];
		res._z = _x * m._m[2][0]
			+ _y * m._m[2][1] + _z * m._m[2][2];
		return (res);
	};

	float	length() const
	{
		return (sqrt(_x * _x + _y * _y + _z * _z));
	};

	float	length_sq() const
	{
		return (_x * _x + _y * _y + _z * _z);
	};

	Vec3&	normalize()
	{
		float	len;
		float	inv_len;

		len = this->length();
		if (len == 0.0)
		{
			_x = 0;
			_y = 0;
			_z = 0;
			return (*this);
		}
		inv_len = 1.0 / len;
		_x = _x * inv_len;
		_y = _y * inv_len;
		_z = _z * inv_len;
		return (*this);
	};
	
	float	operator[](int i) const
	{
		if (i == 0)
			return (_x);
		else if (i == 1)
			return (_y);
		return (_z);
	};
	float&	operator[](int i)
	{
		if (i == 0)
			return (_x);
		else if (i == 1)
			return (_y);
		return (_z);
	};
};

Vec3	operator*(float t, const Vec3& v)
{
	Vec3	tmp(v);
	tmp._x = v._x * t;
	tmp._y = v._y * t;
	tmp._z = v._z * t;
	return (tmp);
};
float	dot(const Vec3& u, const Vec3& v)
{
	return (u._x * v._x + u._y * v._y + u._z * v._z);
};
Vec3	cross(const Vec3& u, const Vec3& v)
{
	Vec3	res;

	res._x = u._y * v._z - u._z * v._y;
	res._y = u._z * v._x - u._x * v._z;
	res._z = u._x * v._y - u._y * v._x;
	return (res);
};
Vec3	normalize(Vec3 v)
{
	float	len;
	float	inv_len;
	Vec3	res(0);

	len = v.length();
	if (len == 0.0)
		return (res);
	inv_len = 1.0 / len;
	res._x = v._x * inv_len;
	res._y = v._y * inv_len;
	res._z = v._z * inv_len;
	return (res);
};
Vec3	reflect(const Vec3& v, const Vec3& n, const float fuzz)
{
};
Vec3	refract(const Vec3& uv, const Vec3& n, float etai_over_etat)
{
};
Vec3	Point_Mult_mat4(const Vec3& v, const Mat4& m)
{
	Vec3	res;

	res._x = v._x * m._m[0][0]
		+ v._y * m._m[0][1] + v._z * m._m[0][2] + m._m[0][3];
	res._y = v._x * m._m[1][0]
		+ v._y * m._m[1][1] + v._z * m._m[1][2] + m._m[1][3];
	res._z = v._x * m._m[2][0]
		+ v._y * m._m[2][1] + v._z * m._m[2][2] + m._m[2][3];
	return (res);
};

std::ostream&	operator<<(std::ostream& out, const Vec3& v)
{
	out << "--------\n  Vec3  \n x = " << v._x << "\n y = " << v._y << "\n z = " << v._z << "\n--------\n";
	return (out);
};