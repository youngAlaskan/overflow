#pragma once

#include <cmath>

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "../input.h"
#include "../glObjects.h"

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // constructor with vectors
    Camera() { UpdateCameraVectors(); }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

    void OnUpdate();

    // processes input received from any keyboard-like input system. Accepts an InputKeyActions struct as input (to abstract it from windowing systems)
    void ProcessKeyInput(const InputKeyActions input, const float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch = true);

    void ProcessMouseScroll(const float yOffset)
    {
        // multiplicative, so every wheel notch feels the same at any zoom level
        m_Zoom = glm::clamp(m_Zoom * std::pow(1.1f, yOffset), kMinZoom, kMaxZoom);
    }

    float GetEffectiveFov() const;
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
    float m_TiltPitch = 0.0f;
    float m_Roll = 0.0f;
    
    // Camera options
    
    float m_MovementSpeed    = 6.0f;
    float m_MouseSensitivity = 0.1f;
    bool m_CanFly            = true;
    static constexpr float kMinFov = 20.0f, kMaxFov = 120.0f;
    static constexpr float kMinZoom = 1.0f, kMaxZoom = 20.0f;
    float m_Fov = 45.0f;   // base vertical FOV in degrees (the slider)
    float m_Zoom = 1.0f;    // magnification (the scroll wheel)

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};
