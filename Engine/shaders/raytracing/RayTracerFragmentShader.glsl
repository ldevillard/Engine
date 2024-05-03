#version 430 core

out vec4 FragColor;

uniform vec2 screenSize;
uniform uint frameCount;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 cameraPosition;

uniform int maxBounceCount;
uniform int numberRaysPerPixel;

struct Material
{
	vec3 color;
	vec3 emissiveColor;
	float emissiveStrength;
	float smoothness;
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
	Material material;
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


struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct HitInfo
{
	bool hit;
	float distance;
	vec3 hitPoint;
	vec3 normal;
	Material material;
};

float RandomValue(inout uint state)
{
	state *= (state + 195439) * (state + 124395) * (state + 845921);
	return state / 4294967295.0;
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

	// related to EditorCollider.cpp IntersectRay() method
	// transform the ray to the local space of the box
	vec4 ro1 = inverse(cube.transform) * vec4(ray.origin, 1.0);
	vec4 rd1 = inverse(cube.transform) * vec4(ray.direction, 0.0);

	vec3 ro = ro1.xyz;
	vec3 rd = rd1.xyz;

	vec3 tMin = (cube.min - ro) / rd;
	vec3 tMax = (cube.max - ro) / rd;

	vec3 tNear = min(tMin, tMax);
	vec3 tFar = max(tMin, tMax);

	float tNearMax = max(max(tNear.x, tNear.y), tNear.z);
	float tFarMin = min(min(tFar.x, tFar.y), tFar.z);

	// intersection
	if (tNearMax <= tFarMin)
	{
		hitInfo.hit = true;
		hitInfo.distance = tNearMax;
		hitInfo.hitPoint = ro + rd * tNearMax;

		vec3 normal;

		if (tNear.x > tNear.y && tNear.x > tNear.z)
			normal = vec3(-1.0, 0.0, 0.0); // Left face
		else if (tNear.y > tNear.x && tNear.y > tNear.z)
			normal = vec3(0.0, -1.0, 0.0); // Bottom face
		else if (tNear.z > tNear.x && tNear.z > tNear.y)
			normal = vec3(0.0, 0.0, -1.0); // Back face
		else if (tNear.x < tNear.y && tNear.x < tNear.z)
			normal = vec3(1.0, 0.0, 0.0); // Right face
		else if (tNear.y < tNear.x && tNear.y < tNear.z)
			normal = vec3(0.0, 1.0, 0.0); // Top face
		else if (tNear.z < tNear.x && tNear.z < tNear.y)
			normal = vec3(0.0, 0.0, 1.0); // Front face

		hitInfo.normal = normal;
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

	return hitInfo;
}

vec3 Trace(Ray ray, inout uint rngState)
{
	vec3 incomingLight = vec3(0.1);
	vec3 rayColor = vec3(1);

	for (int i = 0; i < maxBounceCount; i++)
	{
		HitInfo hitInfo = CalculateRayCollision(ray);

		if (hitInfo.hit)
		{
			ray.origin = hitInfo.hitPoint;
			vec3 diffuseDirection = normalize(hitInfo.normal + RandomDirection(rngState));
			vec3 specularDirection = reflect(ray.direction, hitInfo.normal);
			ray.direction = mix(diffuseDirection, specularDirection, hitInfo.material.smoothness);

			vec3 emittedLight = hitInfo.material.emissiveColor * hitInfo.material.emissiveStrength;
			incomingLight += emittedLight * rayColor;
			rayColor *= hitInfo.material.color;
		}
		else
		{
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

	Ray ray;
	ray.origin = cameraPosition;
	ray.direction = normalize(worldPosition - ray.origin);

	uint pixelIndex = uint(gl_FragCoord.y * screenSize.x + gl_FragCoord.x);
	uint rngState = pixelIndex + frameCount * 719393;

	vec3 totalIncomingLight = vec3(0);
	for (int i = 0; i < numberRaysPerPixel; i++)
	{
		totalIncomingLight += Trace(ray, rngState);
	}

	FragColor = vec4(totalIncomingLight / numberRaysPerPixel, 1);
}