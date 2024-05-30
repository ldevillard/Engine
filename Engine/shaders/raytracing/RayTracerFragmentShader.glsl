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

const int checkerPattern = 1;
const int hideEmissive = 2;

struct Material
{
	vec3 color;
	vec3 emissiveColor;
	int flag;
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
	mat4 inverseTransform;
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

vec3 ComputeCubeNormal(vec3 i, vec3 bmin, vec3 bmax)
{
    float epsilon = 0.001;

    float cx = abs(i.x - bmin.x);
    float fx = abs(i.x - bmax.x);
    float cy = abs(i.y - bmin.y);
    float fy = abs(i.y - bmax.y);
    float cz = abs(i.z - bmin.z);
    float fz = abs(i.z - bmax.z);

    if(cx < epsilon)
        return vec3(-1.0, 0.0, 0.0);
    else if (fx < epsilon)
        return vec3(1.0, 0.0, 0.0);
    else if (cy < epsilon)
        return vec3(0.0, -1.0, 0.0);
    else if (fy < epsilon)
        return vec3(0.0, 1.0, 0.0);
    else if (cz < epsilon)
        return vec3(0.0, 0.0, -1.0);
    else if (fz < epsilon)
        return vec3(0.0, 0.0, 1.0);
    
    return vec3(0.0, 0.0, 0.0);
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
		vec3 normal = ComputeCubeNormal(hitPointLocal, bmin, bmax);
		hitInfo.normal = normalize(vec3((cube.transform * vec4(normal, 0.0)).xyz));
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
			ray.direction = normalize(mix(diffuseDirection, specularDirection, material.smoothness));

			vec3 emittedLight = material.emissiveColor * material.emissiveStrength;
			incomingLight += emittedLight * rayColor;
			rayColor *= material.color;
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