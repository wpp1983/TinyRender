namespace Core
{
    inline HandleAlloc::HandleAlloc(uint16_t _maxHandles)
    : m_numHandles(0)
    , m_maxHandles(_maxHandles)
    {
        reset();
    }

    inline HandleAlloc::~HandleAlloc()
    {

    }
    
    inline const uint16_t* HandleAlloc::getHandles() const
	{
		return getDensePtr();
	}

	inline uint16_t HandleAlloc::getHandleAt(uint16_t _at) const
	{
		return getDensePtr()[_at];
	}

    	inline uint16_t HandleAlloc::getNumHandles() const
	{
		return m_numHandles;
	}

	inline uint16_t HandleAlloc::getMaxHandles() const
	{
		return m_maxHandles;
	}

    inline uint16_t HandleAlloc::alloc()
	{
		if (m_numHandles < m_maxHandles)
		{
			uint16_t index = m_numHandles;
			++m_numHandles;

			uint16_t* dense  = getDensePtr();
			uint16_t  handle = dense[index];
			uint16_t* sparse = getSparsePtr();
			sparse[handle] = index;
			return handle;
		}

		return kInvalidHandle;
	}

	inline bool HandleAlloc::isValid(uint16_t _handle) const
	{
		uint16_t* dense  = getDensePtr();
		uint16_t* sparse = getSparsePtr();
		uint16_t  index  = sparse[_handle];

		return index < m_numHandles
			&& dense[index] == _handle
			;
	}

    	inline void HandleAlloc::free(uint16_t _handle)
	{
		uint16_t* dense  = getDensePtr();
		uint16_t* sparse = getSparsePtr();
		uint16_t index = sparse[_handle];
		--m_numHandles;
		uint16_t temp = dense[m_numHandles];
		dense[m_numHandles] = _handle;
		sparse[temp] = index;
		dense[index] = temp;
	}

	inline void HandleAlloc::reset()
	{
		m_numHandles = 0;
		uint16_t* dense = getDensePtr();
		for (uint16_t ii = 0, num = m_maxHandles; ii < num; ++ii)
		{
			dense[ii] = ii;
		}
	}

    	inline uint16_t* HandleAlloc::getDensePtr() const
	{
		uint8_t* ptr = (uint8_t*)reinterpret_cast<const uint8_t*>(this);
		return (uint16_t*)&ptr[sizeof(HandleAlloc)];
	}

	inline uint16_t* HandleAlloc::getSparsePtr() const
	{
		return &getDensePtr()[m_maxHandles];
	}

    	template <uint16_t MaxHandlesT>
	inline HandleAllocT<MaxHandlesT>::HandleAllocT()
		: HandleAlloc(MaxHandlesT)
	{
	}

	template <uint16_t MaxHandlesT>
	inline HandleAllocT<MaxHandlesT>::~HandleAllocT()
	{
	}
}