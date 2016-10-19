#include "stdafx.h"
#include "Camera.h"

#include "Transform.h"
#include "Planet.h"

Camera::Camera() :
	m_FOV(45),
	m_NearPlane(0.01f),
	m_FarPlane(30.0f),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	m_Projection = glm::mat4();
	m_View = glm::mat4();
	m_ViewInverse = glm::mat4();
	m_ViewProjection = glm::mat4();
	m_ViewProjectionInverse = glm::mat4();

	m_pTransform = new Transform();
	m_pTransform->SetPosition(0, 0, -6);
}

void Camera::Update()
{
	m_Moved = false; 
	if (INPUT->IsMouseButtonDown(1))
	{
		glm::vec2 look = INPUT->GetMouseMovement();
		look.x *= -m_RotationSpeed * TIME->DeltaTime();
		look.y *= -m_RotationSpeed * TIME->DeltaTime();
		m_pTransform->SetRotation(glm::rotate(m_pTransform->GetRotation(), look.y, glm::vec3(1, 0, 0)));
		m_pTransform->SetRotation(glm::rotate(m_pTransform->GetRotation(), look.x, glm::vec3(0, 1, 0)));
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_4))
	{
		m_FOV -= 30.f * TIME->DeltaTime();
		m_Moved = true;
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_6))
	{
		m_FOV += 30.f * TIME->DeltaTime();
		m_Moved = true;
	}
	if (INPUT->IsKeyboardKeyDown('w'))
	{
		m_Altitude -= ((m_Altitude*2.0f) - m_Altitude)*TIME->DeltaTime();
		m_Moved = true;
	}
	if (INPUT->IsKeyboardKeyDown('s'))
	{
		m_Altitude += ((m_Altitude*2.0f) - m_Altitude)*TIME->DeltaTime();
		m_Moved = true;
	}
	float deltaLong = m_Longitude;
	if (INPUT->IsKeyboardKeyDown('a'))
	{
		m_Longitude -= 0.5f*TIME->DeltaTime();
		m_Moved = true;
	}
	if (INPUT->IsKeyboardKeyDown('d'))
	{
		m_Longitude += 0.5f * TIME->DeltaTime();
		m_Moved = true;
	}
	deltaLong -= m_Longitude;
	m_pTransform->SetRotation(glm::rotate(m_pTransform->GetRotation(), deltaLong, glm::vec3(0, 1, 0)));

	float dist = m_pPlanet->GetRadius() + m_Altitude;


	if(m_Moved)m_pTransform->SetPosition(sin(m_Longitude)*dist, 0, -cos(m_Longitude)*dist);

	m_pTransform->UpdateTransforms();

	if (m_PerspectiveProjection)
	{
		m_Projection = glm::perspectiveLH(glm::radians(m_FOV),
			(float)(WINDOW.Width) / (float)WINDOW.Height, m_NearPlane, m_FarPlane);
	}
	else
	{
		float viewWidth = (m_Size>0) ? m_Size * WINDOW.AspectRatio : WINDOW.Width;
		float viewHeight = (m_Size>0) ? m_Size : WINDOW.Height;
		m_Projection = glm::ortho(0.f, viewWidth, viewHeight, 0.f, m_NearPlane, m_FarPlane);
	}

	glm::vec3 worldPos = m_pTransform->GetPosition();
	glm::vec3 lookAt = worldPos + m_pTransform->GetForward();
	glm::vec3 upVec = m_pTransform->GetUp();// glm::vec3(0, 1, 0);//
	m_View = glm::lookAtLH(worldPos, lookAt, upVec);

	m_ViewInverse = glm::inverse(m_View);
	m_ViewProjection = m_Projection*m_View;
	m_ViewProjectionInverse = glm::inverse(m_ViewProjection);
}

Camera::~Camera()
{
	SafeDelete(m_pTransform);
}
