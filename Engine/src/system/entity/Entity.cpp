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

    EntityManager* manager = EntityManager::Get();
    if (manager == nullptr)
    {
        throw std::runtime_error("FATAL: try to create entity before the manager is initialized!");
    }

    manager->RegisterEntity(this);
}

Entity::~Entity()
{
    if (EntityManager::Get())
    {
        EntityManager::Get()->UnregisterEntity(this);
    }
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

    Editor::Get()->GetOutlineBuffer(0)->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Outliner::OutlineShader->Use();
    // setup stencil buffer for outline
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    Editor::Get()->RenderCamera(Outliner::OutlineShader);
    transform->Compute(Outliner::OutlineShader);
    
	model->ComputeOutline(Outliner::OutlineShader);

    // dillate outline
    Editor::Get()->GetOutlineBuffer(1)->Bind();
    Outliner::OutlineDilatingShader->Use();
    Outliner::OutlineDilatingShader->SetFloat("radius", 2.5f);
    Outliner::OutlineDilatingShader->SetInt("screenTexture", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Editor::Get()->GetOutlineBuffer(0)->GetFrameTexture());
    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // bind to main frame buffer
    Editor::Get()->GetSceneBuffer()->Bind();

    Outliner::OutlineBlitShader->SetWorkSize(glm::uvec2(SRC_WIDTH, SRC_HEIGHT));

    Outliner::OutlineBlitShader->Use();
    Outliner::OutlineBlitShader->SetTextures(Editor::Get()->GetSceneBuffer()->GetFrameTexture()
                                            , Editor::Get()->GetOutlineBuffer(1)->GetFrameTexture());
    Outliner::OutlineBlitShader->Dispatch(glm::uvec2(16, 8));
    Outliner::OutlineBlitShader->Wait();

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

#pragma endregion