/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sunlight.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 22:44:27 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/12 10:23:52 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// SunLight.hpp
#pragma once
#include "Math3D.hpp"

struct	SunLight
{
	Vec3f	direction;
	Vec3f	color;
	Vec3f	glow_color;
	float	intensity;
	bool	enabled;
	float	glow_intensity;
	float	size;
	float	glow_size;
};
