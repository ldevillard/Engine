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
   // if (this == Editor::Get()->GetSelectedEntity()) 
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
    //else
    {
    //    glStencilMask(0x00);
    }

    transform->Compute(shader);
    
    for (Component* c : components)
        c->Compute();
    
    editorCollider->ApplyTransform(*transform);
}

void Entity::ComputeOutline() const
{
    //Editor::Get()->GetOutlineBuffer(1)->Unbind();
    //Editor::Get()->GetSceneBuffer()->Unbind();
    Editor::Get()->GetOutlineBuffer(0)->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Outliner::OutlineShader->Use();
    // setup stencil buffer for outline
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    Editor::Get()->RenderCamera(Outliner::OutlineShader);
    transform->Compute(Outliner::OutlineShader);
    
    Model* model = nullptr;
    if (TryGetComponent<Model>(model))
    {
		model->ComputeOutline(Outliner::OutlineShader);
	}

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    //Editor::Get()->GetOutlineBuffer(0)->Unbind();

    Editor::Get()->GetOutlineBuffer(1)->Bind();
    //glBindFramebuffer(GL_FRAMEBUFFER, targetB.fbo);
    //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glStencilFunc(GL_ALWAYS, 1, 0xFF);
    //glStencilMask(0xFF);
    Outliner::OutlineDilatingShader->Use();
    Outliner::OutlineDilatingShader->SetFloat("radius", 3.f);
    Outliner::OutlineDilatingShader->SetFloat("gridX", 1.F / SCR_WIDTH);
    Outliner::OutlineDilatingShader->SetFloat("gridY", 1.F / SCR_HEIGHT);
    Outliner::OutlineDilatingShader->SetInt("screenTexture", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Editor::Get()->GetOutlineBuffer(0)->GetFrameTexture());
    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glStencilFunc(GL_ALWAYS, 0, 0xFF);

    // bind to main frame buffer
    Editor::Get()->GetSceneBuffer()->Bind();

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // blit target B to the default frame buffer
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    Outliner::OutlineBlitShader->Use();
    Outliner::OutlineBlitShader->SetInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Editor::Get()->GetOutlineBuffer(1)->GetFrameTexture());
    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //Editor::Get()->GetOutlineBuffer(1)->Unbind();

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

#pragma endregion