uint	pcg_hash(inout uint seed)
{
	uint	state = seed * 747796405u + 2891336453u;
	uint	word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	seed = (word >> 22u) ^ word;
	return seed;
}

float	randomFloat(inout uint seed)
{
	return float(pcg_hash(seed)) / 4294967296.0;
}

vec3	randomUnitVector(inout uint seed)
{
	float	r1 = randomFloat(seed);
	float	r2 = randomFloat(seed);
	float	phi = 2.0 * M_PI * r1;
	float	cosTheta = 1.0 - 2.0 * r2;
	float	sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
	return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}


//methode rejet
vec3 randomInUnitSphere(inout uint seed)
{
	for (int i = 0; i < 16; i++)
	{
		vec3 p = vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed)) * 2.0 - 1.0;
		if (dot(p, p) < 1.0)
			return p;
	}
	return vec3(0.0, 1.0, 0.0);
}