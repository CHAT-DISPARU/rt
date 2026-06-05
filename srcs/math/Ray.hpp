/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ray.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 21:27:28 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/05 14:35:03 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Math3D.hpp"
#include <iostream>

struct Ray
{
	Vec3f	o;
	Vec3f	dir;

	Vec3f	operator()(float t)
	{
		return ((dir * t) + o);
	};
};
