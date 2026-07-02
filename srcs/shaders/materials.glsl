

//verre
float	schlick(float cosine, float ior_in, float ior_out)
{
	float	r0 = (ior_in - ior_out) / (ior_in + ior_out);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * pow(1.0 - cosine, 5.0);
}

//pbr
float	distribution_ggx(vec3 N, vec3 H, float roughness)
{
	float	a = roughness * roughness;
	float	a2 = a * a;
	float	NdotH = max(dot(N, H), 0.0);
	float	NdotH2 = NdotH * NdotH;
	float	denom = (NdotH2 * (a2 - 1.0) + 1.0);

	denom = M_PI * denom * denom;
	return a2 / max(denom, 1e-6);
}

float	geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float	r = roughness + 1.0;
	float	k = (r * r) / 8.0;
	float	NdotV = max(dot(N, V), 0.0);
	float	NdotL = max(dot(N, L), 0.0);
	float	ggx1 = NdotV / (NdotV * (1.0 - k) + k);
	float	ggx2 = NdotL / (NdotL * (1.0 - k) + k);
	return ggx1 * ggx2;
}

vec3	fresnel_schlick(float cosTheta, vec3 F0)
{
	float	t = pow(max(1.0 - cosTheta, 0.0), 5.0);
	return F0 + (vec3(1.0) - F0) * t;
}


//verre scatter

bool	Dielectric_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	vec3	albedo = mat.color;
	float	fuzz = mat.roughness;
	float	ni = mat.ior;

	attenuation = albedo;
	float	ratio = rec.front_face ? (rec.ni_from / ni) : (ni / rec.ni_from);
	vec3	unitDir = normalize(r_in.dir);
	float	cosTheta = min(dot(-unitDir, rec.normal), 1.0f);
	float	sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	bool	noRefract = ratio * sinTheta > 1.0f;
	vec3	direction;
	float	next_ior = r_in.current_ior;

	if (noRefract || schlick(cosTheta, rec.ni_from, ni) > randomFloat(seed))
		direction = reflect(unitDir, rec.normal);
	else
	{
		direction = refract(unitDir, rec.normal, ratio);
		next_ior = rec.front_face ? ni : 1.0;
	}
	vec3	finalDir = normalize(direction + fuzz * randomInUnitSphere(seed));
	scattered = Ray(rec.point, finalDir, next_ior);
	pdf = 1.0f;
	return (true);
}


//lambertian scatter

bool	Lambertian_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	vec3	albedo = mat.color;
	float	ao = 1.0;
	vec3 scatter_direction = normalize(rec.normal + randomUnitVector(seed));

	if (dot(scatter_direction, scatter_direction) < 1e-16)
		scatter_direction = rec.normal;
	scattered = Ray(rec.point, scatter_direction, r_in.current_ior);
	attenuation = albedo * ao;
	pdf = max(dot(rec.normal, scattered.dir), 0.0) / M_PI;
	return true;
}

//metal

bool	Metal_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	vec3	albedo = mat.color;
	float	fuzz = mat.roughness;
	vec3	reflected = reflect(normalize(r_in.dir), rec.normal);
	scattered = Ray(rec.point, normalize(reflected + randomInUnitSphere(seed) * fuzz), r_in.current_ior);
	attenuation = albedo;
	pdf = 1.0;
	return dot(scattered.dir, rec.normal) > 0.0;
}

bool	PBR_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	float	roughness = mat.roughness;
	float	metallic = mat.metallic;
	float	ao = 1.0; 
	vec3	base_color = mat.color;
	vec3	N = rec.normal;
	vec3	F0 = mix(vec3(0.04), base_color, metallic);
	vec3	V = normalize(-r_in.dir);
	//proba choisir le diffu
	float	spec_prob = max(metallic, 0.04);
	
	if (randomFloat(seed) < spec_prob)
	{
		float	a = roughness * roughness;
		float	r1 = randomFloat(seed);
		float	r2 = randomFloat(seed);
		float	phi = 2.0 * M_PI * r1;
		float	cosTheta = sqrt((1.0 - r2) / (1.0 + (a * a - 1.0) * r2));
		float	sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
		vec3	up = (abs(N.z) < 0.999) ? vec3(0,0,1) : vec3(1,0,0);
		vec3	tangentX = normalize(cross(up, N));
		vec3	tangentY = cross(N, tangentX);
		vec3	H = tangentX * (sinTheta * cos(phi)) + tangentY * (sinTheta * sin(phi)) + N * cosTheta;
		H = normalize(H);
		vec3	L = reflect(-V, H);
		if (dot(N, L) <= 0.0)
			return false;
		scattered = Ray(rec.point, L, r_in.current_ior);
		float	D = distribution_ggx(N, H, roughness);
		float	G = geometry_smith(N, V, L, roughness);
		vec3	F = fresnel_schlick(max(dot(H, V), 0.0), F0);
		float	NdotL = max(dot(N, L), 0.0);
		float	NdotV = max(dot(N, V), 1e-4);
		float	NdotH = max(dot(N, H), 0.0);
		float	HdotV = max(dot(H, V), 1e-4);
		float	pdf_H = (D * NdotH) / (4.0 * HdotV);
		pdf = pdf_H * spec_prob;
		if (pdf < 1e-6)
			return false;
		attenuation = (F * G * HdotV * ao) / (NdotV * NdotH * spec_prob);
		return true;
	}
	else
	{
		vec3	scatterDir = normalize(N + randomUnitVector(seed));
		if (dot(scatterDir, scatterDir) < 1e-16)
			scatterDir = N;
		scattered = Ray(rec.point, scatterDir, r_in.current_ior);
		
		float	cosine = max(dot(N, normalize(scatterDir)), 0.0);
		float	pdf_diffuse = cosine / M_PI;
		pdf = pdf_diffuse * (1.0 - spec_prob);
		if (pdf < 1e-6)
			return false;
		vec3	kD = (vec3(1.0) - F0) * (1.0 - metallic);
		attenuation = (kD * base_color * ao) / (1.0 - spec_prob);
		return true;
	}
}
//light
bool	DiffuseLight_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	return false;
}


bool	mat_scatter(Ray r_in, HitRecord rec, GPUMaterial mat, inout vec3 attenuation, inout Ray scattered, inout float pdf, inout uint seed)
{
	switch (mat.type)
	{
		case MAT_LAMBERTIAN:
			return Lambertian_scatter(r_in, rec, mat, attenuation, scattered, pdf, seed);
		case MAT_METAL:
			return Metal_scatter(r_in, rec, mat, attenuation, scattered, pdf, seed);
		case MAT_DIELECTRIC:
			return Dielectric_scatter(r_in, rec, mat, attenuation, scattered, pdf, seed);
		case MAT_PBR:
			return PBR_scatter(r_in, rec, mat, attenuation, scattered, pdf, seed);
		case MAT_LIGHT:
			return DiffuseLight_scatter(r_in, rec, mat, attenuation, scattered, pdf, seed);
		default:
			return false;
	}
}

vec3	mat_emitted(GPUMaterial mat, float u, float v, vec3 p)
{
	if (mat.type == MAT_LIGHT)
		return mat.emission;
	return vec3(0.0);
}