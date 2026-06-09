/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:54 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 11:26:32 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Hittable.hpp"
#include "Scene.hpp"

struct Render
{
	size_t			start_y;
	size_t			end_y;
	size_t			start_x;
	size_t			end_x;
	Camera			&cam;
	Scene			&scene;
	bool			samples;
	float			inv_w;
	float			inv_h;
	unsigned int	*seed;
};
