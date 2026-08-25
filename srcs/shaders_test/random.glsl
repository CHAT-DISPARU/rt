
#ifndef RANDOM_GLSL
#define RANDOM_GLSL

uint pcg_hash(uint v)
{
    uint state = v * 747796405u + 2891336453u;
    uint word  = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

uint init_seed(uvec2 pixel, uint frame_count, uint global_seed)
{
    uint h = pixel.x * 1973u + pixel.y * 9277u + frame_count * 26699u + global_seed * 40503u;
    return pcg_hash(h ^ 0x9e3779b9u);
}

float random_float(inout uint seed)
{
    seed = pcg_hash(seed);
    return float(seed) * (1.0 / 4294967296.0);
}

vec3 random_in_unit_disk(inout uint seed)
{
    for (int i = 0; i < 32; ++i)
    {
        vec3 p = vec3(random_float(seed) * 2.0 - 1.0, random_float(seed) * 2.0 - 1.0, 0.0);
        if (dot(p, p) < 1.0)
            return p;
    }
    return vec3(0.0);
}

vec3 random_in_unit_sphere(inout uint seed)
{
    for (int i = 0; i < 32; ++i)
    {
        vec3 p;
        p.x = random_float(seed) * 2.0 - 1.0;
        p.y = random_float(seed) * 2.0 - 1.0;
        p.z = random_float(seed) * 2.0 - 1.0;
        if (dot(p, p) < 1.0)
            return p;
    }
    return vec3(0.0);
}

vec3 random_unit_vector(inout uint seed)
{
    return normalize(random_in_unit_sphere(seed));
}

#endif
