# RT C++ + Vulkan TODO LIST
 
## 1 - Math
- [x] Vec3      -- surcharge +, -, *, /, dot, cross, length, normalized
- [x] Mat4		-- surcharge + - * / ~ 
- [x] Ray       -- origin + dir, at(t) = origin + t*dir
- [x] HitRecord -- point, normale, t, front_face, materiau
- [x] AABB      -- min + max, hit(ray, tMin, tMax)

## 2 - Formes materiaux
- [x] Hittable   -- abstraite, hit() et bbox() virtuelles pures
- [x] Sphere     -- herite Hittable, center + radius + material
- [x] Plane      -- herite Hittable, AABB infinie a gerer
- [x] Triangle   -- herite Hittable, Moller-Trumbore, base des mesh
- [x] Material   -- abstraite, scatter() virtuelle pure, emitted() = 0 par defaut
- [x] Lambertian -- herite Material, rayon aleatoire autour de la normale
- [x] Metal      -- herite Material, reflect() + flou selon roughness
- [x] Dielectric -- herite Material, verre, Schlick, reflexion ou refraction

## 3 - Scene & rendu CPU
- [x] BVHNode  -- herite Hittable, trie par axe, divise recursivement
- [x] Scene    -- vector<shared_ptr<Hittable>>, hit() retourne le plus proche
- [x] Camera   -- getRay(u, v), anti-aliasing par perturbation UV
- [ ] Renderer -- traceRay() recursif, scatter(), std::thread

## 4 - Parser GLB
- [ ] cgltf.h   -- ou faire soi meme un parser
- [ ] Geometrie -- triangles, UV, normales. Materiaux ignores pour l'instant.
- [ ] Materiaux -- albedo d'abord, metallic + roughness ensuite
- [ ] Textures  -- images embarquees dans le GLB, mapper sur UV
- [ ] Normales  -- normal maps, meme principe que les bump maps
- [ ] Lumieres  -- KHR_lights_punctual : point, spot, directional

## 5 - Vulkan compute
> Pas avant d'avoir un rendu sur CPU.
 
- [ ] Structs GPU -- GPUSphere, GPUMaterial, GPUBVHNode. POD, 16 bytes.
- [ ] VulkanContext -- instance, physical device, logical device, compute queue
- [ ] Buffers -- upload scene, buffer pixels sortie
- [ ] Descriptors -- brancher buffers au shader (binding 0 = spheres, 1 = pixels)
- [ ] Pipeline -- charger le .spv compile par glslc
- [ ] shader.comp -- traceRay en GLSL, boucle for (pas de recursion), if/else sur type
- [ ] Dispatch -- vkCmdDispatch, fence, readback, passer a MiniLibX

## 6 - Optimisations GPU
> Une fois que rendu GPU = CPU.
 
- [ ] BVH shader -- arbre serialise, traversal avec pile locale dans le shader
- [ ] Staging -- séparer mémoire CPU-visible et DEVICE_LOCAL j'ai pas compris de ouf mais a voir si je le fais
- [ ] Push constants -- au lieu d envoye tout


https://jacco.ompf2.com/2022/04/18/how-to-build-a-bvh-part-2-faster-rays/