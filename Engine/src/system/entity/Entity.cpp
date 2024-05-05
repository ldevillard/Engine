#include <stdexcept>
#include <iostream>
#include <maths/glm/gtc/quaternion.hpp>

#include "system/editor/Editor.h"
#include "system/editor/Outliner.h"
#include "system/entity/Entity.h"
#include "system/entity/EntityManager.h"

#pragma region Public Methods

Entity::Entity(const std::string &name, Shader* sh) :
    shader(sh),
    Name(name)
{
	transform = new Transform();
    editorCollider = new EditorCollider(this);
}

Entity::~Entity()
{
	delete transform;
    delete editorCollider;
}

void Entity::AddComponent(Component* component)
{
    component->SetEditorCollider(editorCollider);
    component->SetTransform(transform);
    component->SetEntity(this);
    component->SetShader(shader);
    components.push_back(component);

    EntityManager::Get()->UpdateLightsIndex();
}

const std::vector<Component*>& Entity::GetComponents() const
{
	return components;
}

const EditorCollider* Entity::GetEditorCollider() const
{
	return editorCollider;
}

bool Entity::IsSelectedEntity() const
{
	return Editor::Get()->GetSelectedEntity() == this;
}

void Entity::Compute()
{
    if (this == Editor::Get()->GetSelectedEntity()) 
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
    else
    {
        glStencilMask(0x00);
    }

    transform->Compute(shader);
    
    for (Component* c : components)
        c->Compute();
    
    editorCollider->ApplyTransform(*transform);
}

void Entity::ComputeOutline() const
{
    Model* model = nullptr;
    if (!TryGetComponent<Model>(model))
    {
        return;
    }

    // setup stencil buffer and depth buffer for outline
    Outliner::Setup();

    // render model
    Editor::Get()->RenderCamera(Outliner::OutlineShader);
    transform->Compute(Outliner::OutlineShader);
    
	 model->ComputeOutline(Outliner::OutlineShader);

    Outliner::Draw();

    // reset them
    Outliner::Reset();
}

#pragma endregion