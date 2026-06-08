/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 16:17:25 by gajanvie          #+#    #+#             */
/*   Updated: 2026/06/08 18:07:07 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BVHNode.hpp"
#include "Sphere.hpp"
#include "Lambertian.hpp"
#include "Math3D.hpp"
#include <chrono>
#include <iostream>

int	main()
{
	std::vector<std::shared_ptr<Hittable>> objects;
	srand(time(NULL));
	unsigned int seed = rand();
	float	r, r1, r2;
	std::cout << "start" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < 6464000; i++)
	{
		r = rand_r(&seed) % 1000;
		r1 = rand_r(&seed) % 1000;
		r2 = rand_r(&seed) % 1000;
		auto s = std::make_shared<Sphere>(1, Vec3f(r, r1, r2), Mat4f::translate(Vec3f(r, r1, r2)), new Lambertian);
		objects.push_back(s);
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "construction time: " << duration.count() << " ms" << std::endl;
	std::cout << "BVH start" << std::endl;
	auto startbvh = std::chrono::high_resolution_clock::now();
	BVHNode bvh(objects);
	auto endbvh = std::chrono::high_resolution_clock::now();
	auto durationbvh = std::chrono::duration_cast<std::chrono::milliseconds>(endbvh - startbvh);
	std::cout << "BVH construction time: " << durationbvh.count() << " ms" << std::endl;
}
