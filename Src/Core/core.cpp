#include "core.h"

#if !BX_CRT_NONE
#	include <string.h> // memcpy, memmove, memset
#endif // !BX_CRT_NONE

namespace Core {

void memCopy(void *_dst, const void *_src, size_t _numBytes) {
#if BX_CRT_NONE
		memCopyRef(_dst, _src, _numBytes);
#else
		::memcpy(_dst, _src, _numBytes);
#endif // BX_CRT_NONE
}

void memCopy(void *_dst, uint32_t _dstStride, const void *_src,
             uint32_t _srcStride, uint32_t _stride, uint32_t _numStrides) {
  if (_stride == _srcStride && _stride == _dstStride) {
    memCopy(_dst, _src, _stride * _numStrides);
    return;
  }

  const uint8_t *src = (const uint8_t *)_src;
  uint8_t *dst = (uint8_t *)_dst;

  for (uint32_t ii = 0; ii < _numStrides;
       ++ii, src += _srcStride, dst += _dstStride) {
    memCopy(dst, src, _stride);
  }
}

	void memMove(void* _dst, const void* _src, size_t _numBytes)
	{
#if BX_CRT_NONE
		memMoveRef(_dst, _src, _numBytes);
#else
		::memmove(_dst, _src, _numBytes);
#endif // BX_CRT_NONE
	}

	void memMove(
		  void* _dst
		, uint32_t _dstStride
		, const void* _src
		, uint32_t _srcStride
		, uint32_t _stride
		, uint32_t _numStrides
		)
	{
		if (_stride == _srcStride
		&&  _stride == _dstStride)
		{
			memMove(_dst, _src, _stride*_numStrides);
			return;
		}

		const uint8_t* src = (const uint8_t*)_src;
		      uint8_t* dst = (uint8_t*)_dst;

		for (uint32_t ii = 0; ii < _numStrides; ++ii, src += _srcStride, dst += _dstStride)
		{
			memMove(dst, src, _stride);
		}
	}

	void memSetRef(void* _dst, uint8_t _ch, size_t _numBytes)
	{
		uint8_t* dst = (uint8_t*)_dst;
		const uint8_t* end = dst + _numBytes;
		while (dst != end)
		{
			*dst++ = char(_ch);
		}
	}

	void memSet(void* _dst, uint8_t _ch, size_t _numBytes)
	{
#if BX_CRT_NONE
		memSetRef(_dst, _ch, _numBytes);
#else
		::memset(_dst, _ch, _numBytes);
#endif // BX_CRT_NONE
	}

	void memSet(void* _dst, uint32_t _dstStride, uint8_t _ch, uint32_t _stride, uint32_t _num)
	{
		if (_stride == _dstStride)
		{
			memSet(_dst, _ch, _stride*_num);
			return;
		}

		uint8_t* dst = (uint8_t*)_dst;

		for (uint32_t ii = 0; ii < _num; ++ii, dst += _dstStride)
		{
			memSet(dst, _ch, _stride);
		}
	}

	int32_t memCmpRef(const void* _lhs, const void* _rhs, size_t _numBytes)
	{
		const char* lhs = (const char*)_lhs;
		const char* rhs = (const char*)_rhs;
		for (
			; 0 < _numBytes && *lhs == *rhs
			; ++lhs, ++rhs, --_numBytes
			)
		{
		}

		return 0 == _numBytes ? 0 : *lhs - *rhs;
	}

	int32_t memCmp(const void* _lhs, const void* _rhs, size_t _numBytes)
	{
#if BX_CRT_NONE
		return memCmpRef(_lhs, _rhs, _numBytes);
#else
		return ::memcmp(_lhs, _rhs, _numBytes);
#endif // BX_CRT_NONE
	}

} // namespace Core
