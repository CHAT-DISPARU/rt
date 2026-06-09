/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 16:17:25 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/09 12:32:06 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BVHNode.hpp"
#include "Sphere.hpp"
#include "Lambertian.hpp"
#include "Math3D.hpp"
#include <chrono>
#include <iostream>

int	main(int ac, char **av)
{
	std::vector<std::shared_ptr<Hittable>> objects;
	if (ac != 2)
		return (1);
	
	parse_file(av[1], objects);
	srand(time(NULL));
	unsigned int seed = rand();
	std::cout << "BVH start" << std::endl;
	auto startbvh = std::chrono::high_resolution_clock::now();
	BVHNode bvh(objects);
	auto endbvh = std::chrono::high_resolution_clock::now();
	auto durationbvh = std::chrono::duration_cast<std::chrono::milliseconds>(endbvh - startbvh);
	std::cout << "BVH construction time: " << durationbvh.count() << " ms" << std::endl;


	return (0);
}
