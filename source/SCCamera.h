#pragma once

#include "Core-pch.hpp"
#include "SCRendererInterface.h"
#include "SCSettings.h"
#include "glMatrix4x4.hpp"
#include "SCGLFWDisplay.h"

namespace sc {

    class SCCamera final {

    public:

        float *LookAt;

        void AddShake(float force)
        {
            shake = std::max(force, shake);
        }

        SCCamera(const std::shared_ptr<SCRendererInterface> &renderer,
                 float cx, float cy, float cz)
        : LookAt(nullptr)
        , _settings(renderer->getSettings())
        , _renderer(renderer)
        {
            cameraRoatation.id();

            C[0] = cx;
            C[1] = cy;
            C[2] = cz;

            angle_y = 0.0;
            angle_x = -0.1f;

            speed = 0.5f;
            shake = 0.0f;

            memset(Sr, 0, 12);
            memset(S, 0, 12);
            memset(E, 0, 12);

            k_v3fLookAtPos = xVec3(0.0, 1.3, 0.0);
            k_v3LookFromPos = xVec3(0.0, 80 /*THIS IS CAM HEIGHT!!!*/, 0.0);

            s_fFacing = 1.0;
            yPosAddition = 0;
            gEye = xVec3(C);
            gDir = xVec3(1, 0, 0);
        }

        void Apply()
        {
            shake *= 0.75;

            for (int i = 0; i < 3; ++i)
            {
                Sr[i] = (rand() % 5 - 3) * shake;
                S[i] += (Sr[i] - S[i]) * 0.3f;
            }

            if (LookAt)
            {
                static xVec3 v3Position = xVec3(0, 0, 0);
                static xVec3 v3LookDirection = xVec3(0, 0, 0);
                static xVec3 vLastPosDir;

                v3Position = xVec3(LookAt[0], LookAt[1], LookAt[2]);

                float CameraDistance = 60.0;

                xVec3 v3LookFromPos;

                vLastPosDir = v3Position + xVec3(2.0, 0.0, 0.0);
                v3LookDirection = v3Position - vLastPosDir;

                float fMag = v3LookDirection.magnitude();

                if (fMag > 0.0f) {

                    v3LookDirection /= fMag;
                }

                if (!_settings->usesSecondCam()) {

                    cameraRoatation.multiply(v3LookDirection, v3LookDirection);
                }

                if (v3LookDirection.y > 0.3f) v3LookDirection.y = 0.3f;
                if (v3LookDirection.y < -0.3f) v3LookDirection.y = -0.3f;

                v3LookFromPos = v3Position - v3LookDirection * CameraDistance;

                if (fabs(v3LookDirection.y) <= 0.99f)
                {
                    k_v3LookFromPos.y = 80 + yPosAddition;
                    v3LookFromPos += k_v3LookFromPos;

                    xVec3 v3LookAtPos = (v3Position + k_v3fLookAtPos);

                    xVec3 Ray = v3LookFromPos - v3LookAtPos;
                    xVec3 d(Ray);
                    d.normalize();

                    gDir = -d;
                    gDir.normalize();
                }

                E[0] = v3LookFromPos.x + S[0];
                E[1] = v3LookFromPos.y + S[1];
                E[2] = v3LookFromPos.z + S[2];

                gEye = xVec3(E);

                auto vUp = xVec3(0, 1, 0);

                _renderer->ModelView.lookAt(gEye, gDir, vUp);

            }
            else
            {
                auto vUp = xVec3(0, 1, 0);
                auto vC = xVec3(C);

                _renderer->ModelView.lookAt(vC, gEye, vUp);
            }
        }

        void FreeCam(const std::shared_ptr<SCDisplay> &display)
        {
            static xVec3 gViewY = xVec3(0.0);

            float add = 0.05f;

            auto window = static_cast<GLFWwindow*>(display->getContext());

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) add = 0.2f;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) add = 0.02f;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) gEye += gDir*add;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) gEye -= gDir*add;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) gEye -= gViewY*add;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) gEye += gViewY*add;

            int MouseX, MouseY;
            static int  tx = 0,
            ty = 0;
            int dx,dy;

            double x,y;

            glfwGetCursorPos(window, &x, &y);

            MouseX = x;
            MouseY = y;

            if((tx != MouseX) || (ty != MouseY))
            {
                dx = tx - MouseX;
                dy = ty - MouseY;

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    gDir.normalize();
                    gViewY.cross(gDir, xVec3(0,1,0));

                    xQuat qx(xPiF32 * dx * 20/ 180.0f, xVec3(0,1,0));
                    qx.rotate(gDir);
                    xQuat qy(xPiF32 * dy * 20/ 180.0f, gViewY);
                    qy.rotate(gDir);
                }

                tx = MouseX;
                ty = MouseY;
            }

            auto gUp = xVec3(0, 1, 0);
            auto dir = gDir;

            _renderer->ModelView.lookAt(gEye, dir, gUp);
        }


        void GoForward(float dir = 1.0f)
        {

            for (int i = 0; i < 3; ++i) {

                C[i] += (E[i] - C[i]) * speed * dir;
            }
        }

        void GoPlane(float add)
        {

            C[0] -= cos(angle_y + add) * speed;
            C[2] -= sin(angle_y + add) * speed;
        }

    public:

        xMat33 cameraRoatation;
        float yPosAddition;
        xVec3 gDir;
        xVec3 gEye;

    private:

        std::shared_ptr<SCSettings> _settings;
        std::shared_ptr<SCRendererInterface> _renderer;

        float E[3];
        float C[3];

        float angle_y;
        float angle_x;

        float speed;
        float shake;

        float S[3], Sr[3];

        float  s_fFacing;
        xVec3 k_v3fLookAtPos;
        xVec3 k_v3LookFromPos;

    };

}

