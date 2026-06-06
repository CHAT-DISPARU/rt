/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mat4.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:47:34 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 18:02:06 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iomanip>
#include <cmath>

template <typename T>
struct Mat4
{
	T _m[4][4];

	Mat4()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					_m[i][j] = T(1);
				else
					_m[i][j] = T(0);
			}
		}
	};

	Mat4(T diagonal)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					_m[i][j] = diagonal;
				else
					_m[i][j] = T(0);
			}
		}
	};

	Mat4(const Mat4 &other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] = other._m[i][j];
		}
	};

	Mat4	&operator=(const Mat4 &other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] = other._m[i][j];
		}
		return (*this);
	};

	Mat4	&operator+=(const Mat4 &other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] = _m[i][j] + other._m[i][j];
		}
		return (*this);
	};

	Mat4	&operator-=(const Mat4 &other)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] = _m[i][j] - other._m[i][j];
		}
		return (*this);
	};

	Mat4	&operator*=(const Mat4 &other)
	{
		Mat4	copy(*this);
	
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				_m[i][j] = T(0);
				for (int k = 0; k < 4; k++)
					_m[i][j] += copy._m[i][k] * other._m[k][j]; 
			}
		}
		return (*this);
	};

	Mat4	&operator*=(T t)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] *= t;
		}
		return (*this);
	};

	Mat4	&operator/=(T t)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				_m[i][j] /= t;
		}
		return (*this);
	};

	Mat4	operator+(const Mat4 &other) const
	{
		Mat4	result(*this);
		result += other;
		return (result);
	};

	Mat4	operator-(const Mat4 &other) const
	{
		Mat4	result(*this);
		result -= other;
		return (result);
	};

	Mat4	operator*(const Mat4 &other) const
	{
		Mat4	result(*this);
		result *= other;
		return (result);
	};

	Mat4	operator*(T t) const
	{
		Mat4	result(*this);
		result *= t;
		return (result);
	};

	Mat4	operator/(T t) const
	{
		Mat4	result(*this);
		result /= t;
		return (result);
	};

	Mat4	transpose() const
	{
		Mat4	res;

		for (int i = 0; i< 4; i++)
		{
			for (int j = 0; j< 4; j++)
			{
				res._m[i][j] = _m[j][i];
			}
		}
		return (res);
	};

	static T	det3x3(const T m[4][4], int c1, int c2, int c3)
	{
		return (m[1][c1] * (m[2][c2] * m[3][c3] - m[3][c2] * m[2][c3])
			- m[2][c1] * (m[1][c2] * m[3][c3] - m[3][c2] * m[1][c3])
			+ m[3][c1] * (m[1][c2] * m[2][c3] - m[2][c2] * m[1][c3]));
	};

	T	determinant() const
	{
		return (_m[0][0] * det3x3(_m, 1, 2, 3)
			- _m[0][1] * det3x3(_m, 0, 2, 3)
			+ _m[0][2] * det3x3(_m, 0, 1, 3)
			- _m[0][3] * det3x3(_m, 0, 1, 2));
	};

	static T	get_minor(Mat4 m, int skip_r, int skip_c)
	{
		int	r[3];
		int	c[3];
		int	i;
		int	j = 0;
	
		for (i = 0; i < 4; i++)
		{
			if (i != skip_r)
				r[j++] = i;
		}
		j = 0;
		i = 0;
		for (i = 0; i < 4; i++)
		{
			if (i != skip_c)
				c[j++] = i;
		}
		return (m._m[r[0]][c[0]] * (m._m[r[1]][c[1]] * m._m[r[2]][c[2]]
			- m._m[r[2]][c[1]] * m._m[r[1]][c[2]]) - m._m[r[0]][c[1]]
			* (m._m[r[1]][c[0]] * m._m[r[2]][c[2]] - m._m[r[2]][c[0]]
			* m._m[r[1]][c[2]]) + m._m[r[0]][c[2]] * (m._m[r[1]][c[0]]
			* m._m[r[2]][c[1]] - m._m[r[2]][c[0]] * m._m[r[1]][c[1]]));
	}

	Mat4	inverse() const
	{
		Mat4	res;
		T		det;
		T		inv_det;

		det = this->determinant();
		if (det == T(0))
			return (Mat4(T(0)));
		inv_det = T(1) / det;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				res._m[j][i] = get_minor(*this, i, j) * inv_det;
				if ((i + j) % 2 != 0)
					res._m[j][i] = -res._m[j][i];
					
			}
		}
		return (res);
	};

	const T	*operator[](int i) const
	{
		return _m[i];
	};

	T	*operator[](int i)
	{
		return _m[i];
	};

	T&	operator()(int r, int c)
	{
		return _m[r][c];
	};

	static Mat4	translate(const Vec3<T> &v)
	{
		Mat4	res;

		res._m[0][3] = v._x;
		res._m[1][3] = v._y;
		res._m[2][3] = v._z;
		return (res);
	};

	static Mat4	scale(const Vec3<T> &v)
	{
		Mat4	res;

		res._m[0][0] = v._x;
		res._m[1][1] = v._y;
		res._m[2][2] = v._z;
		return (res);
	};

	static Mat4	rotateX(T angle)
	{
		Mat4	res;
		T		c = std::cos(angle);
		T		s = std::sin(angle);

		res._m[1][1] = c;  res._m[1][2] = -s;
		res._m[2][1] = s;  res._m[2][2] = c;
		return res;
	}

	static Mat4	rotateY(T angle)
	{
		Mat4	res;
		T		c = std::cos(angle);
		T		s = std::sin(angle);

		res._m[0][0] = c;   res._m[0][2] = s;
		res._m[2][0] = -s;  res._m[2][2] = c;
		return res;
	}

	static Mat4	rotateZ(T angle)
	{
		Mat4	res;
		T		c = std::cos(angle);
		T		s = std::sin(angle);

		res._m[0][0] = c;  res._m[0][1] = -s;
		res._m[1][0] = s;  res._m[1][1] = c;
		return res;
	}

	static Mat4	rotate(T angle, const Vec3<T> &axis)
	{
		Mat4	res;

		T	c = std::cos(angle);
		T	s = std::sin(angle);
		T	t = T(1) - c;
		Vec3<T> n = Vec3<T>::normalize(axis);
		T x = n._x;
		T y = n._y;
		T z = n._z;

		res._m[0][0] = t * x * x + c;
		res._m[0][1] = t * x * y - s * z;
		res._m[0][2] = t * x * z + s * y;

		res._m[1][0] = t * x * y + s * z;
		res._m[1][1] = t * y * y + c;
		res._m[1][2] = t * y * z - s * x;

		res._m[2][0] = t * x * z - s * y;
		res._m[2][1] = t * y * z + s * x;
		res._m[2][2] = t * z * z + c;

		return res;
	}

	static Mat4	lookAt(const Vec3<T> &eye, const Vec3<T> &center, const Vec3<T> &up)
	{
		Vec3<T>	f = (center - eye).normalize();
		Vec3<T>	s = Vec3<T>::cross(f, up).normalize();
		Vec3<T>	u = Vec3<T>::cross(s, f);

		Mat4 res;

		res._m[0][0] = s._x;
		res._m[0][1] = s._y;
		res._m[0][2] = s._z;

		res._m[1][0] = u._x;
		res._m[1][1] = u._y;
		res._m[1][2] = u._z;

		res._m[2][0] = -f._x;
		res._m[2][1] = -f._y;
		res._m[2][2] = -f._z;

		res._m[0][3] = -Vec3<T>::dot(s, eye);
		res._m[1][3] = -Vec3<T>::dot(u, eye);
		res._m[2][3] = -Vec3<T>::dot(f, eye);

		return (res);
	}

	static Mat4	perspective(T fov_radians, T aspect, T near, T far)
	{
		Mat4	res(T(0));

		T	tanHalfFovy = std::tan(fov_radians / T(2));
		res._m[0][0] = T(1) / (aspect * tanHalfFovy);
		res._m[1][1] = T(1) / (tanHalfFovy);
		res._m[2][2] = -(far + near) / (far - near);
		res._m[3][2] = T(-1);
		res._m[2][3] = -(T(2) * far * near) / (far - near);
		return res;
	}
};

template <typename T>
Mat4<T>	operator*(T t, const Mat4<T> &m)
{
	Mat4<T>	result(m);
	result *= t;
	return (result);
};

template <typename T>
std::ostream	&operator<<(std::ostream &out, const Mat4<T> &m)
{
	out << std::fixed << std::setprecision(4);
	out << "------------------------------------------\n";
	for (int i = 0; i < 4; i++)
	{
		out << "| ";
		for (int j = 0; j < 4; j++)
			out << std::setw(6) << m._m[i][j] << " | ";
		out << "\n";
	}
	out << "------------------------------------------";
	return out;
};

typedef	Mat4<float>		Mat4f;
typedef	Mat4<double>	Mat4d;
