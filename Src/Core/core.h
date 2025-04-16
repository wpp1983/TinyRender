#pragma once

#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t

#include "platform.h"
#include "macros.h"
#include "typetraits.h"

///
#define CORE_COUNTOF(_x) sizeof(Core::CountOfRequireArrayArgumentT(_x) )

///
#define CORE_OFFSETOF(_type, _member) \
	(reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<_type*>(16)->_member) )-ptrdiff_t(16) )


namespace Core
{

	/// Arithmetic type `Ty` limits.
	template<typename Ty, bool SignT = isSigned<Ty>()>
	struct LimitsT;


	/// Swap two values.
	template<typename Ty>
	void swap(Ty& _a, Ty& _b);

	/// Swap memory.
	void swap(void* _a, void* _b, size_t _numBytes);


	/// Returns numeric minimum of type.
	template<typename Ty>
	constexpr Ty min();

	/// Returns numeric maximum of type.
	template<typename Ty>
	constexpr Ty max();

	/// Returns minimum of two values.
	template<typename Ty>
	constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b);

	/// Returns maximum of two values.
	template<typename Ty>
	constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b);

	/// Returns minimum of three or more values.
	template<typename Ty, typename... Args>
	constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args);

	/// Returns maximum of three or more values.
	template<typename Ty, typename... Args>
	constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args);

	/// Returns middle of three or more values.
	template<typename Ty, typename... Args>
	constexpr Ty mid(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args);

	/// Returns clamped value between min/max.
	template<typename Ty>
	constexpr Ty clamp(const Ty& _a, const TypeIdentityType<Ty>& _min, const TypeIdentityType<Ty>& _max);

		/// Copy memory block.
	///
	/// @param _dst Destination pointer.
	/// @param _src Source pointer.
	/// @param _numBytes Number of bytes to copy.
	///
	/// @remark Source and destination memory blocks must not overlap.
	///
	void memCopy(void* _dst, const void* _src, size_t _numBytes);

	/// Copy strided memory block.
	///
	/// @param _dst Destination pointer.
	/// @param _dstStride Destination stride.
	/// @param _src Source pointer.
	/// @param _srcStride Source stride.
	/// @param _stride Number of bytes per stride to copy.
	/// @param _numStrides Number of strides.
	///
	/// @remark Source and destination memory blocks must not overlap.
	///
	void memCopy(
		  void* _dst
		, uint32_t _dstStride
		, const void* _src
		, uint32_t _srcStride
		, uint32_t _stride
		, uint32_t _numStrides
		);

	/// Copy memory block.
	///
	/// @param _dst Destination pointer.
	/// @param _src Source pointer.
	/// @param _numBytes Number of bytes to copy.
	///
	/// @remark If source and destination memory blocks overlap memory will be copied in reverse
	///   order.
	///
	void memMove(void* _dst, const void* _src, size_t _numBytes);

	/// Copy strided memory block.
	///
	/// @param _dst Destination pointer.
	/// @param _dstStride Destination stride.
	/// @param _src Source pointer.
	/// @param _srcStride Source stride.
	/// @param _stride Number of bytes per stride to copy.
	/// @param _numStrides Number of strides.
	///
	/// @remark If source and destination memory blocks overlap memory will be copied in reverse
	///   order.
	///
	void memMove(
		  void* _dst
		, uint32_t _dstStride
		, const void* _src
		, uint32_t _srcStride
		, uint32_t _stride
		, uint32_t _numStrides
		);


	/// Fill memory block to specified value `_ch`.
	///
	/// @param _dst Destination pointer.
	/// @param _ch Fill value.
	/// @param _numBytes Number of bytes to copy.
	///
	void memSet(void* _dst, uint8_t _ch, size_t _numBytes);

	/// Fill strided memory block to specified value `_ch`.
	///
	/// @param _dst Destination pointer.
	/// @param _dstStride Destination stride.
	/// @param _ch Fill value.
	/// @param _stride Number of bytes per stride to copy.
	/// @param _numStrides Number of strides.
	///
	void memSet(
		  void* _dst
		, uint32_t _dstStride
		, uint8_t _ch
		, uint32_t _stride
		, uint32_t _numStrides
		);

	/// Compare two memory blocks.
	///
	/// @param _lhs Pointer to memory block.
	/// @param _rhs Pointer to memory block.
	/// @param _numBytes Number of bytes to compare.
	///
	/// @returns 0 if two blocks are identical, positive value if first different byte in `_lhs` is
	///   greater than corresponding byte in `_rhs`.
	///
	int32_t memCmp(const void* _lhs, const void* _rhs, size_t _numBytes);

} // namespace Core


#include "inline/core.inl"