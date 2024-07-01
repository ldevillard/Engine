#version 430 core

out vec4 FragColor;

uniform vec2 screenSize;
uniform uint frameCount;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 cameraPosition;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform int maxBounceCount;
uniform int numberRaysPerPixel;
uniform float divergeStrength;

uniform uint bvhEnabled;

#define BVH_DEPTH 15

const int checkerPattern = 1;
const int hideEmissive = 2;

struct Material
{
	vec3 color;
	vec3 emissiveColor;
	vec3 specularColor;
	int flag;
	float emissiveStrength;
	float smoothness;
	float specularProbability;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Sphere
{
	vec3 position;
	float radius;
	Material material;
};

struct Cube
{
	vec3 min;
	vec3 max;
	mat4 transform;
	mat4 inverseTransform;
	Material material;
};

struct Triangle
{
	vec3 pA;
	vec3 pB;
	vec3 pC;

	vec3 nA;
	vec3 nB;
	vec3 nC;
};

struct HitInfo
{
	bool hit;
	float distance;
	vec3 hitPoint;
	vec3 normal;
	Material material;
};

struct MeshInfo
{
	int firstTriangleIndex;
	int firstNodeIndex;
	mat4 transform;
	mat4 inverseTransform;
	Material material;
};

struct BVHNode
{
	vec3 boundsMin;
	vec3 boundsMax;

	int triangleIndex;
	int triangleCount;
	int childIndex;
};

uniform int sphereCount;
layout(std430, binding = 0) buffer sphereData
{
	Sphere spheres[];
};

uniform int cubeCount;
layout(std430, binding = 1) buffer cubeData
{
	Cube cubes[];
};

layout(std430, binding = 2) buffer triangleData
{
	Triangle triangles[];
};

uniform int meshCount;
layout(std430, binding = 3) buffer meshData
{
	MeshInfo meshes[];
};

layout(std430, binding = 4) buffer bvhNodesData
{
	BVHNode bvhNodes[];
};

uint NextRandom(inout uint state)
{
	state = state * 747796405 + 2891336453;
	uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
	result = (result >> 22) ^ result;
	return result;
}

float RandomValue(inout uint state)
{
	return NextRandom(state) / 4294967295.0; // 2^32 - 1
}

float RandomValueNormalDistribution(inout uint state)
{
	// https://stackoverflow.com/a/6178290
	float theta = 2.0 * 3.1415926 * RandomValue(state);
	float rho = sqrt(-2.0 * log(RandomValue(state)));
	return rho * cos(theta);
}

vec3 RandomDirection(inout uint state)
{
	// https://math/stackexchange.com/a/1585996
	float x = RandomValueNormalDistribution(state);
	float y = RandomValueNormalDistribution(state);
	float z = RandomValueNormalDistribution(state);

	return normalize(vec3(x, y, z));
}

vec3 RandomHemisphereDirection(vec3 normal, inout uint rngState)
{
	vec3 randomDirection = RandomDirection(rngState);
	return dot(randomDirection, normal) > 0.0 ? randomDirection : -randomDirection;
}

vec2 RandomPointInCircle(uint rngState)
{
	float angle = RandomValue(rngState) * 2.0 * 3.1415926;
	vec2 pointOnCircle = vec2(cos(angle), sin(angle));
	return pointOnCircle * sqrt(RandomValue(rngState));;

}

HitInfo RaySphere(Ray ray, vec3 sphereCenter, float sphereRadius)
{
	HitInfo hitInfo;
	hitInfo.hit = false;

	// related to https://iquilezles.org/articles/intersectors/
	
	vec3 offsetRayOrigin = ray.origin - sphereCenter;

	float b = dot(offsetRayOrigin, ray.direction);
	float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;
	float h = b * b - c;

	if (h >= 0)
	{
		h = sqrt(h);
		float dst = -b - h;

		if (dst >= 0)
		{
			hitInfo.hit = true;
			hitInfo.distance = dst;
			hitInfo.hitPoint = ray.origin + ray.direction * dst;
			hitInfo.normal = normalize(hitInfo.hitPoint - sphereCenter);
		}
	}

	return hitInfo;
}

HitInfo RayCube(Ray ray, Cube cube) // box in case
{
	HitInfo hitInfo;
	hitInfo.hit = false;

	mat4 txi = cube.inverseTransform;

	vec3 bmin = cube.min;
	vec3 bmax = cube.max;

	vec3 ro = vec3((txi * vec4(ray.origin, 1.0)).xyz);
	vec3 rd = vec3((txi * vec4(ray.direction, 0.0)).xyz);

	vec3 invRd = 1.0 / rd;

	vec3 t1 = (bmin - ro) * (1.0 / rd);
	vec3 t2 = (bmax - ro) * (1.0 / rd);

	vec3 tNear = min(t1, t2);
	vec3 tFar = max(t1, t2);

	float tNearMax = max(max(tNear.x, tNear.y), tNear.z);
	float tFarMin = min(min(tFar.x, tFar.y), tFar.z);

	if (tNearMax <= tFarMin && tNearMax > 0)
	{
		hitInfo.hit = true;
		hitInfo.distance = tNearMax;
		vec3 hitPointWorld = ray.origin + ray.direction * tNearMax;
		hitInfo.hitPoint = hitPointWorld;
		vec3 hitPointLocal = vec3((txi * vec4(hitPointWorld, 1.0)).xyz);

		// compute normal
		vec3 normal = vec3(0);
		vec3 center = (bmin + bmax) * 0.5;
		vec3 dir = normalize(hitPointLocal - center);
		
		vec3 absDir = abs(dir);
		
		if (absDir.x > absDir.y && absDir.x > absDir.z)
		    normal = vec3(sign(dir.x), 0.0, 0.0);
		else if (absDir.y > absDir.x && absDir.y > absDir.z)
		    normal = vec3(0.0, sign(dir.y), 0.0);
		else
		    normal =  vec3(0.0, 0.0, sign(dir.z));

		hitInfo.normal = normalize(vec3((cube.transform * vec4(normal, 0.0)).xyz));
	}

	return hitInfo;
}

HitInfo RayTriangle(Ray ray, Ray localRay, Triangle triangle, mat4 tx)
{	
	HitInfo hitInfo;
	hitInfo.hit = false;

	vec3 AB = triangle.pB - triangle.pA;
    vec3 AC = triangle.pC - triangle.pA;
	
    vec3 n = cross(AB, AC);
	float det = -dot(localRay.direction, n);
	
	if (det >= 1e-20)
	{
		float inverseDet = 1.0 / det;
		vec3 AO = localRay.origin - triangle.pA;
		vec3 DAO = cross(AO, localRay.direction);
		
		float u = dot(AC, DAO) * inverseDet;
		float v = -dot(AB, DAO) * inverseDet;
		float w = 1 - u - v;
		
		if (u >= 0 && v >= 0 && w >= 0)
		{
			float distance = dot(AO, n) * inverseDet;
			if (distance >= 0)
			{
				hitInfo.hit = true;
				hitInfo.distance = distance;
				hitInfo.hitPoint = ray.origin + ray.direction * distance;
				
				vec3 localNormal = normalize(triangle.nA * w + triangle.nB * u + triangle.nC * v);
				hitInfo.normal = normalize((tx * vec4(localNormal, 0.0)).xyz);
			}
		}
	}
	
	return hitInfo;
}

bool RayBoundingBox(Ray ray, vec3 boxMin, vec3 boxMax)
{
	vec3 invDir = 1 / ray.direction;
	vec3 tMin = (boxMin - ray.origin) * invDir;
	vec3 tMax = (boxMax - ray.origin) * invDir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	return tNear <= tFar;
};

HitInfo RayTriangleBVH(Ray ray, int triangleIndex, int nodeIndex, mat4 tx, mat4 txi)
{
	BVHNode nodeStack[BVH_DEPTH];
	int stackIndex = 0;
	nodeStack[stackIndex++] = bvhNodes[nodeIndex];

	HitInfo hitInfo;
	hitInfo.hit = false;
	hitInfo.distance = 1.0 / 0.0; // infinity

	Ray localRay = ray;
	localRay.origin = vec3((txi * vec4(ray.origin, 1.0)).xyz);
	localRay.direction = vec3((txi * vec4(ray.direction, 0.0)).xyz);

	while (stackIndex > 0)
	{
		BVHNode node = nodeStack[--stackIndex];

		if (RayBoundingBox(localRay, node.boundsMin, node.boundsMax))
		{
			if ((node.childIndex == 0 && bvhEnabled == 1) || bvhEnabled == 0) // leaf node
			{
				for (int i = triangleIndex + node.triangleIndex; i < triangleIndex + node.triangleIndex + node.triangleCount; i++)
				{
					HitInfo triangleHitInfo = RayTriangle(ray, localRay, triangles[i], tx);
					if (triangleHitInfo.hit && triangleHitInfo.distance < hitInfo.distance)
						hitInfo = triangleHitInfo;
				}
			}
			else if (bvhEnabled == 1)
			{
				nodeStack[stackIndex++] = bvhNodes[nodeIndex + node.childIndex + 1];
				nodeStack[stackIndex++] = bvhNodes[nodeIndex + node.childIndex + 0];
			}
		}
	}

	return hitInfo;
}

HitInfo CalculateRayCollision(Ray ray)
{
	HitInfo hitInfo;
	hitInfo.hit = false;
	hitInfo.distance = 1.0 / 0.0; // infinity
	hitInfo.material.color = vec3(0.1, 0.1, 0.1);

	for (int i = 0; i < sphereCount; i++)
	{
		Sphere sphere = spheres[i];
		
		HitInfo hit = RaySphere(ray, sphere.position, sphere.radius);

		if (hit.hit && hit.distance < hitInfo.distance)
		{
			hitInfo = hit;
			hitInfo.material = sphere.material;
		}
	}

	for (int i = 0; i < cubeCount; i++)
	{
		Cube cube = cubes[i];

		HitInfo hit = RayCube(ray, cube);

		if (hit.hit && hit.distance < hitInfo.distance)
		{
			hitInfo = hit;
			hitInfo.material = cube.material;
		}
	}

	for (int i = 0; i < meshCount; i++)
	{
		MeshInfo meshInfo = meshes[i];

		HitInfo hit = RayTriangleBVH(ray, meshInfo.firstTriangleIndex, meshInfo.firstNodeIndex, meshInfo.transform, meshInfo.inverseTransform);
	
		if (hit.hit && hit.distance < hitInfo.distance)
		{
			hitInfo = hit;
			hitInfo.material = meshInfo.material;
		}
	}

	return hitInfo;
}

vec3 Trace(Ray ray, inout uint rngState)
{
	vec3 incomingLight = vec3(0);
	vec3 rayColor = vec3(1);

	for (int i = 0; i <= maxBounceCount; i++)
	{
		HitInfo hitInfo = CalculateRayCollision(ray);

		if (hitInfo.hit)
		{
			Material material = hitInfo.material;

			if (material.flag == checkerPattern)
			{
				vec2 c = mod(floor(hitInfo.hitPoint.xz), vec2(2.0));
				material.color = c.x == c.y ? material.color : material.emissiveColor;
			}
			else if (material.flag == hideEmissive && i == 0)
			{
				ray.origin = hitInfo.hitPoint + ray.direction * 0.001;
				continue;
			}

			ray.origin = hitInfo.hitPoint;
			vec3 diffuseDirection = normalize(hitInfo.normal + RandomDirection(rngState));
			vec3 specularDirection = reflect(ray.direction, hitInfo.normal);

			bool isSpecular = RandomValue(rngState) < material.specularProbability;
			ray.direction = mix(diffuseDirection, specularDirection, material.smoothness * int(isSpecular));

			vec3 emittedLight = material.emissiveColor * material.emissiveStrength;
			incomingLight += emittedLight * rayColor;
			rayColor *= isSpecular ? material.specularColor : material.color;
		}
		else
		{
			incomingLight += vec3(0.1) * rayColor;
			break ;
		}
	}
	return incomingLight;
}

void main()
{
	vec2 fragCoordNorm = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;

	vec4 clipCoord = vec4(fragCoordNorm, 0.0, 1.0);
	vec4 viewCoord = invProjection * clipCoord;
	viewCoord.z = -1.0;

	vec4 worldCoord = invView * viewCoord;

	vec3 worldPosition = worldCoord.xyz / worldCoord.w;

	uint pixelIndex = uint(gl_FragCoord.y * screenSize.x + gl_FragCoord.x);
	uint rngState = pixelIndex + frameCount * 719393;

	vec3 totalIncomingLight = vec3(0);
	for (int i = 0; i < numberRaysPerPixel; i++)
	{
		Ray ray;
		ray.origin = cameraPosition;
		vec2 randomPoint = RandomPointInCircle(rngState) * divergeStrength / screenSize.x;
		vec3 randomPos = worldPosition + cameraRight * randomPoint.x + cameraUp * randomPoint.y;

		ray.direction = normalize(randomPos - ray.origin);

		totalIncomingLight += Trace(ray, rngState);
	}

	FragColor = vec4(totalIncomingLight / numberRaysPerPixel, 1);
}