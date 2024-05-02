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

uniform int dataCount;

layout(std430, binding = 0) buffer data
{
	Sphere spheres[];
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

HitInfo CalculateRayCollision(Ray ray)
{
	HitInfo hitInfo;
	hitInfo.hit = false;
	hitInfo.distance = 1.0 / 0.0; // infinity
	hitInfo.material.color = vec3(0.1, 0.1, 0.1);

	for (int i = 0; i < dataCount; i++)
	{
		Sphere sphere = spheres[i];

		HitInfo hit = RaySphere(ray, sphere.position, sphere.radius);

		if (hit.hit && hit.distance < hitInfo.distance)
		{
			hitInfo = hit;
			hitInfo.material = sphere.material;
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