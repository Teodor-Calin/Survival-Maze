#pragma once

#include "lab_m1/Tema2/Projectile3D.h"
#include "lab_m1/Tema2/CubeObstacle.h"
#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "lab_m1/Tema2/Enemy3DExplosion.h"

#define Z_FAR		(200.f)
#define Z_NEAR		(.01f)

#define RED     glm::vec3(1, 0, 0)
#define GREEN   glm::vec3(0, 1, 0)
#define BLUE    glm::vec3(0, 0, 1)
#define NAVY_BLUE    glm::vec3(0, 0, 0.5)
#define AVATAR_BLUE    glm::vec3(0, 0.4, 1)
#define YELLOW  glm::vec3(1, 1, 0)
#define WHITE   glm::vec3(1, 1, 1)
#define GREY    glm::vec3(0.45, 0.45, 0.45)
#define GRASS_COLOR    glm::vec3(0.49, 0.78, 0.31)
#define BLACK   glm::vec3(0, 0, 0)
#define DARKER_SKIN_COLOR glm::vec3(0.8, 0.5, 0)
#define SKIN_COLOR glm::vec3(1, 0.7, 0)
#define BAG_COLOR glm::vec3(0.3, 0.1, 0)

#define PLAYER_HITBOX_RADIUS 0.6
#define BULLET_SPEED 10.f
#define MAX_BULLET_DISTANCE 20.f

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void GenerateMaze();
        glm::vec3 ComputeNextPosition(float distance, char direction);

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;

        float right = 10.f, left = 0.01f, bottom = 0.01f, top = 10.f, fov = 40.f;
        bool perspectiveBool = true;
        
        int directionsX[4] = { -1, 0, 1, 0 };
        int directionsY[4] = { 0, 1, 0, -1 };

        int finishX, finishY;
        float finish_center_X, finish_center_Y;
        int m[13][13];

        float playerX, playerZ;
        float playerLookAngleY = 0;
        float playerFirstPersonLookAngleX = 0;

        float time_until_fire = 0;
        float hp = 5;
        float invincibility_time = 0;
        float time_left = 50;

        bool firstPerson = false;
        bool gameOn = true;

        vector<CubeObstacle> cubes;
        

    };
}   // namespace m1
