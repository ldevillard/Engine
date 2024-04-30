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

struct Material
{
	vec3 color;
};

struct ObjectData
{
	Sphere sphere;
	Material material;
};

uniform int dataCount;

layout(std430, binding = 0) buffer data
{
	ObjectData objectData[];
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
	
	// origin centered sphere equation x2 + y2 + z2 = r2
	// see https://raytracing.github.io/books/RayTracingInOneWeekend.html
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
	sphere.radius = objectData[0].sphere.radius;
	sphere.position = objectData[0].sphere.position;

	vec2 fragCoordNorm = (gl_FragCoord.xy / screenSize) * 2.0 - 1.0;

	vec4 clipCoord = vec4(fragCoordNorm, 0.0, 1.0);
	vec4 viewCoord = invProjection * clipCoord;
	viewCoord.z = -1.0;

	vec4 worldCoord = invView * viewCoord;

	vec3 worldPosition = worldCoord.xyz / worldCoord.w;

	Ray ray;
	ray.origin = cameraPosition;
	ray.direction = normalize(worldPosition - ray.origin);

	HitInfo hitInfo;
	hitInfo.hit = false;
	hitInfo.distance = 1.0 / 0.0; // infinity

	vec3 color = vec3(0.1, 0.1, 0.1);

	for (int i = 0; i < dataCount; i++)
	{
		Sphere sphere = objectData[i].sphere;
		Material material = objectData[i].material;

		HitInfo hit = RaySphere(ray, sphere.position, sphere.radius);

		if (hit.hit && hit.distance < hitInfo.distance)
		{
			hitInfo = hit;
			color = material.color;
		}
	}

	FragColor = vec4(color, 1.0);
}