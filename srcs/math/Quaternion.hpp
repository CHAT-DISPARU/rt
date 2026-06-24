/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quaternion.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 19:16:51 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/23 19:18:53 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include <cmath>

template <typename T>
struct Quaternion
{
	T	_w, _x, _y, _z;

	Quaternion() : _w(T(1)), _x(T(0)), _y(T(0)), _z(T(0)) {};
	Quaternion(T w, T x, T y, T z) : _w(w), _x(x), _y(y), _z(z) {};
	Quaternion(const Quaternion& other) : _w(other._w), _x(other._x), _y(other._y), _z(other._z) {};

	Quaternion&	operator=(const Quaternion& other)
	{
		_w = other._w;
		_x = other._x;
		_y = other._y;
		_z = other._z;
		return (*this);
	}

	// creation avec angle radiant et axe
	static Quaternion	fromAxisAngle(T angle, const Vec3<T>& axis)
	{
		T	half_angle = angle * T(0.5);
		T	s = std::sin(half_angle);
		Vec3<T>	n = Vec3<T>::normalize(axis);

		return (Quaternion(std::cos(half_angle), n._x * s, n._y * s, n._z * s));
	}

	Quaternion	operator*(const Quaternion& q) const
	{
		return (Quaternion(
			_w * q._w - _x * q._x - _y * q._y - _z * q._z,
			_w * q._x + _x * q._w + _y * q._z - _z * q._y,
			_w * q._y - _x * q._z + _y * q._w + _z * q._x,
			_w * q._z + _x * q._y - _y * q._x + _z * q._w
		));
	}

	Quaternion&	operator*=(const Quaternion& q)
	{
		*this = *this * q;
		return (*this);
	}

	// rot vec
	Vec3<T>	operator*(const Vec3<T>& v) const
	{
		Vec3<T>	q_vec(_x, _y, _z);
		Vec3<T>	t = T(2) * Vec3<T>::cross(q_vec, v);
		return (v + _w * t + Vec3<T>::cross(q_vec, t));
	}

	T	length() const
	{
		return (std::sqrt(_w * _w + _x * _x + _y * _y + _z * _z));
	}

	Quaternion&	normalize()
	{
		T	len = this->length();
		if (len > T(0))
		{
			T	inv_len = T(1) / len;
			_w *= inv_len;
			_x *= inv_len;
			_y *= inv_len;
			_z *= inv_len;
		}
		return (*this);
	}

	Quaternion	conjugate() const
	{
		return (Quaternion(_w, -_x, -_y, -_z));
	}

	// Interpolation sphérique
	static Quaternion	slerp(const Quaternion& q1, const Quaternion& q2, T t)
	{
		Quaternion	q3 = q2;
		T			dot = q1._w * q2._w + q1._x * q2._x + q1._y * q2._y + q1._z * q2._z;

		if (dot < T(0))
		{
			dot = -dot;
			q3 = Quaternion(-q2._w, -q2._x, -q2._y, -q2._z);
		}

		if (dot > T(0.9995))
		{
			Quaternion	res(
				q1._w + t * (q3._w - q1._w),
				q1._x + t * (q3._x - q1._x),
				q1._y + t * (q3._y - q1._y),
				q1._z + t * (q3._z - q1._z)
			);
			return (res.normalize());
		}

		T	theta_0 = std::acos(dot);
		T	theta = theta_0 * t;
		T	sin_theta = std::sin(theta);
		T	sin_theta_0 = std::sin(theta_0);

		T	s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
		T	s2 = sin_theta / sin_theta_0;

		return (Quaternion(
			s1 * q1._w + s2 * q3._w,
			s1 * q1._x + s2 * q3._x,
			s1 * q1._y + s2 * q3._y,
			s1 * q1._z + s2 * q3._z
		));
	}

	// convertion en mat4
	Mat4<T>	toMat4() const
	{
		Mat4<T>	res;
		T		xx = _x * _x;
		T		yy = _y * _y;
		T		zz = _z * _z;
		T		xy = _x * _y;
		T		xz = _x * _z;
		T		yz = _y * _z;
		T		wx = _w * _x;
		T		wy = _w * _y;
		T		wz = _w * _z;

		res._m[0][0] = T(1) - T(2) * (yy + zz);
		res._m[0][1] = T(2) * (xy - wz);
		res._m[0][2] = T(2) * (xz + wy);
		res._m[1][0] = T(2) * (xy + wz);
		res._m[1][1] = T(1) - T(2) * (xx + zz);
		res._m[1][2] = T(2) * (yz - wx);
		res._m[2][0] = T(2) * (xz - wy);
		res._m[2][1] = T(2) * (yz + wx);
		res._m[2][2] = T(1) - T(2) * (xx + yy);
		return (res);
	}
};

typedef Quaternion<float>	Quatf;
