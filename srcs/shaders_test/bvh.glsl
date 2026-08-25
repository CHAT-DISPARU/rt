
#ifndef BVH_GLSL
#define BVH_GLSL

#define BVH_STACK_SIZE 48

bool hit_bvh_triangles(Ray ray, float tMin, inout float tMax, inout HitRecord rec)
{
	if (bvh_triangles.length() == 0)
		return false;

	vec3 invDir = vec3(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
	bool dirIsNeg[3];
	dirIsNeg[0] = ray.dir.x < 0.0;
	dirIsNeg[1] = ray.dir.y < 0.0;
	dirIsNeg[2] = ray.dir.z < 0.0;

	int stack[BVH_STACK_SIZE];
	int stackPtr = 0;
	stack[stackPtr++] = 0;

	bool hitAnything = false;
	float closestSoFar = tMax;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_triangles[nodeIdx];

		if (!aabb_hit(node.aabb_min, node.aabb_max, ray.origin, invDir, tMin, closestSoFar))
			continue;

		if (node.primitive_count > 0)
		{
			for (int i = 0; i < node.primitive_count; ++i)
			{
				HitRecord tmp;
				if (intersect_triangle(node.left_child_or_prim_offset + i, ray, tMin, closestSoFar, tmp))
				{
					hitAnything = true;
					closestSoFar = tmp.t;
					rec = tmp;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;

			if (dirIsNeg[node.axis])
			{
				int tmp = firstChild;
				firstChild = secondChild;
				secondChild = tmp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}

	if (hitAnything)
		tMax = closestSoFar;
	return hitAnything;
}

bool hit_bvh_spheres(Ray ray, float tMin, inout float tMax, inout HitRecord rec)
{
	if (bvh_spheres.length() == 0)
		return false;

	vec3 invDir = vec3(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
	bool dirIsNeg[3];
	dirIsNeg[0] = ray.dir.x < 0.0;
	dirIsNeg[1] = ray.dir.y < 0.0;
	dirIsNeg[2] = ray.dir.z < 0.0;

	int stack[BVH_STACK_SIZE];
	int stackPtr = 0;
	stack[stackPtr++] = 0;

	bool hitAnything = false;
	float closestSoFar = tMax;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_spheres[nodeIdx];

		if (!aabb_hit(node.aabb_min, node.aabb_max, ray.origin, invDir, tMin, closestSoFar))
			continue;

		if (node.primitive_count > 0)
		{
			for (int i = 0; i < node.primitive_count; ++i)
			{
				HitRecord tmp;
				if (intersect_sphere(node.left_child_or_prim_offset + i, ray, tMin, closestSoFar, tmp))
				{
					hitAnything = true;
					closestSoFar = tmp.t;
					rec = tmp;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;

			if (dirIsNeg[node.axis])
			{
				int tmp = firstChild;
				firstChild = secondChild;
				secondChild = tmp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}

	if (hitAnything)
		tMax = closestSoFar;
	return hitAnything;
}

bool hit_bvh_quads(Ray ray, float tMin, inout float tMax, inout HitRecord rec)
{
	if (bvh_quads.length() == 0)
		return false;

	vec3 invDir = vec3(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
	bool dirIsNeg[3];
	dirIsNeg[0] = ray.dir.x < 0.0;
	dirIsNeg[1] = ray.dir.y < 0.0;
	dirIsNeg[2] = ray.dir.z < 0.0;

	int stack[BVH_STACK_SIZE];
	int stackPtr = 0;
	stack[stackPtr++] = 0;

	bool hitAnything = false;
	float closestSoFar = tMax;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_quads[nodeIdx];

		if (!aabb_hit(node.aabb_min, node.aabb_max, ray.origin, invDir, tMin, closestSoFar))
			continue;

		if (node.primitive_count > 0)
		{
			for (int i = 0; i < node.primitive_count; ++i)
			{
				HitRecord tmp;
				if (intersect_quad(node.left_child_or_prim_offset + i, ray, tMin, closestSoFar, tmp))
				{
					hitAnything = true;
					closestSoFar = tmp.t;
					rec = tmp;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;

			if (dirIsNeg[node.axis])
			{
				int tmp = firstChild;
				firstChild = secondChild;
				secondChild = tmp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}

	if (hitAnything)
		tMax = closestSoFar;
	return hitAnything;
}

bool hit_planes(Ray ray, float tMin, inout float tMax, inout HitRecord rec)
{
	bool hitAnything = false;
	int count = planes.length();

	for (int i = 0; i < count; ++i)
	{
		HitRecord tmp;
		if (intersect_plane(i, ray, tMin, tMax, tmp))
		{
			hitAnything = true;
			tMax = tmp.t;
			rec = tmp;
		}
	}
	return hitAnything;
}

bool scene_hit(Ray ray, float tMin, float tMax, out HitRecord rec)
{
	bool hitAnything = false;
	float closest = tMax;
	HitRecord temp_rec;

	if (hit_bvh_triangles(ray, tMin, closest, temp_rec))
	{
		hitAnything = true;
		rec = temp_rec;
	}
	if (hit_bvh_spheres(ray, tMin, closest, temp_rec))
	{
		hitAnything = true;
		rec = temp_rec;
	}
	if (hit_bvh_quads(ray, tMin, closest, temp_rec))
	{
		hitAnything = true;
		rec = temp_rec;
	}
	if (hit_planes(ray, tMin, closest, temp_rec))
	{
		hitAnything = true;
		rec = temp_rec;
	}
	return hitAnything;
}

#endif
