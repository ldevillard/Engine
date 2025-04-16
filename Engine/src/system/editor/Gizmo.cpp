#include "system/editor/Gizmo.h"
#include "system/editor/Editor.h"

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

// mesh includes
#include "data/mesh/WireCube.h"
#include "data/mesh/WireSphere.h"
#include "data/mesh/WireCone.h"
#include "data/mesh/WireConeFrustum.h"

// initialize static variables
std::map<GizmoType, std::unique_ptr<Mesh>> Gizmo::gizmos;
Shader* Gizmo::shader = nullptr;
unsigned int Gizmo::cubeInstanceVBO = 0;

#pragma region Public Methods

void Gizmo::InitGizmos(Shader* s)
{
	shader = s;
	gizmos[WireCubeGizmo] = std::make_unique<WireCube>();
	gizmos[WireSphereGizmo] = std::make_unique<WireSphere>();
	gizmos[WireConeGizmo] = std::make_unique<WireCone>();
	gizmos[WireConeFrustumGizmo] = std::make_unique<WireConeFrustum>();

	initInstanceVBO();
}

// Need to make a function for shader binding
void Gizmo::DrawWireCube(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	if (!Editor::Get().GetSettings().Gizmo)
		return;

	bindShader(color, transform);

	gizmos[WireCubeGizmo]->Draw(shader);
}

void Gizmo::DrawWireCubeInstanced(const Color& color, const std::vector<Transform>& transforms)
{
	//TODO

	//if (!shader)
	//{
	//	std::cerr << "Gizmo shader is not initialized" << std::endl;
	//	return;
	//}
	//if (!Editor::Get().GetSettings().Gizmo)
	//	return;
	//
	//shader->Use();
	//const EditorCamera* camera = Editor::Get().GetCamera();
	//glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCENE_WIDTH / (float)SCENE_HEIGHT, 0.1f, 1000.0f);
	//glm::mat4 view = camera->GetViewMatrix();
	//shader->SetMat4("projection", projection);
	//shader->SetMat4("view", view);
	//shader->SetMat4("model", glm::mat4(1));
	//
	//gizmos[WireCubeGizmo]->DrawInstanced(shader, transforms);
}

void Gizmo::DrawWireSphere(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	if (!Editor::Get().GetSettings().Gizmo)
		return;

	bindShader(color, transform);

	gizmos[WireSphereGizmo]->Draw(shader);
}

void Gizmo::DrawWireCone(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	if (!Editor::Get().GetSettings().Gizmo)
		return;

	bindShader(color, transform);

	gizmos[WireConeGizmo]->Draw(shader);
}

void Gizmo::DrawArrow(const Color& color, Transform transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	if (!Editor::Get().GetSettings().Gizmo)
		return;

	Transform tr(transform.Position, transform.Rotation, glm::vec3(0.001f, 0.001f, .5f));
	DrawWireCube(color, tr);

	// rotate 90 degrees around the x-axis
	transform.Rotation.x += -90.0f;
	transform.Position = tr.Position + tr.GetForwardVector() * 0.5f;
	DrawWireCone(color, transform);
}

void Gizmo::DrawWireConeFrustum(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	if (!Editor::Get().GetSettings().Gizmo)
		return;

	Transform tr(transform);
	tr.Rotation.x += -90.0f;
	tr.Scale = glm::vec3(0.5f);

	bindShader(color, tr);

	gizmos[WireConeFrustumGizmo]->Draw(shader);
}

#pragma endregion

#pragma region Private Methods

void Gizmo::bindShader(const Color& color, const Transform& transform)
{
	shader->Use();

	const EditorCamera* camera = Editor::Get().GetCamera();

	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCENE_WIDTH / (float)SCENE_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera->GetViewMatrix();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.Position);

	// get the rotation quaternion
	glm::quat rotationQuaternion = transform.GetRotationQuaternion();
	// convert the quaternion to a rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
	// apply the rotation matrix to the model matrix
	model *= rotationMatrix;

	model = glm::scale(model, transform.Scale);

	shader->SetMat4("model", model);

	shader->SetVec3("color", color.Value);
}

void Gizmo::initInstanceVBO()
{
	// create instance VBO
	glGenBuffers(1, &cubeInstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(gizmos[WireCubeGizmo]->GetVAO());
	glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);

	// instances transforms
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);
}

#pragma endregion