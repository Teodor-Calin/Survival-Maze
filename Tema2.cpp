#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Transform3D.h"
#include "lab_m1/Tema2/Projectile3D.h"
#include "lab_m1/Tema2/CubeObstacle.h"
#include "lab_m1/Tema2/Enemy3D.h"

#include <vector>
#include <string>
#include <iostream>
#include <stack>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

list<Projectile3D> projectiles;
list<Enemy3D> enemies;
list<Enemy3DExplosion> explosions;


void Tema2::GenerateMaze() {
    int aux, x, y;
    stack<int> sX;
    stack<int> sY;
    int count;
    bool foundFinish = false;


    for (int i = 2; i <= 10; i++) {
        for (int j = 2; j <= 10; j++) {
            m[i][j] = 0;

        }
    }
    for (int i = 0; i <= 12; i++) {
        m[0][i] = -1;
        m[11][i] = -1;
        m[i][0] = -1;
        m[i][11] = -1;
    }

    sX.push(2);
    sY.push(2);
    m[2][2] = 1;

    while (!sX.empty()) {
        x = sX.top();
        y = sY.top();

        count = 0;
        
        int d = rand() % 4;
        for (int i = 0; i < 3; i++) {
            int dir = (d + i) % 4;

            if (m[x + 2 * directionsX[dir]][y + 2 * directionsY[dir]] == 0) {
                m[x + directionsX[dir]][y + directionsY[dir]] = 1;
                m[x + 2 * directionsX[dir]][y + 2 * directionsY[dir]] = 1;
                sX.push(x + 2 * directionsX[dir]);
                sY.push(y + 2 * directionsY[dir]);
                count++;
                break;
            }
        }

        if (count == 0) {
            sX.pop();
            sY.pop();

            if (!foundFinish) {
                finishX = x;
                finishY = y;
                foundFinish = true;
            }

        }
    }

    aux = 0;
    for (int i = 1; i <= 11; i++) {
        for (int j = 1; j <= 11; j++) {
            if (m[i][j] == 1) {
                if (i == finishX && j == finishY) {
                } else {
                    aux = (aux + 1) % 3;
                    if (aux == 0) {
                        enemies.push_back(Enemy3D((i + j) % 2, glm::vec3(5 * i - 2.5f, 1, 5 * j - 2.5f)));
                    }
                }   
            }
            else {
                cubes.push_back(CubeObstacle(5 * i - 5, 5 * i, 5 * j - 5, 5 * j));
            }
        }
    }

}


glm::vec3 Tema2::ComputeNextPosition(float distance, char direction) {
    glm::vec3 pos = glm::vec3(playerX, 0, playerZ);
    glm::vec3 dir;
    if (direction == 'W') {
        dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
    }
    else if (direction == 'R') {
        dir = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
    }
    
    pos += dir * distance;

    return pos;
}


void Tema2::Init()
{
    camera = new implemented::Camera();
    camera->Set(glm::vec3(7.5f, 2.5f, 5.5f), glm::vec3(7.5f, 2, 8.5f), glm::vec3(0, 1, 0));

    //MESHES

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);

    {
        Shader* shader = new Shader("ColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* defshader = new Shader("DeformationShader");
        defshader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderDeformation.glsl"), GL_VERTEX_SHADER);
        defshader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderDeformation.glsl"), GL_FRAGMENT_SHADER);
        defshader->CreateAndLink();
        shaders[defshader->GetName()] = defshader;
    }

    GenerateMaze();
}


void Tema2::FrameStart()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // time up
    if (time_left <= 0) {

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, RED);

        camera->Set(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));

        if (gameOn == true) {
            gameOn = false;
            cout << "\n\n\nTime's Up!\n\n\n";
        }
        
        return;
    }

    // win
    if (playerX < 5 * finishX && playerX > 5 * finishX - 5 &&
            playerZ < 5 * finishY && playerZ > 5 * finishY - 5) {


        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, GREEN);

        camera->Set(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
        
        if (gameOn == true) {
            gameOn = false;
            cout << "\n\n\nYou Win!\n\n\n";
        }

        return;
    }

    // dead
    if (hp <= 0) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, RED);

        camera->Set(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
        
        if (gameOn == true) {
            gameOn = false;
            cout << "\n\n\nYou Died!\n\n\n";
        }

        return;
    }

    // fire rate
    if (time_until_fire > 0) {
        time_until_fire -= deltaTimeSeconds;
    }

    // invincibility after damage
    if (invincibility_time > 0) {
        invincibility_time -= deltaTimeSeconds;
    }
    time_left -= deltaTimeSeconds;

    glm::mat4 modelMatrix = glm::mat4(1);

    // enemies
    std::list<Enemy3D>::iterator enemy_it = enemies.begin();
    while (enemy_it != enemies.end())
    {
        enemy_it->angle += deltaTimeSeconds * 4 * enemy_it->speed;

            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, enemy_it->center_position);
            modelMatrix *= Transform3D::RotateOY(enemy_it->angle);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 2, 1));
            RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, RED);

            enemy_it->actual_pos = enemy_it->center_position + glm::vec3(2 * cos(enemy_it->angle), 0, 2 * -sin(enemy_it->angle));

            // enemy - player collision
            if (invincibility_time <= 0 &&
                distance(enemy_it->actual_pos, glm::vec3(playerX, 1, playerZ)) <= 1.1f) {

                hp--;
                invincibility_time = 0.5f;
            }

            enemy_it++;
    }

    // projectiles
    std::list<Projectile3D>::iterator it = projectiles.begin();
    while (it != projectiles.end())
    {
        it->position += it->direction * deltaTimeSeconds * BULLET_SPEED;
        it->distance += BULLET_SPEED * deltaTimeSeconds;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, it->position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
        RenderSimpleMesh(meshes["sphere"], shaders["ColorShader"], modelMatrix, BLACK);

        bool ok = true;

        // projectile - obstacle collision
        for (int i = 0; i < cubes.size(); i++) {
            if (it->position.x >= cubes[i].x1 && it->position.x <= cubes[i].x2 &&
                it->position.z >= cubes[i].z1 && it->position.z <= cubes[i].z2) {
                
                ok = false;
            }
        }

        // projectile - enemy collision
        std::list<Enemy3D>::iterator enemy_it = enemies.begin();
        while (enemy_it != enemies.end())
        {
            if (distance(it->position, enemy_it->actual_pos) <= 1) {
                
                explosions.push_back(Enemy3DExplosion(enemy_it->actual_pos));
                enemies.erase(enemy_it++);
                ok = false;

                break;
            }
            else {
                enemy_it++;
            }
            
        }
    
        // projectile - floor collision
        if (it->position.y <= 0) {
            ok = false;
        }

        // "maximum distance reached" case
        if (it->distance >= MAX_BULLET_DISTANCE) {
            ok = false;
        }

        if (ok == false) {
            projectiles.erase(it++);
        }
        else {
            it++;
        }
    }

    // explosions
    std::list<Enemy3DExplosion>::iterator expl_it = explosions.begin();
    while (expl_it != explosions.end())
    {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, expl_it->position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f * (1.8f - expl_it->time) + 0.2));
        RenderSimpleMesh(meshes["sphere"], shaders["DeformationShader"], modelMatrix, RED);

        expl_it->time -= deltaTimeSeconds;

        if (expl_it->time <= 0) {
            explosions.erase(expl_it++);
        }
        else {
            expl_it++;
        }
    }
       
    // podea
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(25, 0, 25));
    RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, GREY);
    
    // cuburi (pereti)
    for (int i = 1; i <= 11; i++) {
        for (int j = 1; j <= 11; j++) {

            if (m[i][j] != 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(5 * i - 2.5f, 2.5f, 5 * j - 2.5f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 5, 5));
                RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, NAVY_BLUE);

            }
            else {
                if (i == finishX && j == finishY) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(5 * i - 2.5f, 0.05f, 5 * j - 2.5f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 0.1, 5));
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, GREEN);
                }
                else if (i == 2 && j == 2) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(7.5f, 0.05f, 7.5f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 0.1, 5));
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, RED);
                }
            }
        }
    }

    // player position
    if (firstPerson) {
        playerX = camera->position[0];
        playerZ = camera->position[2];
    }
    else {
        playerX = camera->GetTargetPosition()[0];
        playerZ = camera->GetTargetPosition()[2];
    }

    // player
    if (!firstPerson) {
        // head
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 1.75, playerZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, SKIN_COLOR);

        // body
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 1, playerZ));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 0.6));
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, BLUE);

        // legs
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0.25, playerZ));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3, 0.5, 0.3));
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, SKIN_COLOR);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0.25, playerZ));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3, 0.5, 0.3));
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, SKIN_COLOR);

        // arms
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 1.2, playerZ));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.6, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.6, 0.2));
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, SKIN_COLOR);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 1.2, playerZ));
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.6, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.6, 0.2));
        RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix, SKIN_COLOR);
    }
   
    // hp and time bar
    projectionMatrix = glm::ortho(left, right, bottom, top, Z_NEAR, Z_FAR);
    if (!firstPerson) {
        // hp bar
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-4, 9, 0));
        modelMatrix *= Transform3D::RotateOX(M_PI / 2);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(hp / 10, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, RED);


        // time bar
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-6, 9, 0));
        modelMatrix *= Transform3D::RotateOX(M_PI / 2);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(time_left / 100, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, YELLOW);
    }
    else {

        // hp bar
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->position);
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix *= Transform3D::RotateOX(playerFirstPersonLookAngleX);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-4, 8.5, 3));
        modelMatrix *= Transform3D::RotateOX(M_PI / 2);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(hp / 10, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, RED);

        // time bar
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->position);
        modelMatrix *= Transform3D::RotateOY(playerLookAngleY);
        modelMatrix *= Transform3D::RotateOX(playerFirstPersonLookAngleX);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-6, 8.5, 3));
        modelMatrix *= Transform3D::RotateOX(M_PI / 2);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(time_left / 100, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 1, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
        RenderSimpleMesh(meshes["plane"], shaders["ColorShader"], modelMatrix, YELLOW);
    }
   
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    int modelLocation = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int viewLocation = glGetUniformLocation(shader->program, "View");
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    int projectionLocation = glGetUniformLocation(shader->program, "Projection");
    //glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    int timeLocation = glGetUniformLocation(shader->program, "CurrentTime");
    glUniform1f(timeLocation, Engine::GetElapsedTime());

    int colorLocation = glGetUniformLocation(shader->program, "Color");
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
   
        float cameraSpeed = 4.0f;
        glm::vec3 nextPos;

        // W
        if (window->KeyHold(GLFW_KEY_W) && gameOn) {
            bool ok = true;
            nextPos = ComputeNextPosition(deltaTime * cameraSpeed, 'W');

            for (int i = 0; i < cubes.size(); i++) {
                if (playerX + 0.5 >= cubes[i].x1 && playerX - 0.5 <= cubes[i].x2) {
                    if (nextPos.z + 0.5 > cubes[i].z1 && nextPos.z < cubes[i].z2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.z > cubes[i].z1 && nextPos.z - 0.5 < cubes[i].z2) {
                        ok = false;
                        break;
                    }

                }
                else if (playerZ + 0.5 >= cubes[i].z1 && playerZ - 0.5 <= cubes[i].z2) {
                    if (nextPos.x + 0.5 > cubes[i].x1 && nextPos.x < cubes[i].x2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.x > cubes[i].x1 && nextPos.x - 0.5 < cubes[i].x2) {
                        ok = false;
                        break;
                    }
                    
                }
            }

            if (ok) {
                camera->MoveForward(deltaTime * cameraSpeed);
            }
           
        }

        // A
        if (window->KeyHold(GLFW_KEY_A) && gameOn) {

            bool ok = true;
            nextPos = ComputeNextPosition(-deltaTime * cameraSpeed, 'R');

            for (int i = 0; i < cubes.size(); i++) {
                if (playerX + 0.5 >= cubes[i].x1 && playerX - 0.5 <= cubes[i].x2) {
                    if (nextPos.z + 0.5 > cubes[i].z1 && nextPos.z < cubes[i].z2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.z > cubes[i].z1 && nextPos.z - 0.5 < cubes[i].z2) {
                        ok = false;
                        break;
                    }

                }
                else if (playerZ + 0.5 >= cubes[i].z1 && playerZ - 0.5 <= cubes[i].z2) {
                    if (nextPos.x + 0.5 > cubes[i].x1 && nextPos.x < cubes[i].x2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.x > cubes[i].x1 && nextPos.x - 0.5 < cubes[i].x2) {
                        ok = false;
                        break;
                    }

                }
            }

            if (ok) {
                camera->TranslateRight(-deltaTime * cameraSpeed);
            }

        }

        // S
        if (window->KeyHold(GLFW_KEY_S) && gameOn) {

            bool ok = true;
            nextPos = ComputeNextPosition(-deltaTime * cameraSpeed, 'W');

            for (int i = 0; i < cubes.size(); i++) {
                if (playerX + 0.5 >= cubes[i].x1 && playerX - 0.5 <= cubes[i].x2) {
                    if (nextPos.z + 0.5 > cubes[i].z1 && nextPos.z < cubes[i].z2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.z > cubes[i].z1 && nextPos.z - 0.5 < cubes[i].z2) {
                        ok = false;
                        break;
                    }

                }
                else if (playerZ + 0.5 >= cubes[i].z1 && playerZ - 0.5 <= cubes[i].z2) {
                    if (nextPos.x + 0.5 > cubes[i].x1 && nextPos.x < cubes[i].x2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.x > cubes[i].x1 && nextPos.x - 0.5 < cubes[i].x2) {
                        ok = false;
                        break;
                    }

                }
            }

            if (ok) {
                camera->MoveForward(-deltaTime * cameraSpeed);
            }

        }

        // D
        if (window->KeyHold(GLFW_KEY_D) && gameOn) {

            bool ok = true;
            nextPos = ComputeNextPosition(deltaTime * cameraSpeed, 'R');

            for (int i = 0; i < cubes.size(); i++) {
                if (playerX + 0.5 >= cubes[i].x1 && playerX - 0.5 <= cubes[i].x2) {
                    if (nextPos.z + 0.5 > cubes[i].z1 && nextPos.z < cubes[i].z2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.z > cubes[i].z1 && nextPos.z - 0.5 < cubes[i].z2) {
                        ok = false;
                        break;
                    }

                }
                else if (playerZ + 0.5 >= cubes[i].z1 && playerZ - 0.5 <= cubes[i].z2) {
                    if (nextPos.x + 0.5 > cubes[i].x1 && nextPos.x < cubes[i].x2) {
                        ok = false;
                        break;
                    }
                    else if (nextPos.x > cubes[i].x1 && nextPos.x - 0.5 < cubes[i].x2) {
                        ok = false;
                        break;
                    }

                }
            }
          
            if (ok) {
                camera->TranslateRight(deltaTime * cameraSpeed);
            }

        }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // CTRL   first <-> third person
    if (key == GLFW_KEY_LEFT_CONTROL && gameOn)
    {
        if (firstPerson) {

            glm::vec3 aux = camera->position;
            camera->MoveForward(-3);
            camera->position[1] = 2.5f;
            camera->Set(camera->position, aux, glm::vec3(0, 1, 0));

            firstPerson = false;
        }
        else {

            camera->position[1] = 2;
            camera->MoveForward(3);
            playerFirstPersonLookAngleX = 0;
            firstPerson = true;
        }
    }
 
}


void Tema2::OnKeyRelease(int key, int mods)
{
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

        float sensivityOX = 0.003f;
        float sensivityOY = 0.003f;

        playerLookAngleY += sensivityOY * -deltaX;

        if (gameOn) {
            if (firstPerson) {
                camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
                camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
                playerFirstPersonLookAngleX += sensivityOX * deltaY;
            }
            else {
                camera->RotateThirdPerson_OY(sensivityOY * -deltaX);
            }
        }
        
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // fire
    if (button == GLFW_MOUSE_BUTTON_2 && gameOn) {
        if (time_until_fire <= 0 && firstPerson) {
            projectiles.push_back(Projectile3D(camera->position, glm::normalize(camera->forward)));
            
            time_until_fire = 2;
        }
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
