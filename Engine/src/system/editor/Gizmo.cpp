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

	initCubeInstanceVBO();
}

void Gizmo::ReleaseGizmos()
{
	// release resources
	gizmos.clear();
	if (cubeInstanceVBO)
	{
		glDeleteBuffers(1, &cubeInstanceVBO);
		cubeInstanceVBO = 0;
	}
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
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	// check if gizmo is enabled and if there are transforms to draw
	if (!Editor::Get().GetSettings().Gizmo || transforms.size() == 0) 
	{
		return;
	}
	
	shader->Use();
	// get camera matrices
	const EditorCamera* camera = Editor::Get().GetCamera();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCENE_WIDTH / (float)SCENE_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera->GetViewMatrix();

	// set shader uniforms
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetMat4("model", glm::mat4(1.0f));
	shader->SetVec3("color", color.Value);
	// enable instance rendering
	shader->SetBool("instanceEnabled", true);

	// convert transforms to matrices
	auto getMatrices = [](const std::vector<Transform>& transforms) -> std::vector<glm::mat4> 
	{
		std::vector<glm::mat4> matrices;
		matrices.reserve(transforms.size());

		for (const Transform& tr : transforms)
		{
			matrices.push_back(tr.GetTransformMatrix());
		}
		
		return matrices;
	};

	std::vector<glm::mat4> models = getMatrices(transforms);

	// update instance VBO with new instance matrices
	glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// bind VAO then draw
	glBindVertexArray(gizmos[WireCubeGizmo]->GetVAO());
	glLineWidth(Gizmo::GIZMO_WIDTH);
	// draw instances
	glDrawElementsInstanced(GL_LINES, static_cast<GLsizei>(gizmos[WireCubeGizmo]->Indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(models.size()));
	glLineWidth(1); // reset to default
	glBindVertexArray(0);
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

	// get camera matrices
	const EditorCamera* camera = Editor::Get().GetCamera();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCENE_WIDTH / (float)SCENE_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera->GetViewMatrix();

	// compute the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.Position);
	// get the rotation quaternion
	glm::quat rotationQuaternion = transform.GetRotationQuaternion();
	// convert the quaternion to a rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
	// apply the rotation matrix to the model matrix
	model *= rotationMatrix;
	model = glm::scale(model, transform.Scale);

	// set shader uniforms
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetMat4("model", model);
	shader->SetVec3("color", color.Value);
	shader->SetBool("instanceEnabled", false);
}

void Gizmo::initCubeInstanceVBO()
{
	// create vbo for storing instance data (instance VBO)
	glGenBuffers(1, &cubeInstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO);

	// allocate memory for the instance buffer, but do not populate it yet
	// the buffer will store transformation matrices (glm::mat4) for each instance
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the buffer

	// bind the VAO of the wire cube gizmo
	glBindVertexArray(gizmos[WireCubeGizmo]->GetVAO());
	glBindBuffer(GL_ARRAY_BUFFER, cubeInstanceVBO); // bind the instance buffer to the VAO

	// size of a vec4 (4 floats)
	std::size_t vec4Size = sizeof(glm::vec4);
	GLsizei mat4Size = static_cast<GLsizei>(sizeof(glm::mat4));

	// config vertex attribute 3 (first column of the mat4)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);

	// config vertex attribute 4 (second column of the mat4)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));

	// config vertex attribute 5 (third column of the mat4)
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));

	// config vertex attribute 6 (fourth column of the mat4)
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

	// set the divisor for each attribute to 1, indicating that these attributes
	// will change per instance (not per vertex)
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	// unbind vao
	glBindVertexArray(0);
}

#pragma endregion