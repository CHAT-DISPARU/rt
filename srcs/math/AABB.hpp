/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AABB.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 21:27:48 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/06 17:48:52 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include "Ray.hpp"

struct AABB
{
	Vec3f	_min;
	Vec3f	_max;

	AABB() : _min(Vec3f(FLT_MAX)), _max(Vec3f(FLT_MAX)) {};
	AABB(Vec3f min, Vec3f max) : _min(min), _max(max) {};

	AABB	&operator*=(const Mat4f &m)
	{
		*this = *this * m;
		return (*this);
	}
	AABB	operator*(const Mat4f &m)
	{
		Vec3f	corners[8];
		AABB	world_box;
		Vec3f	point;		

		corners[0] = Vec3f(_min._x, _min._y, _min._z);
		corners[1] = Vec3f(_min._x, _min._y, _max._z);
		corners[2] = Vec3f(_min._x, _max._y, _min._z);
		corners[3] = Vec3f(_min._x, _max._y, _max._z);
		corners[4] = Vec3f(_max._x, _min._y, _min._z);
		corners[5] = Vec3f(_max._x, _min._y, _max._z);
		corners[6] = Vec3f(_max._x, _max._y, _min._z);
		corners[7] = Vec3f(_max._x, _max._y, _max._z);
		
		for (int i = 0; i < 8; i++)
		{
			point *= m;
			world_box._min = Vec3f::vec_min(world_box._min, point);
			world_box._max = Vec3f::vec_max(world_box._max, point);
		}
		return (world_box);
	}

	AABB	&add_point(const Vec3f &p)
	{
		_min = Vec3f::vec_min(_min, p);
		_max = Vec3f::vec_min(_max, p);
	}

	static AABB	AABB_union(const AABB a, const AABB b)
	{
		AABB	res;

		res._min._x = fminf(a._min._x, b._min._x);
		res._min._y = fminf(a._min._y, b._min._y);
		res._min._z = fminf(a._min._z, b._min._z);
		res._max._x = fmaxf(a._max._x, b._max._x);
		res._max._y = fmaxf(a._max._y, b._max._y);
		res._max._z = fmaxf(a._max._z, b._max._z);
		return (res);
	};
	
	float	size()
	{
		Vec3f	size;

		size = _max - _min;
		return (2.0 * ((size._x * size._y) + (size._y * size._z) + (size._z * size._x)));
	};

	bool	hit(const Ray& ray, float tMin, float tMax) const
	{
		for (int a = 0; a < 3; a++)
		{
			float	invD = 1.0f / ray._dir[a];
			float	t0 = (_min[a] - ray._o[a]) * invD;
			float	t1 = (_max[a] - ray._o[a]) * invD;
			
			if (invD < 0.0f)
				std::swap(t0, t1);
			tMin = t0 > tMin ? t0 : tMin;
			tMax = t1 < tMax ? t1 : tMax;
			if (tMax <= tMin)
				return (false);
		}
		return (true);
	}
};