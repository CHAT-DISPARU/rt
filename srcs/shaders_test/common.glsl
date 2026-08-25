
#ifndef COMMON_GLSL
#define COMMON_GLSL

// Point_Mult_mat4(v, m)
vec3 transform_point(mat4 m, vec3 v)
{
    float w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + m[3][3];
    if (w == 0.0)
        w = 1.0;

    vec3 res;
    res.x = (v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3]) / w;
    res.y = (v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3]) / w;
    res.z = (v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3]) / w;
    return res;
}

//operator*(Mat4) / operator*=(Mat4) (partie lineaire
vec3 transform_dir(mat4 m, vec3 v)
{
    vec3 res;
    res.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2];
    res.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2];
    res.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2];
    return res;
}

Ray transform_ray(Ray r, mat4 m)
{
    Ray out_r;
    out_r.origin = transform_point(m, r.origin);
    out_r.dir    = transform_dir(m, r.dir);
    return out_r;
}

vec3 vec_reflect(vec3 v, vec3 n)
{
    return v - 2.0 * dot(v, n) * n;
}

vec3 vec_refract(vec3 v, vec3 n, float ni)
{
    float cos_theta = min(dot(-v, n), 1.0);
    vec3  r_out_perp = ni * (v + cos_theta * n);
    vec3  r_out_parallel = -sqrt(abs(1.0 - dot(r_out_perp, r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

bool vec_near_zero(vec3 v)
{
    const float eps = 1.1920929e-7; //epsilon
    return (abs(v.x) < eps && abs(v.y) < eps && abs(v.z) < eps);
}

#endif
