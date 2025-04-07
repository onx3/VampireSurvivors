#pragma once
#include "BaseManager.h"
#include <string>
#include <functional>

//------------------------------------------------------------------------------------------------------------------------
// ResourceId
//------------------------------------------------------------------------------------------------------------------------

class ResourceId
{
public:
    explicit ResourceId(std::string const & name);        

    size_t GetHash() const;

    std::string & GetName();

    bool operator==(ResourceId const & other) const
    {
        return mHash == other.mHash;
    }
    bool operator<(ResourceId const & other) const
    {
        return mHash < other.mHash;
    }

private:
    std::string mResourceName;
    size_t mHash;
};

//------------------------------------------------------------------------------------------------------------------------
// Hashing
//------------------------------------------------------------------------------------------------------------------------

namespace std
{
    template<>
    struct hash<ResourceId>
    {
        size_t operator() (const ResourceId & resourceId) const noexcept
        {
            return resourceId.GetHash();
        }
    };
}

//------------------------------------------------------------------------------------------------------------------------
// ResourceManager
//------------------------------------------------------------------------------------------------------------------------

class ResourceManager : public BaseManager
{
public:
	ResourceManager(GameManager * pGameManager);
	
    std::shared_ptr<sf::Texture> GetTexture(ResourceId & resourceId);

    void PreloadResources(std::vector<std::string> const & resourcePaths);

    void PreloadResources(std::vector<ResourceId> & resourceIds);
    
private:
    std::unordered_map<ResourceId, std::shared_ptr<sf::Texture>> mTextureResources;
};
