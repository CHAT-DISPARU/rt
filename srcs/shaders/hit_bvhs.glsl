bool hit_bvh_triangles(Ray ray, float t_min, float t_max, inout HitRecord rec)
{
	if (bvh_triangles.length() == 0)
		return false;

	vec3 safe_dir;
	safe_dir.x = abs(ray.dir.x) < 1e-6 ? (ray.dir.x < 0.0 ? -1e-6 : 1e-6) : ray.dir.x;
	safe_dir.y = abs(ray.dir.y) < 1e-6 ? (ray.dir.y < 0.0 ? -1e-6 : 1e-6) : ray.dir.y;
	safe_dir.z = abs(ray.dir.z) < 1e-6 ? (ray.dir.z < 0.0 ? -1e-6 : 1e-6) : ray.dir.z;
	
	vec3 invDir = 1.0 / safe_dir;
	
	int dirIsNeg[3];
	dirIsNeg[0] = (ray.dir.x < 0.0) ? 1 : 0;
	dirIsNeg[1] = (ray.dir.y < 0.0) ? 1 : 0;
	dirIsNeg[2] = (ray.dir.z < 0.0) ? 1 : 0;

	int stack[64]; 
	int stackPtr = 0;
	stack[stackPtr++] = 0;
	bool	hitAnything = false;
	float	closestSoFar = t_max;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_triangles[nodeIdx];

		if (!hit_aabb(node.aabb_min, node.aabb_max, ray, invDir, t_min, closestSoFar))
			continue;

		if (node.prim_count > 0)
		{
			for (int i = 0; i < node.prim_count; ++i)
			{
				int objIdx = node.left_child_or_prim_offset + i;

				if (hit_triangle(ray, triangles[objIdx], t_min, closestSoFar, rec))
				{
					hitAnything = true;
					closestSoFar = rec.t;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;
			
			if (dirIsNeg[node.axis] == 1)
			{
				int temp = firstChild;
				firstChild = secondChild;
				secondChild = temp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}
	return hitAnything;
}

bool hit_bvh_spheres(Ray ray, float t_min, float t_max, inout HitRecord rec)
{
	if (bvh_spheres.length() == 0)
		return false;

	vec3 safe_dir;
	safe_dir.x = abs(ray.dir.x) < 1e-6 ? (ray.dir.x < 0.0 ? -1e-6 : 1e-6) : ray.dir.x;
	safe_dir.y = abs(ray.dir.y) < 1e-6 ? (ray.dir.y < 0.0 ? -1e-6 : 1e-6) : ray.dir.y;
	safe_dir.z = abs(ray.dir.z) < 1e-6 ? (ray.dir.z < 0.0 ? -1e-6 : 1e-6) : ray.dir.z;
	
	vec3 invDir = 1.0 / safe_dir;
	
	int dirIsNeg[3];
	dirIsNeg[0] = (ray.dir.x < 0.0) ? 1 : 0;
	dirIsNeg[1] = (ray.dir.y < 0.0) ? 1 : 0;
	dirIsNeg[2] = (ray.dir.z < 0.0) ? 1 : 0;

	int stack[64]; 
	int stackPtr = 0;
	stack[stackPtr++] = 0;
	bool	hitAnything = false;
	float	closestSoFar = t_max;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_spheres[nodeIdx];

		if (!hit_aabb(node.aabb_min, node.aabb_max, ray, invDir, t_min, closestSoFar))
			continue;

		if (node.prim_count > 0)
		{
			for (int i = 0; i < node.prim_count; ++i)
			{
				int objIdx = node.left_child_or_prim_offset + i;

				if (hit_sphere(ray, spheres[objIdx], t_min, closestSoFar, rec))
				{
					hitAnything = true;
					closestSoFar = rec.t;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;
			
			if (dirIsNeg[node.axis] == 1)
			{
				int temp = firstChild;
				firstChild = secondChild;
				secondChild = temp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}
	return hitAnything;
}

bool hit_bvh_quads(Ray ray, float t_min, float t_max, inout HitRecord rec)
{
	if (bvh_quads.length() == 0)
		return false;

	vec3 safe_dir;
	safe_dir.x = abs(ray.dir.x) < 1e-6 ? (ray.dir.x < 0.0 ? -1e-6 : 1e-6) : ray.dir.x;
	safe_dir.y = abs(ray.dir.y) < 1e-6 ? (ray.dir.y < 0.0 ? -1e-6 : 1e-6) : ray.dir.y;
	safe_dir.z = abs(ray.dir.z) < 1e-6 ? (ray.dir.z < 0.0 ? -1e-6 : 1e-6) : ray.dir.z;
	
	vec3 invDir = 1.0 / safe_dir;
	
	int dirIsNeg[3];
	dirIsNeg[0] = (ray.dir.x < 0.0) ? 1 : 0;
	dirIsNeg[1] = (ray.dir.y < 0.0) ? 1 : 0;
	dirIsNeg[2] = (ray.dir.z < 0.0) ? 1 : 0;

	int stack[64]; 
	int stackPtr = 0;
	stack[stackPtr++] = 0;
	bool	hitAnything = false;
	float	closestSoFar = t_max;

	while (stackPtr > 0)
	{
		int nodeIdx = stack[--stackPtr];
		GPUBVHNode node = bvh_quads[nodeIdx];

		if (!hit_aabb(node.aabb_min, node.aabb_max, ray, invDir, t_min, closestSoFar))
			continue;

		if (node.prim_count > 0)
		{
			for (int i = 0; i < node.prim_count; ++i)
			{
				int objIdx = node.left_child_or_prim_offset + i;

				if (hit_quad(ray, quads[objIdx], t_min, closestSoFar, rec))
				{
					hitAnything = true;
					closestSoFar = rec.t;
				}
			}
		}
		else
		{
			int firstChild = node.left_child_or_prim_offset;
			int secondChild = node.right_child;
			
			if (dirIsNeg[node.axis] == 1)
			{
				int temp = firstChild;
				firstChild = secondChild;
				secondChild = temp;
			}
			stack[stackPtr++] = secondChild;
			stack[stackPtr++] = firstChild;
		}
	}
	return hitAnything;
}

bool	hit_scene(Ray r, float t_min, float t_max, inout HitRecord rec)
{
	bool		hit_anything = false;
	float		closest = t_max;
	HitRecord	temp_rec;

	if (hit_bvh_triangles(r, t_min, closest, temp_rec))
	{
		hit_anything = true;
		closest = temp_rec.t;
		rec = temp_rec;
	}
	if (hit_bvh_spheres(r, t_min, closest, temp_rec))
	{
		hit_anything = true;
		closest = temp_rec.t;
		rec = temp_rec;
	}
	if (hit_bvh_quads(r, t_min, closest, temp_rec))
	{
		hit_anything = true;
		closest = temp_rec.t;
		rec = temp_rec;
	}
	for (int i = 0; i < planes.length(); ++i) 
	{
		if (hit_plane(r, planes[i], t_min, closest, temp_rec))
		{
			hit_anything = true;
			closest = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}