/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVHNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 16:30:47 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/08 12:20:55 by CHAT-DISPAR      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "Hittable.hpp"
#include "AABB.hpp"

class	BVHNode : public Hittable
{
	public:
		struct alignas(16)	Node
		{
			AABB		bbox;
			uint32_t	leftChildOrPrimOffset; // index gauche (droit = gauche + 1) ou si fueille index tableau
			uint32_t	primitiveCount; //0 si noeud interne si feuille nb obj
			uint32_t	axis; // axe de separation choisi
		};

		struct	MortonPrimitive
		{
			uint32_t	primitiveIndex; // index obj
			uint64_t	mortonCode; // code de Morton 32 bits 
		};

		BVHNode(std::vector<std::shared_ptr<Hittable>>& objects);

		bool	hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const;
		bool	bbox(AABB& output_box) const;

		const std::vector<Node>& getLinearNodes() const
		{
			return _nodes;
		};

	private:
		std::vector<Node>						_nodes;
		std::vector<std::shared_ptr<Hittable>>	_orderedObjects;

		// 1 calc le morton
		uint64_t	computeMortonCode(const Vec3f& centroid, const AABB& globalBox) const;

		// 2 radix sort
		void	radixSort(std::vector<MortonPrimitive>& prims);

		// 3 : Organise les primitives triées en clusters et génère la topologie globale de l'arbre
		void	buildTreeFromMorton(std::vector<MortonPrimitive>& mortonPrims, 
									 const std::vector<std::shared_ptr<Hittable>>& srcObjects);

		// Étape 4 : Routine ultra-rapide pour construire les sous-arbres (LBVH) par masquage de bits
		int	buildLocalLBVH(std::vector<MortonPrimitive>& mortonPrims, 
								const std::vector<std::shared_ptr<Hittable>>& srcObjects,
								size_t start, size_t end, int bitShift);
};