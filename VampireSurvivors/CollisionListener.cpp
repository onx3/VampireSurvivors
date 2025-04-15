#include "AstroidsPrivate.h"
#include "CollisionListener.h"
#include "GameObject.h"
#include "ProjectileComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"

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
       /* auto pHealthComp = pObjA->GetComponent<HealthComponent>().lock();
        if (pHealthComp)
        {
            if (pObjB->IsActive())
            {
                pHealthComp->LoseHealth(100.f);
            }
        }*/
    }
    else if (pObjA->GetTeam() == ETeam::Enemy && pObjB->GetTeam() == ETeam::Player)
    {
        /*auto pHealthComp = pObjB->GetComponent<HealthComponent>().lock();
        if (pHealthComp)
        {
            if (pObjB->IsActive())
            {
                pHealthComp->LoseHealth(100.f);
            }
        }*/
    }
    // Persistant Obj
    else if (pObjA->GetTeam() == ETeam::FriendlyPersistant && pObjB->GetTeam() == ETeam::Enemy)
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
            }
        }
    }
    else if (pObjA->GetTeam() == ETeam::Enemy && pObjB->GetTeam() == ETeam::FriendlyPersistant)
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
    // Nuke Drop
    else if (pObjA->GetTeam() == ETeam::Player && pObjB->GetTeam() == ETeam::NukeDrop)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            mpGameManager->GetManager<EnemyAIManager>()->DestroyAllEnemies();
            pObjB->Deactivate();
        }
    }
    else if (pObjA->GetTeam() == ETeam::NukeDrop && pObjB->GetTeam() == ETeam::Player)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            mpGameManager->GetManager<EnemyAIManager>()->DestroyAllEnemies();
            pObjB->Deactivate();
        }
    }
    // Life Drop
    else if (pObjA->GetTeam() == ETeam::Player && pObjB->GetTeam() == ETeam::LifeDrop)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            auto pObjAHealthComp = pObjA->GetComponent<HealthComponent>().lock();
            if (pObjAHealthComp)
            {
                pObjAHealthComp->AddLife(1);
            }
            pObjB->Destroy();
        }
    }
    else if (pObjA->GetTeam() == ETeam::LifeDrop && pObjB->GetTeam() == ETeam::Player)
    {
        if (pObjA->IsActive() && pObjB->IsActive())
        {
            auto pObjBHealthComp = pObjB->GetComponent<HealthComponent>().lock();
            if (pObjBHealthComp)
            {
                pObjBHealthComp->AddLife(1);
            }
            pObjA->Destroy();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionListener::EndContact(b2Contact * contact)
{
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------