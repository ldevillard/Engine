#include "physics/Physics.h"

#include <cassert>

#include "system/entity/EntityManager.h"

namespace Physics
{
	bool EditorRaycast(const Ray& ray, RaycastHit& outRayCastHit)
	{
		std::vector<Entity*> entities = EntityManager::Get()->GetEntities();

		for (Entity* e : entities)
		{
			const EditorCollider* collider = e->GetEditorCollider();
			
			assert(collider != nullptr); // Entity has no editor collider -> mouse picking purpose

			collider->IntersectRay(ray, outRayCastHit);
		}
		return outRayCastHit.hit;
	}
}