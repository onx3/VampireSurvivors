#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <limits>
#include <intrin.h> // For _BitScanForward64

namespace BD
{
    typedef unsigned long uint32;
    typedef unsigned long long uint64;
    typedef uint64 Handle;
}

template<class T, size_t MAX_SIZE = 1024>
class TPool
{
public:
    TPool()
    {
        memset(mOccupancyChunks, 0, sizeof(mOccupancyChunks));
    }

    BD::Handle AddObject(T * obj)
    {
        BD::uint32 index = FindFirstFreeIndex();
        if (index == MAX_SIZE)
        {
            assert(false && "Overflow object pool");
        }

        mStorage[index] = obj;
        mIsOccupied.set(index);
        mOccupancyChunks[index / 64] |= (1ULL << (index % 64)); // Set bit in chunk
        ++mVersions[index];

        return PackHandle(index, mVersions[index]);
    }

    T * Get(BD::Handle handle)
    {
        BD::uint32 index = ExtractIndex(handle);
        BD::uint32 version = ExtractVersion(handle);

        if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version || !mStorage[index])
        {
            return nullptr;
        }

        return mStorage[index];
    }

    void Remove(BD::Handle handle)
    {
        BD::uint32 index = ExtractIndex(handle);
        BD::uint32 version = ExtractVersion(handle);

        if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version)
        {
            return;
        }

        mStorage[index] = nullptr;
        mIsOccupied.reset(index);
        mOccupancyChunks[index / 64] &= ~(1ULL << (index % 64)); // Clear bit in chunk
        ++mVersions[index];
    }

private:
    inline BD::Handle PackHandle(uint32 index, uint32 version)
    {
        return (static_cast<BD::Handle>(version) << 32) | index;
    }

    inline uint32 ExtractIndex(BD::Handle handle)
    {
        return static_cast<uint32>(handle & 0xFFFFFFFF);
    }

    inline uint32 ExtractVersion(BD::Handle handle)
    {
        return static_cast<uint32>((handle >> 32) & 0xFFFFFFFF);
    }

    BD::uint32 FindFirstFreeIndex()
    {
        for (BD::uint32 chunkIdx = 0; chunkIdx < MAX_SIZE / 64; ++chunkIdx)
        {
            if (mOccupancyChunks[chunkIdx] != std::numeric_limits<uint64>::max()) // Not the max
            {
                BD::uint32 bitIdx = FindFirstClearBit(mOccupancyChunks[chunkIdx]);
                return (chunkIdx * 64) + bitIdx;
            }
        }
        return MAX_SIZE; // No free slot found
    }

    BD::uint32 FindFirstClearBit(BD::uint64 chunk)
    {
        unsigned long index;
        if (_BitScanForward64(&index, ~chunk)) // Finds first clear bit
        {
            return static_cast<uint32>(index);
        }
        return 64; // No free bits
    }

    T * mStorage[MAX_SIZE] = { nullptr };
    std::bitset<MAX_SIZE> mIsOccupied;
    BD::uint32 mVersions[MAX_SIZE] = { 0 };
    BD::uint64 mOccupancyChunks[MAX_SIZE / 64] = {};
};
