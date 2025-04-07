#pragma once

class GameObject;
class GameManager;

class GameComponent
{
public:
    explicit GameComponent(GameObject * pOwner, GameManager & gameManager);
    virtual ~GameComponent() = default;

    void SetOwner(GameObject * pOwner);

    GameObject & GetGameObject() const;
    GameManager & GetGameManager() const;

    virtual void Update(float deltaTime) = 0;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states);
    virtual void DebugImGuiComponentInfo();
    virtual std::string & GetClassName();

protected:
    BD::Handle mOwnerHandle;
    GameManager & mGameManager;
private:
    std::string mName;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------