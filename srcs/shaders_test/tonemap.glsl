

#ifndef TONEMAP_GLSL
#define TONEMAP_GLSL

uint tonemap_pack(vec3 fc)
{
	fc.x = max(0.0, fc.x);
	fc.y = max(0.0, fc.y);
	fc.z = max(0.0, fc.z);

	fc.x = pow(fc.x / (fc.x + 1.0), 1.0 / 2.2);
	fc.y = pow(fc.y / (fc.y + 1.0), 1.0 / 2.2);
	fc.z = pow(fc.z / (fc.z + 1.0), 1.0 / 2.2);

	uint ir = uint(255.999 * min(1.0, fc.x));
	uint ig = uint(255.999 * min(1.0, fc.y));
	uint ib = uint(255.999 * min(1.0, fc.z));
	return (ir << 24) | (ig << 16) | (ib << 8) | 0xFFu;
}

#endif
