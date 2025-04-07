#pragma once

template<class T>
class TReusePool
{
public:
	TReusePool(size_t capacity)
	{
		mFree.reserve(capacity);
		mActive.reserve(capacity);
	}

	void AddToPool(T * pObject)
	{
		mFree.push_back(object);
	}

	T * Acquire()
	{
		if (mFree.empty())
		{
			return nullptr;
		}

		T * pObj = mFree.back();
		mFree.pop_back();
		mActive.push_back(pObj);

		if (constexpr (requires(T * pO)
		{
			pO->OnActivate();
		}))
		{
			pObj->OnActivate();
		}

		return pObj;
	}

	void Release(T * pObject)
	{
		auto it = std::find(mActive.begin(), mActive.end(), pObject);
		if (it != mActive.end())
		{
			if constexpr (requires(T * o)
			{
				o->OnDeactivate();
			})
				pObject->OnDeactivate();
			mActive.erase(it);
			mFree.push_back(pObject);
		}
	}

	const std::vector<T *> & GetActive() const
	{
		return mActive;
	}

private:
	std::vector<T *> mFree;
	std::vector<T *> mActive;
};

