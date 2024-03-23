#include "camera.h"

void Camera::OnUpdate()
{
    m_View = GetViewMatrix();
    m_Proj = glm::perspective(glm::radians(45.0f), m_AspectRatio, 0.01f, 1000.0f);
    m_ViewProjMatrices.SetBufferSubData(0, m_Proj);
    m_ViewProjMatrices.SetBufferSubData(sizeof(glm::mat4), m_View);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(const CameraMovement direction, const float deltaTime)
{
    const float velocity = m_MovementSpeed * deltaTime;
    if (direction == FORWARD)
        m_Position += (m_CanFly ? m_Front : glm::normalize(glm::vec3(m_Front.x, 0.0, m_Front.z))) * velocity;
    if (direction == BACKWARD)
        m_Position -= (m_CanFly ? m_Front : glm::normalize(glm::vec3(m_Front.x, 0.0, m_Front.z))) * velocity;
    if (direction == LEFT)
        m_Position -= (m_CanFly ? m_Right : glm::normalize(glm::vec3(m_Right.x, 0.0, m_Right.z))) * velocity;
    if (direction == RIGHT)
        m_Position += (m_CanFly ? m_Right : glm::normalize(glm::vec3(m_Right.x, 0.0, m_Right.z))) * velocity;
    if (direction == TOGGLE_FLY)
        m_CanFly = !m_CanFly;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch)
{
    xOffset *= m_MouseSensitivity;
    yOffset *= m_MouseSensitivity;

    m_Yaw += xOffset;
    m_Pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front = glm::vec3(0.0f);
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
