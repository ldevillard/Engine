#version 430 core

out vec4 FragColor;

uniform vec2 screenSize;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 cameraPosition;

struct Sphere
{
	vec3 position;
	float radius;
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
};

HitInfo RaySphere(Ray ray, vec3 sphereCenter, float sphereRadius)
{
	HitInfo hitInfo;
	hitInfo.hit = false;

	vec3 offsetRayOrigin = ray.origin - sphereCenter;

	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(offsetRayOrigin, ray.direction);
	float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4 * a * c;

	// >= 0 mean intersection
	if (discriminant >= 0)
	{
		float dst = (-b - sqrt(discriminant)) / (2 * a);

		// ignore intersections that occur behind the ray
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

void main()
{
	Sphere sphere;
	sphere.radius = 1;
	sphere.position = vec3(0, 0, 0);

	vec2 fragCoordNorm = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;

	vec4 clipCoord = vec4(fragCoordNorm, 1.0, 1.0);
	vec4 viewCoord = invProjection * clipCoord;
	viewCoord.z = -1.0;

	vec4 worldCoord = invView * viewCoord;

	vec3 worldPosition = worldCoord.xyz;

	Ray ray;
	ray.origin = cameraPosition;
	ray.direction = normalize(worldPosition);

	HitInfo hitInfo = RaySphere(ray, sphere.position, sphere.radius);
	if (hitInfo.hit)
	{
		// * 0.5 + 0.5 for pastel tint
		vec3 color = normalize(hitInfo.normal) * 0.5f + 0.5f;
		FragColor = vec4(color, 1.0);
	}
	else
	{
		FragColor = vec4(0.1, 0.1, 0.1, 1.0);
	}
}