#include "AstroidsPrivate.h"
#include "SwordSlashComponent.h"
#include "CameraManager.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "DamageComponent.h"

SwordSlashComponent::SwordSlashComponent(GameObject * owner, GameManager & gameManager, float arcAngleDeg, float radius, float duration)
	: GameComponent(owner, gameManager)
    , mSlashObjs()
	, mArcAngleRad(arcAngleDeg * BD::gsPi / 180.f)
	, mRadius(radius)
	, mDuration(duration)
	, mElapsedTime(0.f)
	, mTimeSinceLastSlash(0.f)
	, mCooldown(1.0f)
    , mDamagePerSlash(50.f)
	, mIsSlashing(false)
	, mName("SwordSlashComponent")
{
    int pointCount = 30; // number of triangle segments to form the arc
    float halfAngle = mArcAngleRad * 0.5f;

    mWedge.setPointCount(pointCount + 2); // center + all arc points
    mWedge.setPoint(0, sf::Vector2f(0.f, 0.f)); // center

    for (int i = 0; i <= pointCount; ++i)
    {
        float t = static_cast<float>(i) / pointCount;
        float angle = -halfAngle + mArcAngleRad * t;
        sf::Vector2f point(std::cos(angle) * mRadius, std::sin(angle) * mRadius);
        mWedge.setPoint(i + 1, point);
    }

    mWedge.setFillColor(sf::Color(255, 100, 100, 180));
    mWedge.setOutlineThickness(1.f); // optional debug help
    mWedge.setOutlineColor(sf::Color::White);
}

//------------------------------------------------------------------------------------------------------------------------

SwordSlashComponent::~SwordSlashComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::Update(float deltaTime)
{
    mTimeSinceLastSlash += deltaTime;

    // Automatically trigger slash when cooldown is met
    if (!mIsSlashing && mTimeSinceLastSlash >= mCooldown)
    {
        auto pCameraManager = GetGameManager().GetManager<CameraManager>();
        if (pCameraManager)
        {
            sf::Vector2f playerPos = GetGameObject().GetParent().GetPosition();
            sf::Vector2f aimDir = pCameraManager->GetCrosshairPosition() - playerPos;

            float len = std::sqrt(aimDir.x * aimDir.x + aimDir.y * aimDir.y);
            if (len > 0.01f)
            {
                aimDir /= len;
                mSlashDirection = aimDir;
                PerformSlash();
            }
        }
    }

    if (!mIsSlashing)
    {
        return;
    }

    mElapsedTime += deltaTime;
    float t = std::min(mElapsedTime / mDuration, 1.0f);
    float swingAngle = -mArcAngleRad / 2.f + mArcAngleRad * t;

    auto pCameraManager = GetGameManager().GetManager<CameraManager>();
    if (!pCameraManager)
    {
        return;
    }

    sf::Vector2f mouseWorld = pCameraManager->GetCrosshairPosition();
    sf::Vector2f currentDir = mouseWorld - GetGameObject().GetPosition();

    float len = std::sqrt(currentDir.x * currentDir.x + currentDir.y * currentDir.y);
    if (len > 0.01f)
    {
        currentDir /= len;
    }
    else
    {
        currentDir = { 1.f, 0.f };
    }

    float cosA = std::cos(swingAngle);
    float sinA = std::sin(swingAngle);
    sf::Vector2f swingDir(
        currentDir.x * cosA - currentDir.y * sinA,
        currentDir.x * sinA + currentDir.y * cosA
    );

    sf::Vector2f newPos = GetGameObject().GetPosition() + swingDir * mRadius;
    GetGameObject().SetPosition(newPos);

    float alpha = 1.f - t;
    mWedge.setFillColor(sf::Color(255, 100, 100, static_cast<sf::Uint8>(alpha * 180)));

    if (mElapsedTime >= mDuration)
    {
        mIsSlashing = false;
        mElapsedTime = 0.f;
    }

    CleanUpSlashes(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (!mIsSlashing)
    {
        return;
    }

	mWedge.setPosition(GetGameObject().GetParent().GetPosition());

	auto pCameraManager = GetGameManager().GetManager<CameraManager>();
    if (!pCameraManager)
    {
        return;
    }

	float angle = 0.f;
    if (std::abs(mSlashDirection.x) > 0.001f || std::abs(mSlashDirection.y) > 0.001f)
    {
        angle = std::atan2(mSlashDirection.y, mSlashDirection.x) * 180.f / BD::gsPi;
    }

	mWedge.setRotation(angle);
	target.draw(mWedge, states);
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & SwordSlashComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::PerformSlash()
{
    if (mTimeSinceLastSlash < mCooldown || mIsSlashing)
    {
        return;
    }

	mTimeSinceLastSlash = 0.f;
	mElapsedTime = 0.f;
	mIsSlashing = true;

    auto & gameManager = GetGameManager();
    auto slashHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, GetGameObject().GetHandle());
    auto * pSlashObj = gameManager.GetGameObject(slashHandle);
    if (!pSlashObj)
    {
        return;
    }

    pSlashObj->SetPosition(GetGameObject().GetParent().GetPosition());
    float angleRad = std::atan2(mSlashDirection.y, mSlashDirection.x);
    float angleDeg = angleRad * (180.f / BD::gsPi);
    pSlashObj->SetRotation(angleDeg);
    
    // Collision
    {
        auto pSlashCollisionComponent = pSlashObj->GetComponent<CollisionComponent>().lock();
        if (!pSlashCollisionComponent)
        {
            pSlashObj->CreateWedgeShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                mArcAngleRad,
                (mRadius) / BD::gsPixelsPerMeter,
                6,            // safe number of segments
                true          // dynamic
            );

            pSlashObj->AddComponent(std::make_shared<CollisionComponent>(
                pSlashObj,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pSlashObj->GetPhysicsBody(),
                pSlashObj->GetSize(),
                true
            ));
        }
    }
    // DamageComponent
    {
        auto pSlashDamageComponent = pSlashObj->GetComponent<DamageComponent>().lock();
        if (!pSlashDamageComponent)
        {
            auto pSwordSlashComponent = std::make_shared<DamageComponent>(pSlashObj, gameManager, mDamagePerSlash);
            pSlashObj->AddComponent(pSwordSlashComponent);
        }
    }

    Slash slash = { slashHandle, mDuration, mDamagePerSlash };
    mSlashObjs.push_back(slash);
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::CleanUpSlashes(float deltaTime)
{
    auto & gameManager = GetGameManager();

    for (auto & slash : mSlashObjs)
    {
        GameObject * pSlash = gameManager.GetGameObject(slash.handle);
        if (pSlash && !pSlash->IsDestroyed())
        {
            slash.lifespan -= deltaTime;
        }
    }

    mSlashObjs.erase(
        std::remove_if(mSlashObjs.begin(), mSlashObjs.end(),
            [&gameManager](Slash & slash) {
                GameObject * pSlash = gameManager.GetGameObject(slash.handle);
                if (pSlash && !pSlash->IsDestroyed() && slash.lifespan <= 0.0f)
                {
                    pSlash->Destroy();
                }
                return !pSlash || pSlash->IsDestroyed();
            }),
        mSlashObjs.end());
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------