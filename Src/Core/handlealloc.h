#pragma once

#include "core.h"

// #include "allocator.h"
// #include "uint32_t.h"

namespace Core
{
	constexpr uint16_t kInvalidHandle = UINT16_MAX;

	///
	class HandleAlloc
	{
	public:
		///
		HandleAlloc(uint16_t _maxHandles);

		///
		~HandleAlloc();

		///
		const uint16_t* getHandles() const;

		///
		uint16_t getHandleAt(uint16_t _at) const;

		///
		uint16_t getNumHandles() const;

		///
		uint16_t getMaxHandles() const;

		///
		uint16_t alloc();

		///
		bool isValid(uint16_t _handle) const;

		///
		void free(uint16_t _handle);

		///
		void reset();

	private:
		HandleAlloc();

		///
		uint16_t* getDensePtr() const;

		///
		uint16_t* getSparsePtr() const;

		uint16_t m_numHandles;
		uint16_t m_maxHandles;
	};


    ///
	template <uint16_t MaxHandlesT>
	class HandleAllocT : public HandleAlloc
	{
	public:
		///
		HandleAllocT();

		///
		~HandleAllocT();

	private:
		uint16_t m_padding[2*MaxHandlesT];
	};
}

#include "inline/handlealloc.inl"