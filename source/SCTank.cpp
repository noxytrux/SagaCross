#include "SCTank.h"
#include "SCAudio.h"
#include "SCBulletManager.h"
#include "SCGround.h"
#include "SCSparkles.h"
#include "SCCamera.h"
#include "SCMeshInstanceManager.h"
#include "SCRandomMeshPoint.h"
#include "SCParticleLayer.h"

using namespace sc;

const float kMaxTankRotationSpeed = 4.0f;
const float PiOver180 = M_PI / 180.0f;

inline float myMod(float a, float b)
{
    float r = fmod(a, b);

    if (r < 0)
    {
        return b + r;
    }
    else {
        return r;
    }
}

inline float differenceMod(float a, float b, float modulo)
{
    if (myMod(b - a, modulo) > myMod(a - b, modulo))
    {
        return -myMod(a - b, modulo);
    }
    else
    {
        return myMod(b - a, modulo);
    }
}

SCTank::SCTank(const json &botsData,
               const std::string &p,
               const std::shared_ptr<SCGround> &g,
               const std::shared_ptr<SCOpenGLRenderable> &r,
               const std::shared_ptr<SCAudio> &a,
               const std::shared_ptr<SCCamera> &c,
               const std::shared_ptr<SCParticleLayer> &pl,
               const std::shared_ptr<SCBonusManager> &bm,
               const std::shared_ptr<SCRandomMeshPoint> &rmp,
               const std::shared_ptr<SCMeshInstance> &mi,
               const std::shared_ptr<SCBulletManager> bum,
               bool ai,
               float x, float y, float z,
               int peerID, SCGameTankType TankModel, bool isAIHard)
    : SCVehicleObj()
    , ground(g)
    , renderer(r)
    , audio(a)
    , camera(c)
    , particleLayer(pl)
    , bonusManager(bm)
    , randomMeshPoint(rmp)
    , meshInstance(mi)
    , bulletManager(bum)
    , ai(ai)
{
    path = p;
    HARD_AI = isAIHard;

    emots[0] = textureLoader.loadFile(path + "emoticons/smile.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emots[1] = textureLoader.loadFile(path + "emoticons/sad.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emots[2] = textureLoader.loadFile(path + "emoticons/evil.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emots[3] = textureLoader.loadFile(path + "emoticons/tongue.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emots[4] = textureLoader.loadFile(path + "emoticons/dead.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emots[5] = textureLoader.loadFile(path + "emoticons/werid.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    emotID = 0;

    blank = textureLoader.loadFile(path + "textures/blank.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    skull = textureLoader.loadFile(path + "textures/Skull.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    star = textureLoader.loadFile(path + "textures/Star.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    aimTex = textureLoader.loadFile(path + "textures/crossair.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    muzzle_flash = textureLoader.loadFile(path + "textures/mesh_fx-muzzleflash-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    body_destroyed = textureLoader.loadFile(path + "textures/mesh_tank_02_destroyed.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    muzzle_flash_mesh = meshInstance->GetMesh("models/fx_muzzleflash-a", path, SCMeshTypeNormal);

    tankHitTex = textureLoader.loadFile(path + "textures/mesh_fx-explosion01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    tankHitMesh = std::dynamic_pointer_cast<SCFastCutOffMesh>(meshInstance->GetMesh("models/fx_explosion-spherical-c", path, SCMeshTypeCutOff));

    tankExplosionA = std::dynamic_pointer_cast<SCFastCutOffMesh>(meshInstance->GetMesh("models/fx_explosion-spherical-b", path, SCMeshTypeCutOff));
    tankExplosionB = std::dynamic_pointer_cast<SCFastCutOffMesh>(meshInstance->GetMesh("models/fx_explosion-spherical-b", path, SCMeshTypeCutOff));

    destroyedAnim = false;
    destoyedASize = 0.0f;
    destoyedBSize = 0.0f;
    destoyedPosA = 0.0f;
    destoyedPosB = 0.0f;

    textureExpGround = textureLoader.loadFile(path + "textures/mesh_fx-ground-explosion-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    explosionFirst = meshInstance->GetMesh("models/fx_ground-explosion-b", path, SCMeshTypeNormal);

    switch (TankModel) {
        case TankTypeNormal:
        case TankTypeHeavy:

            suspension = meshInstance->GetMesh("tanks/Normal/tank_suspension_01", path, SCMeshTypeNormal);
            chasis = meshInstance->GetMesh("tanks/Normal/tank_chasis_01", path, SCMeshTypeNormal);
            turret = meshInstance->GetMesh("tanks/Normal/tank_turret_01", path, SCMeshTypeNormal);
            gun = meshInstance->GetMesh("tanks/Normal/tank_gun_01", path, SCMeshTypeNormal);

            break;
        case TankTypeLight:
        default:
            break;
    }

    isSterable = true;

    Pos[0] = x;
    Pos[1] = y;
    Pos[2] = z;

    follow = nullptr;

    if (ai)
    {
        color[0] = 229;
        color[1] = 148;
        color[2] = 142;

        body = textureLoader.loadFile(path + "textures/mesh_tank_02_red.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

        kMaxTurretSpeed = botsData["turret_speed"].get<float>();
        kMaxChaseDistance = botsData["chase_distance"].get<float>();
        kMaxShootChance = botsData["shoot_chance"].get<int>();
        max_speed = botsData["max_speed"].get<float>();
    }
    else
    {
        kMaxTurretSpeed = 6.0f;
        kMaxChaseDistance = 120.0f;
        kMaxShootChance = 15;
        max_speed = 1.2f;

        switch (peerID) {
            case 0: {

                switch (TankModel) {
                    case TankTypeNormal:
                    case TankTypeHeavy:

                        body = textureLoader.loadFile(path + "textures/mesh_tank_02_blue.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

                        break;

                    case TankTypeLight:
                    default:
                        break;
                }


                color[0] = 32;
                color[1] = 32;
                color[2] = 128;
            }
                break;

            case 1: {

                body = textureLoader.loadFile(path + "textures/mesh_tank_02_yellow.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

                color[0] = 255;
                color[1] = 165;
                color[2] = 0;
            }
                break;

            case 2: {

                body = textureLoader.loadFile(path + "textures/mesh_tank_02_green.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

                color[0] = 32;
                color[1] = 128;
                color[2] = 32;
            }
                break;

            case 3: {

                body = textureLoader.loadFile(path + "textures/mesh_tank_02_white.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

                color[0] = 139;
                color[1] = 69;
                color[2] = 0;
            }
                break;

            default: {

                body = textureLoader.loadFile(path + "textures/mesh_tank_02_white.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

                color[0] = rand() % 255;
                color[1] = rand() % 255;
                color[2] = rand() % 255;
            }
                break;
        }

    }

    ResetTank();
}

void SCTank::ResetTank() {

    alpha = 1.0f;
    inTIME = 0.0f;
    lastExplosionPos = xVec3(0, 0, 0);
    overfallScore = 0;

    Rot = 0.0f;

    timer = 0;
    emoTime = 0;
    live = 60;

    explode = 0.0f;

    render_info = true;
    traceNum = 0;
    tankDie = false;

    muzzleSize = 0.0f;
    shooted = false;

    hitPos = xVec3(0, 0, 0);
    hited = false;
    hitSize = 0.0f;

    aimPos[0] = aimPos[1] = aimPos[2] = -1;
    aimAcc[0] = aimAcc[1] = aimAcc[2] = 0;

    acc = 0.0f;
    score = 0;
    goal_count = 0;
    acc_to = 0;
    rifle_rot = 0;
    rifle_angle = 1;

    turn_acc = 0.0f;
    reload_time = 0;
    turn_to = 0;

    bullet_speed = 100.0;
    mines = 3;
    pyr_sound = 0;
    hit = 0.0f;

    internalRotation.id();
    active = true;
}

void SCTank::emoticon(int i)
{
    emotID = i;
    emoTime = 3.0;
}

void SCTank::DrawStatus(float dt)
{
    if (emoTime > 0) emoTime -= dt;
    else emoTime = 0;

    if (!render_info) return;

    float x, y;

    if (renderer->Get2DOGLPos(Pos[0], Pos[1] + 12.0f, Pos[2], x, y))
    {

        auto current = renderer->GuiShader;
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(current->uniforms[UNI_TEX0], 0);

        renderer->RectFill(x - 31, y - 3, x + 31, y + 3, 0, 0, 0, 128 + 64);
        renderer->RectFill(x - 30, y - 2, x - 30 + live, y + 2, 32 + 2 * (60 - live), 32 + 2 * live, 32, 255);

        if (!ai && isSterable)
        {
            if (live > 0) {
                int reloadTime = reload_time;
                reloadTime = std::max(reloadTime, 0);

                static int counter = 0;
                static bool blink = true;

                if (reloadTime == 0) {
                    if (counter++ % 15 == false) {
                        blink = !blink;
                    }
                }
                else counter = 0;

                renderer->RectFill(x - 31, y + 7, x + 31, y + 13, 0, 0, 0, 128 + 64);

                if (blink) {
                    renderer->RectFill(x - 30, y + 8, x - 30 + (60 - reloadTime), y + 8 + 4, 32 + 2 * (reloadTime), 32 + 2 * (60 - reloadTime), 32, 255);
                }
                else {
                    renderer->RectFill(x - 30, y + 8, x - 30 + (60 - reloadTime), y + 8 + 4, 272, 32, 32, 255);
                }
            }
        }


        if (live && emoTime > 0) {
            glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, emots[emotID]);
            renderer->DebugBlit(x - 32, y - 70, 64, 64);
        }

        if (!live)
        {

            glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, skull);
            renderer->DebugBlit(x - 20, y - 50, 40, 40);

        }

        if (score)
        {

            glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, star);

            for (int k = 0; k < score; ++k)
            {
                renderer->DebugBlit(x - score * 6 + k * 12, y - 16, 12, 12);
            }
        }
    }
}

void SCTank::Explode()
{
    lastExplosionPos = xVec3(Pos);

    destroyedAnim = true;

    destoyedASize = 0.0f;
    destoyedBSize = 0.0f;
    destoyedPosA = 0.0f;
    destoyedPosB = 0.0f;

    inTIME = 0.0f;
    alpha = 1.0f;

    SCAudio::SoundID explodeID = audio->loadSound("sounds/explode.wav");
    audio->playSound(explodeID, Pos, 1.0f);

    for (int i = 0; i < 40; ++i)
    {

        float dx = (rand() % 10 - 5) * 0.5f;
        float dz = (rand() % 10 - 5) * 0.5f;

        float dy = rand() % 3 * 10;

        LightSparcles.Add(Pos[0], Pos[1] + 1, Pos[2], dx * 2, dy, dz * 2, 12, 244, 142, 74, 2 + rand() % 2);
        ParticleSmoke.Add(Pos[0], Pos[1] + 1, Pos[2], dx, 0.0f, dz, 1, 32, 32, 32);

    }

    ParticleSmoke.Add(Pos[0], Pos[1] + 2, Pos[2], 0, 2, 0, 20, 12, 12, 12, 1);

    for (int i = 0; i < 360; i += 36)
    {

        ParticleSmoke.Add(Pos[0], Pos[1] + 2, Pos[2], cos(i * PiOver180), 0, sin(i * PiOver180), 3, 12, 12, 12, 1);

    }

    if (!ai && isSterable) {

        camera->AddShake(8.0f);
    }

    explode = 1.0f;
}

void SCTank::Die()
{
    if (tankDie) return;

    live = 0;

    if (live <= 0)
        live = 0;

    if (!tankDie) {
        tankDie = true;

        Explode();
        SCVehicle::Damage(Pos);
    }
}

bool SCTank::Damage(int c)
{
    if (isSterable && ai) emoticon((rand() % 2 ? 1 : 4));

    live -= c;

    if (live <= 0)
        Die();

    if (!ai && isSterable) {

        camera->AddShake(8.0f);

        hit = std::min(0.1f + rand() % 4 * 0.2f, c * 0.2f);

        if (c > 0) {

            std::string filename = "hitx";
            filename[3] = '0' + rand() % 4;

            SCAudio::SoundID hitID = audio->loadSound("sounds/" + filename + ".wav");
            audio->playSound(hitID, Pos, 1.0f);
        }

    }
    else {

        state = 1;
    }

    return tankDie;
}

void SCTank::Score()
{
    if (isSterable && ai) emoticon(0);

    score++;

    if (!ai) {

        std::string filename = "nicex";
        filename[4] = '0' + rand() % 5;

        SCAudio::SoundID niceID = audio->loadSound("sounds/" + filename + ".wav");
        audio->playSound(niceID, Pos, 1.0f);
    }
}

void SCTank::Render()
{

    auto current = renderer->SimpleShader;
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(current->uniforms[UNI_TEX0], 0);
    xMat34 meshMainMatrix;

    meshMainMatrix.t = xVec3(Pos[0], Pos[1], Pos[2]);
    meshMainMatrix.M = internalRotation; //.rotY(Rot* M_PI / 180.0);

    xMat34 meshAdditionalTransform;
    meshAdditionalTransform.t = xVec3(0, 0, 0);
    meshAdditionalTransform.M.id();

    if (active) {

        active = !tankDie;
    }
    else {

        live = 0;
    }

    if (explode > 0.0) {

        meshAdditionalTransform.t = xVec3(0, 6 * sin(explode * M_PI), 0);
        xMat33 a, b;

        a.rotZ((sinf(explode * M_PI) * 18.0f) * M_PI / 180.0);
        b.rotX((sinf(explode * M_PI) * 18.0f) * M_PI / 180.0);

        meshAdditionalTransform.M = a * b;
    }

    if (tankDie) {

        glBindTexture(GL_TEXTURE_2D, body_destroyed);
    }
    else {

        glBindTexture(GL_TEXTURE_2D, body);
    }

    if (live < 20 && rand() % (20 + live / 2) == 0)
    {

        float dx = (rand() % 10 - 5) * 0.5f;
        float dz = (rand() % 10 - 5) * 0.5f;

        float dy = rand() % 3 * 5;

        LightSparcles.Add(Pos[0] + dx, Pos[1], Pos[2] + dz, dx * 2, dy, dz * 2, 12 - live, 244, 142, 74, 2 + rand() % 2);

    }

    if (!live)
    {

        if (pyr_sound > 30 * 8)
        {
            SCAudio::SoundID fireID = audio->loadSound("sounds/fire.wav");
            audio->playSound(fireID, Pos, 1.0f, 0.1f);

            pyr_sound = 0;
        }

        pyr_sound += 1;
    }

    glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

    meshMainMatrix.t = xVec3(Pos);
    xMat34 outputMatrix = meshMainMatrix * meshAdditionalTransform;

    suspension->setUserMatrix(outputMatrix);
    suspension->Draw();

    //animacja akceleracji
    xMat34 meshAccMatrixZ;
    meshAccMatrixZ.t.zero();
    meshAccMatrixZ.M.id();
    xMat34 meshAccMatrixX;
    meshAccMatrixX.t.zero();
    meshAccMatrixX.M.id();

    xMat33 zacc, xacc;

    zacc.rotZ((acc * 9.0f * -1.0)* M_PI / 180.0);
    xacc.rotX(-turn_acc * M_PI / 180.0);

    meshAccMatrixX.M = xacc;
    meshAccMatrixZ.M = zacc;

    outputMatrix = meshMainMatrix * meshAdditionalTransform * meshAccMatrixZ * meshAccMatrixX;

    chasis->setUserMatrix(outputMatrix);
    chasis->Draw();

    render_info = renderer->getFrustum().SphereInFrustum(Pos[0], Pos[1], Pos[2], chasis->getAABBRay() * std::max(chasis->getScaleX(), std::max(chasis->getScaleY(), chasis->getScaleZ())));

    xMat34 turretMatrixMain;
    xMat33 turretMat;
    turretMat.rotY((rifle_rot * M_PI / 180.0) + M_PI);
    turretMatrixMain.M = turretMat;
    turretMatrixMain.t.zero();

    xMat34 translate;
    translate.t = xVec3(0.4, 5, 0); //2.4Y
    translate.M.id();

    outputMatrix = meshMainMatrix * meshAdditionalTransform * meshAccMatrixZ * meshAccMatrixX * turretMatrixMain * translate;

    turret->setUserMatrix(outputMatrix);
    turret->Draw();

    xMat33 rifle_AngleMat;
    rifle_AngleMat.rotZ(-rifle_angle * M_PI / 180.0);

    xMat34 rifleAngleMain;
    rifleAngleMain.t = xVec3(-2.4, 5.8, 0);
    rifleAngleMain.M = rifle_AngleMat;

    xMat33 tankSimpleRotation;
    tankSimpleRotation.rotY(Rot * M_PI / 180.0);
    xMat34 tankSimpleMatrix;
    tankSimpleMatrix.M = tankSimpleRotation;
    tankSimpleMatrix.t.zero();

    outputMatrix = tankSimpleMatrix * meshAdditionalTransform * meshAccMatrixZ * meshAccMatrixX;

    if (!ai && isSterable) {

        camera->cameraRoatation = outputMatrix.M;
        camera->yPosAddition = fabs(acc) * 62.0;
    }

    rifleAngleMain.M = rifle_AngleMat;
    outputMatrix = meshMainMatrix * meshAdditionalTransform * meshAccMatrixZ * meshAccMatrixX * turretMatrixMain * rifleAngleMain;

    gun->setUserMatrix(outputMatrix);
    gun->Draw();

    glDepthMask(0);
    glEnable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, muzzle_flash);

    xMat34 muzzleAngleMain;
    muzzleAngleMain.t = xVec3(6.5, 5.8, 0);
    muzzleAngleMain.M = rifle_AngleMat;

    turretMat.rotY((rifle_rot * M_PI / 180.0));
    turretMatrixMain.M = turretMat;

    outputMatrix = meshMainMatrix * meshAdditionalTransform * meshAccMatrixZ * meshAccMatrixX * turretMatrixMain * muzzleAngleMain;


    if (shooted) {
        muzzleSize += deltaTime * 4.0;

        if (muzzleSize >= 1.0f) {
            muzzleSize = 0.0f;
            shooted = false;

            muzzle_flash_mesh->setScale(0.0f);
        }
        else {

            muzzle_flash_mesh->setScale(0.1f * muzzleSize);
            muzzle_flash_mesh->setUserMatrix(outputMatrix);
            muzzle_flash_mesh->Draw();
        }
    }

    glBindTexture(GL_TEXTURE_2D, tankHitTex);

    if (hited) {
        auto current = renderer->CutoffShader;
        current->begin();

        hitSize += deltaTime * 3.0f;

        if (hitSize >= 1.0f) {
            hitSize = 0.0f;
            hited = false;

            tankHitMesh->setScale(0.0f);
        }
        else {

            tankHitMesh->setScale(0.1f * hitSize);
            tankHitMesh->cutoff = hitSize;

            auto userMatrix = tankHitMesh->getUserMatrix();

            userMatrix.M.id();
            userMatrix.t = xVec3(Pos[0], Pos[1] + 2.0, Pos[2]) - hitPos;

            tankHitMesh->Draw();
        }

        current = renderer->SimpleShader;
        current->begin();
    }

    if (destroyedAnim) {

        auto current = renderer->CutoffShader;
        current->begin();

        destoyedASize += deltaTime;
        destoyedPosA += deltaTime * 10.0;

        if (destoyedASize > 0.5) {
            destoyedBSize += deltaTime;
            destoyedPosB += deltaTime * 11.0;
        }

        if (destoyedBSize >= 1.0f) {

            destoyedASize = 0.0f;
            destoyedBSize = 0.0f;

            destroyedAnim = false;

            tankExplosionA->setScale(0.0f);
            tankExplosionB->setScale(0.0f);
        }
        else {

            tankExplosionA->setScale((0.05f * destoyedASize) + 0.04);
            tankExplosionB->setScale((0.05f * destoyedBSize) + 0.03);
        }

        tankExplosionA->cutoff = destoyedASize;

        auto userMatrix = tankExplosionA->getUserMatrix();

        userMatrix.M.rotZ(destoyedASize * M_PI);
        userMatrix.t = xVec3(lastExplosionPos) + xVec3(0, 4.0 + destoyedPosA, 0);
        tankExplosionA->Draw();

        tankExplosionB->cutoff = destoyedBSize;

        userMatrix = tankExplosionB->getUserMatrix();

        userMatrix.M.rotZ(M_PI_4 + destoyedBSize * M_PI);
        userMatrix.t = xVec3(lastExplosionPos) + xVec3(0, 3.0 + destoyedPosB, 0);
        tankExplosionB->Draw();

        current = renderer->SimpleShader;
        current->begin();

        inTIME += deltaTime * 5.0;

        const float mineexpSize = 3.0f;
        const float mineLifeTime = 4.0f;

        if (inTIME < mineLifeTime)
        {

            glDisable(GL_CULL_FACE);

            alpha = (mineLifeTime - inTIME) / mineLifeTime;
            glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, alpha);

            glBindTexture(GL_TEXTURE_2D, textureExpGround);

            auto userMatrix = explosionFirst->getUserMatrix();

            userMatrix.t = xVec3(lastExplosionPos);
            userMatrix.t.y += 1.5;

            userMatrix.M.rotY(-(inTIME*0.5));

            explosionFirst->setScale(xVec3(0.1 * mineexpSize, (0.1 * (1.1 - std::min(inTIME, 1.0f))) * mineexpSize, 0.1 * mineexpSize));
            explosionFirst->Draw();

            glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);

            glEnable(GL_CULL_FACE);
        }

    }

    if (!ai && isSterable)
    {
        if (!tankDie) {

            aimPos[0] = Pos[0];
            aimPos[1] = Pos[1] + 5.7f;
            aimPos[2] = Pos[2];

            aimAcc[0] = bullet_speed * cos((Rot + rifle_rot) * PiOver180) * cos(rifle_angle * PiOver180);
            aimAcc[1] = bullet_speed * sin(rifle_angle * PiOver180);
            aimAcc[2] = -bullet_speed * sin((Rot + rifle_rot) * PiOver180) * cos(rifle_angle * PiOver180);

            float DT = 0.1;

            int idx = 0;
            traceNum = 0;

            while (aimPos[1] > ground->GetHeight(aimPos[0], aimPos[2])) {
                aimPos[0] += aimAcc[0] * DT;
                aimPos[1] += aimAcc[1] * DT;
                aimPos[2] += aimAcc[2] * DT;

                aimAcc[0] *= 0.98f;
                aimAcc[1] -= 12.0f * DT;
                aimAcc[2] *= 0.98f;

                tracePoints[idx] = aimPos[0];
                tracePoints[idx + 1] = aimPos[1];
                tracePoints[idx + 2] = aimPos[2];

                idx += 3;

                traceNum++;
            }

            tracePoints[idx] = aimPos[0];
            tracePoints[idx + 1] = aimPos[1];
            tracePoints[idx + 2] = aimPos[2];

            aimPos[0] -= aimAcc[0] * DT;
            aimPos[1] -= aimAcc[1] * DT;
            aimPos[2] -= aimAcc[2] * DT;

            aimPos[1] = ground->GetHeight(aimPos[0], aimPos[2]) + 0.3f;

            static float aimRot = 360;

            xMat34 aimrotation;
            aimrotation.id();
            aimrotation.M.rotY(aimRot * M_PI / 180.0);
            aimRot -= 4;
            if (aimRot <= 0) aimRot = 360;

            xMat34 positionmat;
            positionmat.id();
            positionmat.t = xVec3(aimPos);

            glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, renderer->Projection.m());
            glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, ((renderer->ModelView * positionmat) * aimrotation).m());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, aimTex);
            glUniform1i(current->uniforms[UNI_TEX0], 0);
            glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

            const int S = 12;

            float squareVertices[] = {
                -S , 0.5f,  S ,
                S , 0.5f,  S ,
                -S , 0.5f, -S ,
                S , 0.5f, -S
            };

            static float textureVertices[] = {
                1, 1,
                0, 1,
                1, 0,
                0, 0
            };

            glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, squareVertices);
            glEnableVertexAttribArray(ATTRIB_VERTEX);
            glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 0, textureVertices);
            glEnableVertexAttribArray(ATTRIB_COORDS);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        }

    }

    glDisable(GL_BLEND);
    glDepthMask(1);
}

void SCTank::DrawTray() {

    if (!ai && isSterable)
    {
        if (!tankDie) {

            auto current = renderer->SimpleShader;

            glEnable(GL_BLEND);
            glDepthMask(0);

            glBindTexture(GL_TEXTURE_2D, blank);
            glUniform4f(current->uniforms[UNI_TEX1], 1.0 * 0.6, 32.0 / 255.0 * 0.6, 32.0 / 255.0 * 0.6, 0.6);
            glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, renderer->Projection.m());
            glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, renderer->ModelView.m());

            glLineWidth(4.0);

            glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, tracePoints);
            glEnableVertexAttribArray(ATTRIB_VERTEX);

            glDrawArrays(GL_LINE_STRIP, 0, traceNum);

            glLineWidth(1.0f);

            glDisable(GL_BLEND);
            glDepthMask(1);

        }
    }

}

void SCTank::Steer(SCRenderObj * o, float dt)
{

    if (tankDie) {

        return;
    }

    if (isSterable) {
        if (ai)
        {
            Simulate(dt);
        }
        else
        {
            Input();
        }
    }

    acc += (acc_to - acc) * dt;

    turn_acc += (turn_to - turn_acc) * dt;

    float maxTanksSpeed = 38.0;

    listenerForward = xVec3(cos(Rot * PiOver180) * acc * dt * maxTanksSpeed,
                            0,
                            sin(Rot * PiOver180) * acc * dt  * maxTanksSpeed);

    Pos[0] -= listenerForward.x;
    Pos[2] += listenerForward.z;

    if (Pos[0] < -ground->GetGroundSize() + 6)
        Pos[0] = -ground->GetGroundSize() + 6;

    else if (Pos[0] > ground->GetGroundSize() - 6)
        Pos[0] = ground->GetGroundSize() - 6;

    if (Pos[2] < -ground->GetGroundSize() + 6)
        Pos[2] = -ground->GetGroundSize() + 6;

    else if (Pos[2] > ground->GetGroundSize() - 6)
        Pos[2] = ground->GetGroundSize() - 6;


    xVec3 norm = xVec3(0,1,0);

    if (ground)
    {
        Pos[1] = ground->GetHeight(Pos[0], Pos[2]);
        norm = ground->GetNormal(Pos[0], Pos[2]);
    }

    SCGroundType currType = particleLayer->getCurrentGroundType(Pos[0], Pos[2]);

    if (std::fabs(acc) > 0.2f)
    {
        for (int i = 3; i < 7; ++i)
        {

            float xp = i * cos(Rot * PiOver180) + 4 * sin(Rot * PiOver180);
            float zp = -i * sin(Rot * PiOver180) + 4 * cos(Rot * PiOver180);

            switch (currType) {
                case SCGroundTypeGrass: {
                    TankSmokeGrass.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                case SCGroundTypeSand: {
                    TankSmoke.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                case SCGroundTypeWater: {
                    TankSmokeWater.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                default:
                    break;
            }

            xp = i * cos(Rot * PiOver180) - 4 * sin(Rot * PiOver180);
            zp = -i * sin(Rot * PiOver180) - 4 * cos(Rot * PiOver180);

            switch (currType) {
                case SCGroundTypeGrass: {
                    TankSmokeGrass.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                case SCGroundTypeSand: {
                    TankSmoke.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                case SCGroundTypeWater: {
                    TankSmokeWater.Add(Pos[0] + xp, Pos[1] + 2.0, Pos[2] + zp, 0, 0, 0, rand() % 5 == 0, 255, 255, 255, 0.9);
                }
                    break;

                default:
                    break;
            }

        }

    }

    if (pyr_sound > 20)
    {
        SCAudio::SoundID pyrID = audio->loadSound("sounds/pyr.wav");
        audio->playSound(pyrID, Pos, 0.7f - ai * 0.3, 0.1f);
        pyr_sound = 0;
    }

    pyr_sound += std::max((int)(acc * 10), 2);

    norm.normalize();

    xQuat quaternion;
    quaternion.fromAngleAxis(Rot, norm);
    internalRotation.id();
    internalRotation.fromQuat(quaternion);

    turn_to = 0.0f;
    acc_to = 0.0f;

    if (reload_time == 4 && !ai) {
        std::string filename = "missilex";
        filename[7] = '0' + rand() % 3;

        SCAudio::SoundID readyID = audio->loadSound("sounds/" + filename + ".wav");
        audio->playSound(readyID, Pos, 1.0f);
    }

    if (reload_time > 0) reload_time -= 4;

    rifle_angle = std::min(std::max(rifle_angle, -6.0f), 24.0f); //MAX VAL -12!
}

void SCTank::Simulate(float dt)
{

    deltaTime = dt;

    if (explode > 0.0) {

        explode -= dt;
    }

    timer += dt;

    if (!live) {
        return;
    }

    switch (state)
    {
        case 0:

            if (rand() % 300 == 0)
                state = 1;

            if (rand() % 600 == 0)
                state = 2;

            break;
        case 1:
            acc_to = max_speed;

            if (rand() % 1000 == 0)
                state = 0;

            break;
        case 2:
            acc_to = -0.5*max_speed;

            if (rand() % 400 == 0)
                state = 0;

            break;
    }

    if (HARD_AI) {

        auto tank = SCVehicle::all[0];

        if (tank->active && tank.get() != this) {

            xVec3 A = xVec3(this->Pos);
            xVec3 B = xVec3(tank->Pos);

            if (A.distance(B) < kMaxChaseDistance) {

                memcpy(Goal, tank->Pos, 12);
                isGoal = true;

                follow = tank.get();
            }
        }
    }

    int extra_action = rand() % 1000;

    if (extra_action < kMaxShootChance)
    {
        Shoot();
    }
    else if (extra_action < 20 && mines)
    {
        DropMine();

        mines--;
        state = 1;
        rot_random = 0;

        SCAudio::SoundID tickID = audio->loadSound("sounds/tick.wav");
        audio->playSound(tickID, Pos, 1.0f);
    }

    else if (!isGoal && extra_action < 40)
    {
        follow = nullptr;

        std::shared_ptr<SCVehicleObj> t = nullptr;

        if (rand() % 2) {
            t = SCVehicle::all[0];
        }
        else {
            t = SCVehicle::GetRandom();
        }

        if (t.get() != this && t->active)
        {

            memcpy(Goal, t->Pos, 12);
            isGoal = true;

            follow = t.get();

        }

    }
    else if (!isGoal && extra_action < 65) {

        follow = nullptr;
        isGoal = true;

        xVec3 rnd = randomMeshPoint->GetRandomPoint();

        Goal[0] = rnd.x;
        Goal[2] = rnd.z;

        Goal[1] = ground->GetHeight(Goal[0], Goal[2]);

    }

    rot_random += (rand() % 5 - 3) * deltaTime;
    rot_rifle += (rand() % 5 - 3) * deltaTime;
    rot_rifle_a += (rand() % 5 - 3) * deltaTime;

    if (isGoal)
    {

        if (follow == nullptr && ++goal_count > 180 * 12) {
            follow = nullptr;
            isGoal = false;
            goal_count = 0;

            Shoot();
        }

        if (follow) {

            memcpy(Goal, follow->Pos, 12);

            isGoal = follow->active;
            if (!isGoal)
                follow = nullptr;

        }

        xVec3 G(Goal);
        xVec3 P(Pos);
        xVec3 D = P - G;
        D.y = 0.0f;

        float angle = atan2(-D.z, D.x) / PiOver180;

        if (Rot < angle) {
            Rot += std::min((angle - Rot) * 0.2f, 2.0f);
            turn_to = 10.0f;
        }
        else if (Rot > angle) {
            Rot += std::max((angle - Rot) * 0.2f, -2.0f);
            turn_to = -10.0f;
        }

        float distance = D.magnitude();

        acc_to = std::min(std::max(max_speed * distance / 14, 0.0f), max_speed);

        if (distance < 2 && !follow)
        {
            Shoot();

            isGoal = false;
            follow = nullptr;
            memset(Goal, 0, 12);
            goal_count = 0;
        }

        if (rand() % 20 == 0)
            state = 1;

        if (distance < 60) {

            if (rifle_rot < 181) rot_rifle = -90;
            else if (rifle_rot > 181) rot_rifle = 90;

            if (rifle_angle < 2) rot_rifle_a = -90;
            else if (rifle_angle > 2) rot_rifle_a = 90;

            if (HARD_AI) {

                rifle_rot += (differenceMod(rifle_rot, (180 - (Rot - angle)), 360.0) * (deltaTime *kMaxTurretSpeed));
            }
        }
    }
    else
    {
        if (isSterable && ai && rand() % 2) emoticon(5);

        if (rot_random < -64)
        {
            Rot += 8.0 * deltaTime;
            turn_to = 10.0f;

            if (Rot > 180)
                Rot -= 360;

        }
        else if (rot_random > 64)
        {
            Rot -= 8.0 * deltaTime;
            turn_to = -10.0f;

            if (Rot < -180)
                Rot += 360;

        }

    }

    if (rot_rifle < -64)
        rifle_rot += 8.0 * deltaTime;
    else if (rot_rifle > 64)
        rifle_rot -= 8.0 * deltaTime;

    if (rot_rifle_a < -64)
        rifle_angle += 1.0 * deltaTime;
    else if (rot_rifle_a > 64)
        rifle_angle -= 1.0 * deltaTime;

}

void SCTank::Input()
{
    xVec2 dir = Velocity;

    auto settings = renderer->getSettings();

    if (settings->usesSecondCam()) {

        acc_to = (fabsf(dir.mag()) / 50.0) * max_speed;

        float desiretRotation = currAngle + 180;

        Rot += (differenceMod(Rot, desiretRotation, 360.0) * (deltaTime * kMaxTankRotationSpeed));
    }
    else {

        if (dir.x > 20) {
            Rot -= (fabsf(dir.x) / 45.0) * 4.0 - fabs(acc);
            turn_to = -10.0f;
        }

        if (dir.x < -20) {
            Rot += (fabsf(dir.x) / 45.0) * 4.0 - fabs(acc);
            turn_to = 10.0f;
        }

        if (dir.y < -20) {
            acc_to = (fabsf(dir.y) / 50.0) * max_speed;
        }

        if (dir.y > 20) {
            acc_to = -0.5 * max_speed;
        }
    }

    float desiredAngle = currAngle;

    if (settings->usesSecondCam()) {

        rifle_rot += (differenceMod(rifle_rot, (desiredAngle - Rot), 360.0) * (deltaTime * kMaxTurretSpeed));
    }
    else {

        rifle_rot += (differenceMod(rifle_rot, desiredAngle, 360.0) * (deltaTime * kMaxTurretSpeed));
    }
}

void SCTank::handleMineDrop()
{

    if (mines) {

        if (isSterable && ai) emoticon(3);

        DropMine();

        SCAudio::SoundID tickID = audio->loadSound("sounds/tick.wav");
        audio->playSound(tickID, Pos, 1.0f);

        if (--mines == 0) {

            SCAudio::SoundID mineID = audio->loadSound("sounds/last_mine.wav");
            audio->playSound(mineID, Pos, 1.0f);
        }
    }
    else {

        SCAudio::SoundID nomineID = audio->loadSound("sounds/no_mine.wav");
        audio->playSound(nomineID, Pos, 1.0f);
    }
}

void SCTank::Shoot()
{
    if (reload_time > 0) return;
    reload_time = 2 * 60;

    muzzleSize = 0.0f;
    shooted = true;

    if (isSterable && ai) emoticon(2);

    float bx = Pos[0];
    float by = Pos[1] + 5.7f;
    float bz = Pos[2];

    float ax = bullet_speed * cos((Rot + rifle_rot) * PiOver180) * cos(rifle_angle * PiOver180);
    float ay = bullet_speed * sin(rifle_angle * PiOver180);
    float az = -bullet_speed * sin((Rot + rifle_rot) * PiOver180) * cos(rifle_angle * PiOver180);

    auto bullet = std::make_shared<SCBullet>(meshInstance, audio, ground, path, bx, by, bz, ax, ay, az);
    bullet->parent = (void*)this;

    bulletManager->AddBullet(bullet);

    if (!ai && isSterable) {
        camera->AddShake(4.0f);
    }

    ParticleSmoke.Add(bx + ax * 0.09, by + ay * 0.09, bz + az * 0.09, ax*0.04, ay*0.04, az*0.04, 20, 12, 12, 12, 3);

    SCAudio::SoundID shootID = audio->loadSound("sounds/shoot.wav");
    audio->playSound(shootID, Pos, 1.0f, 0.1f);
}

void SCTank::Collide(std::vector<std::shared_ptr<SCVehicleObj>> &rest, std::vector<SCCollisionModel> &col, std::vector<std::shared_ptr<SCBullet>> &bullets)
{

    if (tankDie)
        return;

    for (int i = 0; i < bullets.size(); ++i) {

        if (bullets[i]->parent != (void*)this)
        {
            xVec3 P(Pos);
            xVec3 G(bullets[i]->Pos);

            float L = (P - G).magnitude();

            if (L < 14.0f)
            {

                LightSparcles.Add(Pos[0], Pos[1], Pos[2], -0.2*bullets[i]->Acc[0], -0.2*bullets[i]->Acc[1], -0.2*bullets[i]->Acc[2], 20, 244, 142, 74, 0.6 + rand() % 2);


                hitSize = 0.0f;
                hited = true;

                hitPos = xVec3(Pos) - xVec3(bullets[i]->Pos);
                hitPos.normalize();
                hitPos *= 6.0;

                bullets[i]->Destroy();

                SCTank *parentTank = (SCTank*)bullets[i]->parent;

                float damagePower = std::max((int)(64 - (L*0.33) * 10), 5);

                if (Damage(damagePower)) {

                    (parentTank)->Score();
                }

            }

            if (ai) {

                follow = nullptr;

                memcpy(Goal, ((SCTank*)bullets[i]->parent)->Pos, 12);
                isGoal = true;

                follow = ((SCTank*)bullets[i]->parent);
            }

            return;
        }
    }



    for (int i = 0; i < col.size(); ++i)
    {

        SCCollisionModel * other = &col[i];

        float dx = other->Pos[0] - Pos[0];
        float dz = other->Pos[2] - Pos[2];

        float distance = dx * dx + dz * dz;
        float away_distance = other->R * other->R * 30;

        if (distance < other->R * other->R * 4)
        {

            distance = std::sqrt(distance);
            if (!distance) distance = 1;

            dx = dx / distance * other->R * 2;
            dz = dz / distance * other->R * 2;

            Pos[0] = other->Pos[0] - dx;
            Pos[2] = other->Pos[2] - dz;

            LightSparcles.Add(Pos[0] + dx * 0.5, Pos[1], Pos[2] + dz * 0.5, 0, 1, 0, 4, 244, 142, 74, 0.6 + rand() % 2);

            if (rand() % 10 == 0) {

                SCAudio::SoundID metalID = audio->loadSound("sounds/metal.wav");
                audio->playSound(metalID, Pos, 1.0f, 0.2f);
            }

            if (!ai && isSterable)
            {
                camera->AddShake(1);
                hit = rand() % 4 * 0.1f;
            }
            else
            {
                state = 2;

                if (rand() % 200 == 0) {
                    if (isGoal) isGoal = false;
                }

                xVec3 cGoal = xVec3(Goal);

                xMat33 randRot;
                randRot.rotY(2.0f * M_PI / 180.0f);
                randRot.multiply(cGoal, cGoal);

                Goal[0] = cGoal.x;
                Goal[1] = cGoal.y;
                Goal[2] = cGoal.z;

            }

        }
        else if (distance < away_distance)
        {

            if (ai && (state == 1))
            {

                acc_to *= std::min(1.0 - distance / away_distance, 1.0);

                Rot += (3 - fabs(acc)) * (1 - distance / away_distance);
                turn_to = 10.0f;

            }

        }

    }

    for (int i = 0; i < rest.size(); ++i) {

        if (rest[i].get() != this)
        {

            auto other = rest[i];

            float dx = other->Pos[0] - Pos[0];
            float dz = other->Pos[2] - Pos[2];

            float distance = dx * dx + dz * dz;

            const float away_distance = 24 * 12;

            if (distance < 20 * 4)
            {

                distance = sqrt(distance);
                if (!distance) distance = 1;

                dx = dx / distance * 9;
                dz = dz / distance * 9;

                Pos[0] = other->Pos[0] - dx;
                Pos[2] = other->Pos[2] - dz;

                LightSparcles.Add(Pos[0] + dx * 0.5, Pos[1], Pos[2] + dz * 0.5, 0, 1, 0, 4, 244, 142, 74, 0.6 + rand() % 2);

                if (rand() % 10 == 0) {

                    SCAudio::SoundID metalID = audio->loadSound("sounds/metal.wav");
                    audio->playSound(metalID, Pos, 1.0f, 0.2f);
                }

                if (!ai  && isSterable)
                {
                    camera->AddShake(1);
                    hit = rand() % 4 * 0.1f;

                }
                else
                {
                    state = 2;
                    if (isGoal) isGoal = false;
                }

            }
            else if (distance < away_distance)
            {

                if (ai)
                {
                    acc_to *= std::min(1.0f - distance / away_distance, 1.0f);

                    Rot += (3 - fabs(acc)) * (1 - distance / away_distance);
                    turn_to = 10.0f;

                }

            }

        }

    }

}

void SCTank::DropMine()
{
    bonusManager->AddBonus(std::make_shared<SCMine>(shared_from_this(),
                                                    path,
                                                    randomMeshPoint,
                                                    ground,
                                                    meshInstance));
}

