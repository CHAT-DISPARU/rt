/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVHNodes.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: CHAT-DISPARU <CHAT-DISPARU@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 18:09:10 by CHAT-DISPAR       #+#    #+#             */
/*   Updated: 2026/06/12 23:30:34 by CHAT-DISPAR      ###   ########.fr       */
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
	_nodes.reserve(mortonPrims.size() * 2);
	// sort morton
	radixSort(mortonPrims);
	//contruction arbre
	buildTreeFromMorton(mortonPrims, objects);


	std::cout << "bvh finit nb node :" <<  _nodes.size() << " nb objs :" << _orderedObjects.size() << std::endl;
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

void	BVHNode::radixSort(std::vector<MortonPrimitive>& prims)
{
	const size_t					size = prims.size();
	std::vector<MortonPrimitive>	tmp(size);
	size_t							mIndex[8][256] = { {0} };
	
	MortonPrimitive*	in = prims.data();
	MortonPrimitive*	out = tmp.data();
	uint64_t			u;

	for (size_t i = 0; i < size; ++i)
	{
		u = in[i].mortonCode;
		mIndex[0][(u) & 0xff]++;
		mIndex[1][(u >> 8) & 0xff]++;
		mIndex[2][(u >> 16) & 0xff]++;
		mIndex[3][(u >> 24) & 0xff]++;
		mIndex[4][(u >> 32) & 0xff]++;
		mIndex[5][(u >> 40) & 0xff]++;
		mIndex[6][(u >> 48) & 0xff]++;
		mIndex[7][(u >> 56) & 0xff]++;
	}

	size_t*	sum;
	size_t	m, n;
	for (size_t j = 0; j < 8; ++j)
	{
		sum = mIndex[j];
		n = 0;
		for (int i = 0; i < 256; ++i)
		{
			m = sum[i];
			sum[i] = n;
			n += m;
		}
	}

	for (int pass = 0; pass < 8; ++pass)
	{
		const int	shift = pass * 8;
		sum = mIndex[pass];

		for (size_t i = 0; i < size; ++i)
			out[sum[(in[i].mortonCode >> shift) & 0xff]++] = in[i];
		std::swap(in, out);
	}
}

void	BVHNode::buildTreeFromMorton(std::vector<MortonPrimitive>& mortonPrims,
	const std::vector<std::shared_ptr<Hittable>>& srcObjects)
{
	//12 ser a def les clusters
	const int		mask_bits = 12;
	const uint64_t	mask = (uint64_t)((1 << mask_bits) - 1) << (63 - mask_bits);

	// satrt end
	std::vector<Cluster>	clusters;
	size_t  clusterStart = 0;
	for (size_t i = 1; i <= mortonPrims.size(); i++)
	{
		bool new_cluster = (i == mortonPrims.size()) ||
			((mortonPrims[i].mortonCode & mask) != (mortonPrims[i - 1].mortonCode & mask));
		if (new_cluster)
		{
			Cluster	c;
			c.start = clusterStart;
			c.end = i;
	
			for (size_t j = clusterStart; j < i; j++)
			{
				AABB box;
				srcObjects[mortonPrims[j].primitiveIndex]->bbox(box);
				c.bbox = AABB::AABB_union(c.bbox, box);
			}
			clusters.push_back(c);
			clusterStart = i;
		}
	}
	if (!clusters.empty())
		buildSAHTopLevel(clusters, mortonPrims, srcObjects, 0, clusters.size());
}

int	BVHNode::buildSAHTopLevel(std::vector<Cluster>& clusters, std::vector<MortonPrimitive>& mortonPrims,
	const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end)
{
	size_t	span = end - start;

	// arret si cluster tout seul -> lbvh
	if (span == 1)
		// bitshift 63 - 12 (mask_bits) - 1 = 50
		return (buildLocalLBVH(mortonPrims, srcObjects, clusters[start].start, clusters[start].end, 50));

	AABB	bounds, centroidBounds;
	for (size_t i = start; i < end; i++)
	{
		bounds = AABB::AABB_union(bounds, clusters[i].bbox);
		Vec3f centroid = (clusters[i].bbox._min + clusters[i].bbox._max) * 0.5f;
		centroidBounds = AABB::AABB_union(centroidBounds, AABB(centroid, centroid));
	}

	const int	BINS = std::min(MAX_BIN, (int)span);
	float		bestCost = FLT_MAX;
	int			bestSplit = -1;
	int			bestAxis = 0;

	for (int axis = 0; axis < 3; axis++)
	{
		float	extent = centroidBounds._max[axis] - centroidBounds._min[axis];

		// /0 portection et si axe hyperfin
		if (extent < 1e-6f)
			continue; 

		struct	Bin
		{
			AABB	bbox;
			int		count = 0;
		};
		Bin	bins[MAX_BIN];
		float	scale = BINS / extent;

		// rempli bin
		for (size_t i = start; i < end; i++)
		{
			Vec3f	c = (clusters[i].bbox._min + clusters[i].bbox._max) * 0.5f;
			int		b = std::min(BINS - 1, (int)((c[axis] - centroidBounds._min[axis]) * scale));
			
			bins[b].count++;
			bins[b].bbox = AABB::AABB_union(bins[b].bbox, clusters[i].bbox);
		}

		// sah sur axe
		for (int i = 0; i < BINS - 1; i++)
		{
			AABB	leftBox, rightBox;
			int	leftCount = 0;
			int	rightCount = 0;

			for (int j = 0; j <= i; j++)
			{
				leftBox = AABB::AABB_union(leftBox, bins[j].bbox);
				leftCount += bins[j].count;
			}
			for (int j = i + 1; j < BINS; j++) {
				rightBox = AABB::AABB_union(rightBox, bins[j].bbox);
				rightCount += bins[j].count;
			}

			if (leftCount == 0 || rightCount == 0)
				continue;

			float	cost = leftBox.size() * leftCount + rightBox.size() * rightCount;
			if (cost < bestCost)
			{
				bestCost = cost;
				bestSplit = i;
				bestAxis = axis;
			}
		}
	}

	// separ cluster slon split
	float	bestExtent = centroidBounds._max[bestAxis] - centroidBounds._min[bestAxis];
	float	bestScale = BINS / bestExtent;
	
	
	auto mid = std::partition(clusters.begin() + start, clusters.begin() + end,
		[&](const Cluster& cl)
		{
			Vec3f	c = (cl.bbox._min + cl.bbox._max) * 0.5f;
			int		b = std::min(BINS - 1, (int)((c[bestAxis] - centroidBounds._min[bestAxis]) * bestScale));
			
			return (b <= bestSplit);
		});

	// securite si sah marche pa
	size_t midIdx = std::distance(clusters.begin(), mid);
	if (midIdx == start || midIdx == end) 
		midIdx = start + span / 2;

	// creation noed
	int	nodeIdx = _nodes.size();
	_nodes.push_back(Node());
	int	leftChild = buildSAHTopLevel(clusters, mortonPrims, srcObjects, start, midIdx);
	int	rightChild = buildSAHTopLevel(clusters, mortonPrims, srcObjects, midIdx, end);
	_nodes[nodeIdx].bbox = bounds;
	_nodes[nodeIdx].leftChildOrPrimOffset = leftChild;
	_nodes[nodeIdx].rightChildOffset = rightChild;
	_nodes[nodeIdx].primitiveCount = 0;
	_nodes[nodeIdx].axis = bestAxis;

	return (nodeIdx);
}

int BVHNode::buildLocalLBVH(std::vector<MortonPrimitive>& mortonPrims, 
							const std::vector<std::shared_ptr<Hittable>>& srcObjects,
							size_t start, size_t end, int bitShift)
{
	//arret si <= 4 objet ou plus de bit a ecamine
	if (end - start <= SAH_MIN_OBJS || bitShift < 0)
	{
		Node	leaf;

		leaf.leftChildOrPrimOffset = _orderedObjects.size();
		leaf.rightChildOffset = 0; // feuile donc inutils
		leaf.primitiveCount = end - start;
		leaf.axis = 0; 

		AABB	box;

		for (size_t i = start; i < end; i++)
		{
			AABB	b;
			size_t	idx = mortonPrims[i].primitiveIndex;
	
			srcObjects[idx]->bbox(b);
			box = AABB::AABB_union(box, b);
			_orderedObjects.push_back(srcObjects[idx]);
		}
		leaf.bbox = box;

		int	nodeIdx = _nodes.size();

		_nodes.push_back(leaf);
		return (nodeIdx);
	}

	// trouv separation
	uint64_t mask = 1ULL << bitShift;
	
	// trouver obj avec 1 = premier bit 
	auto split = std::partition_point(mortonPrims.begin() + start, mortonPrims.begin() + end,
		[mask](const MortonPrimitive& p)
		{
			return ((p.mortonCode & mask) == 0);
		});

	size_t	splitIndex = std::distance(mortonPrims.begin(), split);

	// obj tous meme bit on descend plus loin
	if (splitIndex == start || splitIndex == end)
		return (buildLocalLBVH(mortonPrims, srcObjects, start, end, bitShift - 1));

	// sinon creation neoud
	int nodeIdx = _nodes.size();
	_nodes.push_back(Node());

	int leftChild = buildLocalLBVH(mortonPrims, srcObjects, start, splitIndex, bitShift - 1);
	int rightChild = buildLocalLBVH(mortonPrims, srcObjects, splitIndex, end, bitShift - 1);

	_nodes[nodeIdx].bbox = AABB::AABB_union(_nodes[leftChild].bbox, _nodes[rightChild].bbox);
	_nodes[nodeIdx].leftChildOrPrimOffset = leftChild;
	_nodes[nodeIdx].rightChildOffset = rightChild;
	_nodes[nodeIdx].primitiveCount = 0;
	_nodes[nodeIdx].axis = bitShift % 3; 

	return (nodeIdx);
}

bool BVHNode::hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const
{
	if (_orderedObjects.empty() || _nodes.empty())
		return (false);
	Vec3f invDir(1.0f / ray._dir._x, 1.0f / ray._dir._y, 1.0f / ray._dir._z);
	// ray neg sur ?
	int dirIsNeg[3] =
	{
		ray._dir._x < 0,
		ray._dir._y < 0,
		ray._dir._z < 0
	};

	int	stack[128];
	int	stackPtr = 0;
	stack[stackPtr++] = 0;// premier noeu tjr 0

	bool	hitAnything = false;
	float	closestSoFar = tMax;

	while (stackPtr > 0)
	{
		int			nodeIdx = stack[--stackPtr];
		const Node&	node = _nodes[nodeIdx];

		if (!node.bbox.hit(ray, invDir, tMin, closestSoFar))
			continue;
		//feuille
		if (node.primitiveCount > 0)
		{
			for (uint32_t i = 0; i < node.primitiveCount; ++i)
			{
				auto&	obj = _orderedObjects[node.leftChildOrPrimOffset + i];
				if (obj->hit(ray, tMin, closestSoFar, rec))
				{
					hitAnything = true;
					closestSoFar = rec.t;
				}
			}
		}
		//interne
		else
		{
			int	firstChild = node.leftChildOrPrimOffset;
			int	secondChild = node.rightChildOffset;

			// si ray neg sur axe de coupe droit plus proche
			if (dirIsNeg[node.axis])
				std::swap(firstChild, secondChild);

			// plus loin en premier
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}
	return (hitAnything);
}

bool	BVHNode::bbox(AABB& output_box) const
{
	if (_nodes.empty())
		return false;		
	output_box = _nodes[0].bbox; 
	return true;
}