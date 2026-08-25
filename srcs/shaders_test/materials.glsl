
#ifndef MATERIALS_GLSL
#define MATERIALS_GLSL


bool scatter_lambertian(GPUMaterial mat, Ray r_in, HitRecord rec, out vec3 attenuation,
						 out Ray scattered, out float pdf, inout uint seed)
{
	vec3 albedo = mat.color;
	float ao = 1.0;
	vec3 scatter_direction = rec.normal + random_unit_vector(seed);

	if (dot(scatter_direction, scatter_direction) < 1e-8)
		scatter_direction = rec.normal;

	scattered.origin = rec.point;
	scattered.dir = scatter_direction;
	attenuation = albedo * ao;
	pdf = dot(rec.normal, scattered.dir) / PI;
	return true;
}

float scattering_pdf_lambertian(GPUMaterial mat, Ray r_in, HitRecord rec, Ray scattered)
{
	float cosine = dot(rec.normal, normalize(scattered.dir));
	return (cosine < 0.0) ? 0.0 : cosine / PI;
}


bool scatter_metal(GPUMaterial mat, Ray r_in, HitRecord rec, out vec3 attenuation,
					out Ray scattered, out float pdf, inout uint seed)
{
	vec3 albedo = mat.color;
	float fuzz = mat.roughness;
	vec3 reflected = vec_reflect(normalize(r_in.dir), rec.normal);

	scattered.origin = rec.point;
	scattered.dir = reflected + random_in_unit_sphere(seed) * fuzz;

	attenuation = albedo;
	pdf = 1.0;
	return (dot(scattered.dir, rec.normal) > 0.0);
}

float schlick(float cosine, float ior_in, float ior_out)
{
	float r0 = (ior_in - ior_out) / (ior_in + ior_out);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * pow(1.0 - cosine, 5.0);
}

bool scatter_dielectric(GPUMaterial mat, Ray r_in, HitRecord rec, out vec3 attenuation,
						 out Ray scattered, out float pdf, inout uint seed)
{
	vec3 albedo = mat.color;
	float fuzz = mat.roughness;

	attenuation = albedo;
	float ratio = rec.front_face ? (rec.ni_from / mat.ior) : (mat.ior / rec.ni_from);
	vec3 unitDir = normalize(r_in.dir);
	float cosTheta = min(dot(-unitDir, rec.normal), 1.0);
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	bool noRefract = ratio * sinTheta > 1.0;
	vec3 direction;

	if (noRefract || schlick(cosTheta, rec.ni_from, mat.ior) > random_float(seed))
		direction = vec_reflect(unitDir, rec.normal);
	else
		direction = vec_refract(unitDir, rec.normal, ratio);

	vec3 finalDir = normalize(direction + fuzz * random_in_unit_sphere(seed));

	scattered.origin = rec.point;
	scattered.dir = finalDir;
	pdf = 1.0;
	return true;
}

float distribution_ggx(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return a2 / max(denom, 1e-6);
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Karis
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);

	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
	float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
	return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
	float t = pow(max(1.0 - cosTheta, 0.0), 5.0);
	return F0 + (vec3(1.0) - F0) * t;
}

bool scatter_pbr(GPUMaterial mat, Ray r_in, HitRecord rec, out vec3 attenuation,
				  out Ray scattered, out float pdf, inout uint seed)
{
	float roughness = mat.roughness;
	float metallic = mat.metallic;
	float ao = 1.0;
	vec3 base_color = mat.color;

	vec3 N = rec.normal;
	vec3 F0 = vec3(0.04) * (1.0 - metallic) + base_color * metallic;
	vec3 V = normalize(-r_in.dir);
	float spec_prob = max(metallic, 0.04);

	if (random_float(seed) < spec_prob)
	{
		float a = roughness * roughness;
		float r1 = random_float(seed);
		float r2 = random_float(seed);
		float phi = 2.0 * PI * r1;
		float cosTheta = sqrt((1.0 - r2) / (1.0 + (a * a - 1.0) * r2));
		float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
		vec3 up = (abs(N.z) < 0.999) ? vec3(0, 0, 1) : vec3(1, 0, 0);
		vec3 tangentX = normalize(cross(up, N));
		vec3 tangentY = cross(N, tangentX);
		vec3 H = tangentX * (sinTheta * cos(phi)) + tangentY * (sinTheta * sin(phi)) + N * cosTheta;
		H = normalize(H);
		vec3 L = vec_reflect(-V, H);

		if (dot(N, L) <= 0.0)
			return false;

		scattered.origin = rec.point;
		scattered.dir = L;

		float D = distribution_ggx(N, H, roughness);
		float G = geometry_smith(N, V, L, roughness);
		vec3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);
		float NdotL = max(dot(N, L), 0.0);
		float NdotV = max(dot(N, V), 1e-4);
		vec3 specular = (F * D * G) / (4.0 * NdotV * NdotL + 1e-6);
		float NdotH = max(dot(N, H), 0.0);
		float pdf_H = D * NdotH / (4.0 * max(dot(H, V), 1e-4));

		pdf = pdf_H * spec_prob;
		if (pdf < 1e-6)
			return false;
		attenuation = specular * NdotL * ao / pdf;
		return true;
	}
	else
	{
		vec3 scatterDir = N + random_unit_vector(seed);
		if (vec_near_zero(scatterDir))
			scatterDir = N;

		scattered.origin = rec.point;
		scattered.dir = scatterDir;

		float cosine = max(dot(N, normalize(scatterDir)), 0.0);
		float pdf_diffuse = cosine / PI;

		pdf = pdf_diffuse * (1.0 - spec_prob);
		if (pdf < 1e-6)
			return false;

		vec3 kD = (vec3(1.0) - F0) * (1.0 - metallic);
		attenuation = (kD * base_color * ao) / (1.0 - spec_prob);
		return true;
	}
}

float scattering_pdf_pbr(GPUMaterial mat, Ray r_in, HitRecord rec, Ray scattered)
{
	float roughness = mat.roughness;
	float metallic = mat.metallic;
	vec3 N = rec.normal;
	vec3 V = normalize(-r_in.dir);
	vec3 L = normalize(scattered.dir);
	float NdotL = dot(N, L);

	if (NdotL <= 0.0)
		return 0.0;

	float spec_prob = max(metallic, 0.04);
	float pdf_diffuse = NdotL / PI;
	vec3 H = normalize(V + L);
	float NdotH = max(dot(N, H), 0.0);
	float HdotV = max(dot(H, V), 1e-4);
	float D = distribution_ggx(N, H, roughness);
	float pdf_specular = (D * NdotH) / (4.0 * HdotV);
	float pdf_total = pdf_diffuse * (1.0 - spec_prob) + pdf_specular * spec_prob;

	return max(pdf_total, 0.0);
}


bool material_scatter(int matIdx, Ray r_in, HitRecord rec, out vec3 attenuation,
					   out Ray scattered, out float pdf, inout uint seed)
{
	GPUMaterial mat = materials[matIdx];

	if (mat.type == MAT_LAMBERTIAN)
		return scatter_lambertian(mat, r_in, rec, attenuation, scattered, pdf, seed);
	else if (mat.type == MAT_METAL)
		return scatter_metal(mat, r_in, rec, attenuation, scattered, pdf, seed);
	else if (mat.type == MAT_DIELECTRIC)
		return scatter_dielectric(mat, r_in, rec, attenuation, scattered, pdf, seed);
	else if (mat.type == MAT_PBR)
		return scatter_pbr(mat, r_in, rec, attenuation, scattered, pdf, seed);

	attenuation = vec3(0.0);
	scattered.origin = rec.point;
	scattered.dir = rec.normal;
	pdf = 0.0;
	return false;
}

float material_scattering_pdf(int matIdx, Ray r_in, HitRecord rec, Ray scattered)
{
	GPUMaterial mat = materials[matIdx];

	if (mat.type == MAT_LAMBERTIAN)
		return scattering_pdf_lambertian(mat, r_in, rec, scattered);
	else if (mat.type == MAT_PBR)
		return scattering_pdf_pbr(mat, r_in, rec, scattered);
	return 0.0;
}

vec3 material_emitted(int matIdx, float u, float v, vec3 p)
{
	GPUMaterial mat = materials[matIdx];
	if (mat.type == MAT_EMISSIVE)
		return mat.emission;
	return vec3(0.0);
}

#endif
