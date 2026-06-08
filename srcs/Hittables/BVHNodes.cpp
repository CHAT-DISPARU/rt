/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVHNodes.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 18:09:10 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/08 12:24:36 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BVHNode.hpp"

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects)
{
	if (objects.empty())
		return;

	// calcul centre
	std::vector<MortonPrimitive>	mortonPrims(objects.size());
	AABB							globalBox;

	for (size_t i = 0; i < objects.size(); i++)
	{
		AABB	box;
		objects[i]->bbox(box);
		Vec3f centroid = (box._min + box._max) * 0.5f;

		globalBox = AABB::AABB_union(globalBox, AABB(centroid, centroid));

		mortonPrims[i].primitiveIndex = (uint32_t)i;
		mortonPrims[i].mortonCode = 0;
	}

	// calce morton
	for (size_t i = 0; i < objects.size(); i++)
	{
		AABB	box;
		objects[i]->bbox(box);
		Vec3f	centroid = (box._min + box._max) * 0.5f;
		mortonPrims[i].mortonCode = computeMortonCode(centroid, globalBox);
	}

	// sort morton
	radixSort(mortonPrims);

	//contruction arbre
	buildTreeFromMorton(mortonPrims, objects);
}

// etale sur 63 bit
inline uint64_t	splitBy3(unsigned int a)
{
	uint64_t x = a & 0x1fffff; // we only look at the first 21 bits
	x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
	x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
	x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
	x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
	x = (x | x << 2) & 0x1249249249249249;
	return (x);
}
//https://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/
uint64_t	BVHNode::computeMortonCode(const Vec3f& centroid, const AABB& globalBox) const
{
	Vec3f extent = globalBox._max - globalBox._min;
	Vec3f normalized = (centroid - globalBox._min) / extent;

	//evite les debordmant entre 0 et 1
	normalized._x = std::fmin(std::fmax(normalized._x, 0.0f), 1.0f);
	normalized._y = std::fmin(std::fmax(normalized._y, 0.0f), 1.0f);
	normalized._z = std::fmin(std::fmax(normalized._z, 0.0f), 1.0f);

	// c onvertit en 21 bits [0, * 2097151]
	uint32_t ix = (uint32_t)(normalized._x* 2097151.0f);
	uint32_t iy = (uint32_t)(normalized._y* 2097151.0f);
	uint32_t iz = (uint32_t)(normalized._z* 2097151.0f);

	//entrelace les bits x=bit0 y=bit1 z=bit2
	//splitBy3(ix) → x sur position 0, 3, 6, 9, 12 
	//splitBy3(iy) << 1 → y sur poitions 1, 4, 7, 10, 13 
	//splitBy3(iz) << 2 → z sur positions 2, 5, 8, 11, 14
	return (splitBy3(ix) | splitBy3(iy) << 1 | splitBy3(iz) << 2);
}
