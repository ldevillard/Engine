#pragma once

class Entity;
class EditorCollider;
class Shader;
class Transform;

class Component
{
public:
	virtual void SetEditorCollider(EditorCollider* cl);
	void SetTransform(Transform* tr);
	void SetEntity(Entity* en);
	void SetShader(Shader* sh);

	virtual void Compute() = 0;
	virtual Component* Clone() = 0;
	
	Transform* transform		   = nullptr;
	Entity*	entity				   = nullptr;

protected:
	EditorCollider* editorCollider = nullptr;
	Shader*	shader				   = nullptr;
};