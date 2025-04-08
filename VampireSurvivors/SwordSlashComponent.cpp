#include "AstroidsPrivate.h"
#include "SwordSlashComponent.h"
#include "CameraManager.h"
#include "HealthComponent.h"

SwordSlashComponent::SwordSlashComponent(GameObject * owner, GameManager & gameManager, float arcAngleDeg, float radius, float duration)
	: GameComponent(owner, gameManager)
	, mCenter(owner->GetPosition())
	, mArcAngleRad(arcAngleDeg * BD::gsPi / 180.f)
	, mRadius(radius)
	, mDuration(duration)
	, mElapsedTime(0.f)
	, mTimeSinceLastSlash(0.f)
	, mCooldown(1.0f)
	, mIsSlashing(false)
	, mpFixture(nullptr)
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
            sf::Vector2f playerPos = GetGameObject().GetParent()->GetPosition();
            sf::Vector2f aimDir = pCameraManager->GetCrosshairPosition() - playerPos;

            float len = std::sqrt(aimDir.x * aimDir.x + aimDir.y * aimDir.y);
            if (len > 0.01f)
            {
                aimDir /= len;
                PerformSlash(aimDir);
            }
        }
    }

    if (!mIsSlashing)
        return;

    mElapsedTime += deltaTime;
    float t = std::min(mElapsedTime / mDuration, 1.0f);
    float swingAngle = -mArcAngleRad / 2.f + mArcAngleRad * t;

    auto pCameraManager = GetGameManager().GetManager<CameraManager>();
    if (!pCameraManager) return;

    sf::Vector2f mouseWorld = pCameraManager->GetCrosshairPosition();
    sf::Vector2f currentDir = mouseWorld - mCenter;

    float len = std::sqrt(currentDir.x * currentDir.x + currentDir.y * currentDir.y);
    if (len > 0.01f) currentDir /= len;
    else currentDir = { 1.f, 0.f };

    float cosA = std::cos(swingAngle);
    float sinA = std::sin(swingAngle);
    sf::Vector2f swingDir(
        currentDir.x * cosA - currentDir.y * sinA,
        currentDir.x * sinA + currentDir.y * cosA
    );

    sf::Vector2f newPos = mCenter + swingDir * mRadius;
    GetGameObject().SetPosition(newPos);

    float alpha = 1.f - t;
    mWedge.setFillColor(sf::Color(255, 100, 100, static_cast<sf::Uint8>(alpha * 180)));

    if (mElapsedTime >= mDuration)
    {
        mIsSlashing = false;
        mElapsedTime = 0.f;

        if (mpFixture)
            mpFixture->SetSensor(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void SwordSlashComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
	if (!mIsSlashing)
		return;

	mWedge.setPosition(GetGameObject().GetParent()->GetPosition());

	auto pCameraManager = GetGameManager().GetManager<CameraManager>();
	if (!pCameraManager)
		return;

	sf::Vector2f mouseWorld = pCameraManager->GetCrosshairPosition();
	sf::Vector2f currentDir = mouseWorld - mCenter;

	float angle = 0.f;
	if (std::abs(currentDir.x) > 0.001f || std::abs(currentDir.y) > 0.001f)
		angle = std::atan2(currentDir.y, currentDir.x) * 180.f / BD::gsPi;

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

void SwordSlashComponent::PerformSlash(const sf::Vector2f & direction)
{
    if (mTimeSinceLastSlash < mCooldown || mIsSlashing)
    {
        return;
    }

	mTimeSinceLastSlash = 0.f;
	mElapsedTime = 0.f;
	mIsSlashing = true;

	mCenter = GetGameObject().GetParent()->GetPosition();

	if (!mpFixture)
	{
		b2Body * pBody = GetGameObject().GetPhysicsBody();
		if (pBody && pBody->GetFixtureList())
			mpFixture = pBody->GetFixtureList();
	}
    if (mpFixture)
    {
        mpFixture->SetSensor(false);
    }
}
//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------