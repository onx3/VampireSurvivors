#include "AstroidsPrivate.h"
#include "CollisionListener.h"
#include "GameObject.h"
#include "ProjectileComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "DamageZoneComponent.h"

namespace
{
    static const int sScorePerCoin = 250;
}

CollisionListener::CollisionListener(GameManager * pGameManager)
    : mpGameManager(pGameManager)
{
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionListener::BeginContact(b2Contact * contact)
{
    GameObject * pObjectA = reinterpret_cast<GameObject *>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    GameObject * pObjectB = reinterpret_cast<GameObject *>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if (pObjectA && pObjectB)
    {
        HandleCollision(pObjectA, pObjectB);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionListener::HandleCollision(GameObject * pObjA, GameObject * pObjB)
{
    // Enemy hit player
    if (pObjA->GetTeam() == ETeam::Player && pObjB->GetTeam() == ETeam::Enemy)
    {
       // Do nothing only when the Enemy Attacks
    }
    else if (pObjA->GetTeam() == ETeam::Enemy && pObjB->GetTeam() == ETeam::Player)
    {
        // Do nothing only when the Enemy Attacks
    }
    // Persistant Obj
    else if (pObjA->GetTeam() == ETeam::FriendlyPersistant && pObjB->GetTeam() == ETeam::Enemy)
    {
        if (pObjB->IsActive())
        {
            // Check for Zone Damage
            auto pZone = pObjA->GetComponent<DamageZoneComponent>().lock();
            if (pZone)
            {
                pZone->OnBeginOverlap(pObjB->GetHandle());
            }

            // Normal Damage Check
            float damageNumber = 0.f;
            auto pObjADamageComponent = pObjA->GetComponent<DamageComponent>().lock();
            if (pObjADamageComponent)
            {
                damageNumber = pObjADamageComponent->GetDamageAmount();
            }

            auto pObjBHealthComp = pObjB->GetComponent<HealthComponent>().lock();
            if (pObjBHealthComp)
            {
                pObjBHealthComp->LoseHealth(damageNumber);
            }
        }
    }
    else if (pObjA->GetTeam() == ETeam::Enemy && pObjB->GetTeam() == ETeam::FriendlyPersistant)
    {
        if (pObjA->IsActive())
        {
            // Check for Zone Damage
            auto pZone = pObjB->GetComponent<DamageZoneComponent>().lock();
            if (pZone)
            {
                pZone->OnBeginOverlap(pObjA->GetHandle());
            }

            // Normal Damage Check
            float damageNumber = 0.f;
            auto pObjBDamageComponent = pObjB->GetComponent<DamageComponent>().lock();
            if (pObjBDamageComponent)
            {
                damageNumber = pObjBDamageComponent->GetDamageAmount();
            }

            auto pObjAHealthComp = pObjA->GetComponent<HealthComponent>().lock();
            if (pObjAHealthComp)
            {
                pObjAHealthComp->LoseHealth(damageNumber);
            }
        }
    }
    // Fleeting Obj
    else if (pObjA->GetTeam() == ETeam::FriendlyFleeting && pObjB->GetTeam() == ETeam::Enemy)
    {
        if (pObjB->IsActive())
        {
            float damageNumber = 0.f;
            auto pObjADamageComponent = pObjA->GetComponent<DamageComponent>().lock();
            if (pObjADamageComponent)
            {
                damageNumber = pObjADamageComponent->GetDamageAmount();
            }

            auto pObjBHealthComp = pObjB->GetComponent<HealthComponent>().lock();
            if (pObjBHealthComp)
            {
                pObjBHealthComp->LoseHealth(damageNumber);
                pObjA->Destroy();
            }
        }
    }
    else if (pObjA->GetTeam() == ETeam::Enemy && pObjB->GetTeam() == ETeam::FriendlyFleeting)
    {
        if (pObjA->IsActive())
        {
            float damageNumber = 0.f;
            auto pObjBDamageComponent = pObjB->GetComponent<DamageComponent>().lock();
            if (pObjBDamageComponent)
            {
                damageNumber = pObjBDamageComponent->GetDamageAmount();
            }

            auto pObjAHealthComp = pObjA->GetComponent<HealthComponent>().lock();
            if (pObjAHealthComp)
            {
                pObjAHealthComp->LoseHealth(damageNumber);
                pObjB->Destroy();
            }
        }
    }
    // Coin Drop
    else if (pObjA->GetTeam() == ETeam::Player && pObjB->GetTeam() == ETeam::CoinDrop)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            auto pUIManager = pObjA->GetGameManager().GetManager<UIManager>();
            if (pUIManager)
            {
                pUIManager->AddScore(sScorePerCoin);
            }
            pObjB->Deactivate();
        }
    }
    else if (pObjA->GetTeam() == ETeam::CoinDrop && pObjB->GetTeam() == ETeam::Player)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            auto pUIManager = pObjB->GetGameManager().GetManager<UIManager>();
            if (pUIManager)
            {
                pUIManager->AddScore(sScorePerCoin);
            }
            pObjA->Deactivate();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionListener::EndContact(b2Contact * contact)
{
    GameObject * pObjectA = reinterpret_cast<GameObject *>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    GameObject * pObjectB = reinterpret_cast<GameObject *>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if (!pObjectA || !pObjectB || pObjectA->IsDestroyed() || pObjectB->IsDestroyed())
    {
        return;
    }

    // Remove enemy from zone if either side ends contact
    if (pObjectA->GetTeam() == ETeam::FriendlyPersistant && pObjectB->GetTeam() == ETeam::Enemy)
    {
        if (pObjectA->HasComponent<DamageZoneComponent>())
        {
            auto pZone = pObjectA->GetComponent<DamageZoneComponent>().lock();
            if (pZone)
            {
                pZone->OnEndOverlap(pObjectB->GetHandle());
            }
        }
    }
    else if (pObjectA->GetTeam() == ETeam::Enemy && pObjectB->GetTeam() == ETeam::FriendlyPersistant)
    {
        if (pObjectB->HasComponent<DamageZoneComponent>())
        {
            auto pZone = pObjectB->GetComponent<DamageZoneComponent>().lock();
            if (pZone)
            {
                pZone->OnEndOverlap(pObjectA->GetHandle());
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------