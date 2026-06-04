/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec3.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 15:25:12 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/04 21:15:36 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cmath>

template <typename T>
struct Vec3
{
	T	_x, _y, _z;

	Vec3() : _x(T(0)), _y(T(0)), _z(T(0)) {};
	Vec3(T scalar) : _x(scalar), _y(scalar), _z(scalar) {};
	Vec3(T x, T y, T z) : _x(x), _y(y), _z(z) {};
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
	Vec3&	operator*=(T t)
	{
		_x *= t;
		_y *= t;
		_z *= t;
		return (*this);
	};
	Vec3&	operator/=(T t)
	{
		_x /= t;
		_y /= t;
		_z /= t;
		return (*this);
	};

	Vec3	operator-() const
	{
		return Vec3(-_x, -_y, -_z);
	};

	Vec3	operator+(const Vec3& v) const
	{
		return Vec3(_x + v._x, _y + v._y, _z + v._z);
	}
	Vec3	operator-(const Vec3& v) const
	{
		return Vec3(_x - v._x, _y - v._y, _z - v._z);
	};
	Vec3	operator*(const Vec3& v) const
	{
		return Vec3(_x * v._x, _y * v._y, _z * v._z);
	};
	
	Vec3	operator*(T t) const
	{
		return Vec3(_x * t, _y * t, _z * t);
	};
	Vec3	operator/(T t) const
	{
		return Vec3(_x / t, _y / t, _z / t);
	};

	Vec3	operator*(const Mat4<T>& m) const
	{
		Vec3	res;

		res._x = _x * m._m[0][0] + _y * m._m[0][1] + _z * m._m[0][2];
		res._y = _x * m._m[1][0] + _y * m._m[1][1] + _z * m._m[1][2];
		res._z = _x * m._m[2][0] + _y * m._m[2][1] + _z * m._m[2][2];
		return (res);
	};

	T	length() const
	{
		return (std::sqrt(_x * _x + _y * _y + _z * _z));
	};

	T	length_sq() const
	{
		return (_x * _x + _y * _y + _z * _z);
	};

	Vec3&	normalize()
	{
		T	len = this->length();
		if (len == T(0))
		{
			_x = T(0);
			_y = T(0);
			_z = T(0);
			return (*this);
		}
		T	inv_len = T(1) / len;
		_x *= inv_len;
		_y *= inv_len;
		_z *= inv_len;
		return	(*this);
	};
	
	T	operator[](int i) const
	{
		if (i == 0)
			return (_x);
		else if (i == 1)
			return (_y);
		return (_z);
	};
	T&	operator[](int i)
	{
		if (i == 0)
			return (_x);
		else if (i == 1)
			return (_y);
		return (_z);
	};

	static T	dot(const Vec3& u, const Vec3& v)
	{
		return (u._x * v._x + u._y * v._y + u._z * v._z);
	};
	static Vec3	cross(const Vec3& u, const Vec3& v)
	{
		Vec3	res;
		res._x = u._y * v._z - u._z * v._y;
		res._y = u._z * v._x - u._x * v._z;
		res._z = u._x * v._y - u._y * v._x;
		return (res);
	};
	static Vec3	normalize(Vec3 v)
	{
		T	len = v.length();
		if (len == T(0))
			return Vec3(T(0));
		T	inv_len = T(1) / len;
		return Vec3(v._x * inv_len, v._y * inv_len, v._z * inv_len);
	};
	static Vec3	reflect(const Vec3& v, const Vec3& n)
	{
		return (v - T(2) * dot(v, n) * n);
	};
	static Vec3	refract(const Vec3& uv, const Vec3& n, T ni)
	{
		T		cos_theta = std::fmin(dot(-uv, n), T(1));
		Vec3	r_out_perp = ni * (uv + cos_theta * n);
		Vec3	r_out_parallel = -std::sqrt(std::fabs(T(1) - r_out_perp.length_sq())) * n;
		return (r_out_perp + r_out_parallel);
	}
	static Vec3	Point_Mult_mat4(const Vec3& v, const Mat4<T>& m)
	{
		T	w = v._x * m._m[3][0] + v._y * m._m[3][1] + v._z * m._m[3][2] + m._m[3][3];
		if (w == T(0))
			w = T(1);

		Vec3	res;

		res._x = (v._x * m._m[0][0] + v._y * m._m[0][1] + v._z * m._m[0][2] + m._m[0][3]) / w;
		res._y = (v._x * m._m[1][0] + v._y * m._m[1][1] + v._z * m._m[1][2] + m._m[1][3]) / w;
		res._z = (v._x * m._m[2][0] + v._y * m._m[2][1] + v._z * m._m[2][2] + m._m[2][3]) / w;
		return (res);
	};
};

template <typename T>
Vec3<T>	operator*(T t, const Vec3<T>& v)
{
	return Vec3<T>(v._x * t, v._y * t, v._z * t);
};

template <typename T>
std::ostream&	operator<<(std::ostream& out, const Vec3<T>& v)
{
	out << "--------\n  Vec3  \n x = " << v._x << "\n y = " << v._y << "\n z = " << v._z << "\n--------\n";
	return (out);
};

typedef	Vec3<float>  Vec3f;
typedef	Vec3<double> Vec3d;
typedef	Vec3<int>    Vec3i;
