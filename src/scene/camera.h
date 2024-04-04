#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "../glObjects.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    TOGGLE_FLY
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // constructor with vectors
    Camera() { UpdateCameraVectors(); }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

    void OnUpdate();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(const CameraMovement direction, const float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch = true);

    void ProcessMouseScroll(const float yOffset)
    {
        //m_MovementSpeed += 0.001f * yOffset;
        //if (m_MovementSpeed < 0.0f)
        //    m_MovementSpeed = 0.0f;
    }
public:
    // Camera attributes
    
    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Front    = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up       = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 m_Right    = glm::vec3(1.0f, 0.0f,  0.0f);
    glm::vec3 m_WorldUp  = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::mat4 m_View     = glm::mat4(0.0f);
    glm::mat4 m_Proj     = glm::mat4(0.0f);
    float m_AspectRatio  = 0.0f;
    UBO m_ViewProjMatrices = UBO("Matrices");
    
    // Euler angles

    float m_Yaw   = -90.0f;
    float m_Pitch = 0.0f;
    
    // Camera options
    
    float m_MovementSpeed    = 0.05f;
    float m_MouseSensitivity = 0.1f;
    bool m_CanFly            = true;

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};
