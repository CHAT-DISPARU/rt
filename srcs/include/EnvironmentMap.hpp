/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EnvironmentMap.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 13:13:33 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/21 08:58:03 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
 
#include "Math3D.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
 
#include "stb_image.h"
 

class EnvironmentMap
{
	public:
		EnvironmentMap()
		{
			_data = nullptr;
			_width = 0;
			_height = 0;
			_channels = 0;
			_intensity = 1.0f;
			_rotation_y = 0.0f;
		}
	
		~EnvironmentMap()
		{
			if (_data)
				stbi_image_free(_data);
		}
		EnvironmentMap(const EnvironmentMap&) = delete;
		EnvironmentMap& operator=(const EnvironmentMap&) = delete;
	
		EnvironmentMap(EnvironmentMap&& other) noexcept
		{
			_data = other._data;
			_width = other._width;
			_height = other._height;
			_channels = other._channels;
			_intensity = other._intensity;
			_rotation_y = other._rotation_y;
			other._data = nullptr;
		}
	
		EnvironmentMap& operator=(EnvironmentMap&& other) noexcept
		{
			if (this != &other)
			{
				if (_data)
					stbi_image_free(_data);
				_data = other._data;
				_width = other._width;
				_height = other._height;
				_channels = other._channels;
				_intensity = other._intensity;
				_rotation_y = other._rotation_y;
				other._data = nullptr;
			}
			return (*this);
		}
	
		bool	load(const std::string& path)
		{
			if (_data)
			{
				stbi_image_free(_data);
				_data = nullptr;
			}
			_data = stbi_loadf(path.c_str(), &_width, &_height, &_channels, 3);
	
			if (!_data)
			{
				std::cerr << "Error: impossible de charger la HDRI " << path << std::endl;
				return (false);
			}
	
			std::cout << "HDRI chargee: " << path << " (" << _width << "x" << _height << ")" << std::endl;
			return (true);
		}
	
		bool	isLoaded() const
		{
			return (_data != nullptr);
		}
	
		void	setIntensity(float intensity)
		{
			_intensity = intensity;
		}
	
		void	setRotation(float radians)
		{
			_rotation_y = radians;
		}

		Vec3f	sample(const Vec3f& dir) const
		{
			if (!_data)
				return (Vec3f(0.0f));
	
			Vec3f	d = Vec3f::normalize(dir);
			if (_rotation_y != 0.0f)
			{
				float	cosA = std::cos(_rotation_y);
				float	sinA = std::sin(_rotation_y);
				float	x = d._x * cosA - d._z * sinA;
				float	z = d._x * sinA + d._z * cosA;
				d._x = x;
				d._z = z;
			}
			float	u = 0.5f + std::atan2(d._z, d._x) / (2.0f * (float)M_PI);
			float	v = 0.5f - std::asin(std::fmax(-1.0f, std::fmin(1.0f, d._y))) / (float)M_PI;
	
			return (bilinearSample(u, v) * _intensity);
		}
	
	private:
		float	*_data;
		int		_width;
		int		_height;
		int		_channels;
		float	_intensity;
		float	_rotation_y;
	
		Vec3f	texelAt(int x, int y) const
		{
			x = ((x % _width) + _width) % _width;
			y = std::min(std::max(y, 0), _height - 1);
	
			int	idx = (y * _width + x) * 3;
			return (Vec3f(_data[idx], _data[idx + 1], _data[idx + 2]));
		}
	
		Vec3f	bilinearSample(float u, float v) const
		{
			float	fx = u * (float)_width - 0.5f;
			float	fy = v * (float)_height - 0.5f;
	
			int		x0 = (int)std::floor(fx);
			int		y0 = (int)std::floor(fy);
			float	tx = fx - (float)x0;
			float	ty = fy - (float)y0;
	
			Vec3f	c00 = texelAt(x0, y0);
			Vec3f	c10 = texelAt(x0 + 1, y0);
			Vec3f	c01 = texelAt(x0, y0 + 1);
			Vec3f	c11 = texelAt(x0 + 1, y0 + 1);
	
			Vec3f	top = c00 * (1.0f - tx) + c10 * tx;
			Vec3f	bottom = c01 * (1.0f - tx) + c11 * tx;
	
			return (top * (1.0f - ty) + bottom * ty);
		}
};
