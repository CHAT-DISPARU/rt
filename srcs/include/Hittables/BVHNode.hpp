/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVHNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gajanvie <gajanvie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 16:30:47 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/12 16:48:26 by gajanvie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "Hittable.hpp"
#include "AABB.hpp"
#define MAX_BIN 16
#define SAH_MIN_OBJS 4


class	BVHNode : public Hittable
{
	public:
		struct	Cluster
		{
			size_t	start;//inde x morton
			size_t	end;
			AABB	bbox;   // bbox du cluster
		};

		struct	Node
		{
			AABB		bbox;
			uint32_t	leftChildOrPrimOffset; // index gauche  ou si fueille index tableau
			uint32_t	rightChildOffset;
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
		bool	hit_shadow(const Ray& ray, float tMin, float light_dist, HitRecord& rec) const;
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

		// 3 organise les primitive avec le sah en cluster
		void	buildTreeFromMorton(std::vector<MortonPrimitive>& mortonPrims, 
									 const std::vector<std::shared_ptr<Hittable>>& srcObjects);
		//sah haut de l arbre donc pour les cluster
		int	buildSAHTopLevel(std::vector<Cluster>& clusters, std::vector<MortonPrimitive>& mortonPrims, const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end);
		// 4 sous arbre avec les bits
		int	buildLocalLBVH(std::vector<MortonPrimitive>& mortonPrims, 
								const std::vector<std::shared_ptr<Hittable>>& srcObjects,
								size_t start, size_t end, int bitShift);
};

/*
	explication construction


	1 cumpute morton entrlacement de bit pour cree uint64

	trie radix


	cree un mask sur les 12 premier bit donc 111111111111et le reste de 0
	2puissance 12 = 4096 cluster possible ratio plutot ok ...
	cree une cluster par mask different faire la bvh en lbvh dans le cluster
	ensuite sah jusqu a avoir un cluster solo on l envoie dans la lbvh 
	qui fait des cpoupe selon le dernier bit des mortons trie ....
 
*/
