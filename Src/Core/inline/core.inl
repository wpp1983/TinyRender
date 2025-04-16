namespace Core
{

	/// Smallest normalized positive floating-point number.
	constexpr float    kFloatSmallest  = 1.175494351e-38f;

		/// Maximum representable floating-point number.
	constexpr float    kFloatLargest   = 3.402823466e+38f;

		/// Smallest normalized positive double-precision floating-point number.
	constexpr double   kDoubleSmallest = 2.2250738585072014e-308;

	/// Largest representable double-precision floating-point number.
	constexpr double   kDoubleLargest  = 1.7976931348623158e+308;

	// Reference(S):
	// - https://web.archive.org/web/20181115035420/http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
	//
	template<typename Ty, size_t NumT>
	char (&CountOfRequireArrayArgumentT(const Ty (&)[NumT]) )[NumT];

	template<bool B>
	struct isEnabled
	{
		// Template for avoiding MSVC: C4127: conditional expression is constant
		static constexpr bool value = B;
	};




	template<typename Ty>
	struct LimitsT<Ty, true>
	{
		static constexpr Ty max = ( ( (Ty(1) << ( (sizeof(Ty) * 8) - 2) ) - Ty(1) ) << 1) | Ty(1);
		static constexpr Ty min = -max - Ty(1);
	};

	template<typename Ty>
	struct LimitsT<Ty, false>
	{
		static constexpr Ty min = 0;
		static constexpr Ty max = Ty(-1);
	};

		template<>
	struct LimitsT<float, true>
	{
		static constexpr float min = -kFloatLargest;
		static constexpr float max =  kFloatLargest;
	};

	template<>
	struct LimitsT<double, true>
	{
		static constexpr double min = -kDoubleLargest;
		static constexpr double max =  kDoubleLargest;
	};

	template<typename Ty>
	inline constexpr Ty max()
	{
		return LimitsT<Ty>::max;
	}

	template<typename Ty>
	inline constexpr Ty min()
	{
		return LimitsT<Ty>::min;
	}

		template<typename Ty>
	inline constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b)
	{
		return _a < _b ? _a : _b;
	}

	template<typename Ty>
	inline constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b)
	{
		return _a > _b ? _a : _b;
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty min(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return min(min(_a, _b), _args...);
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty max(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return max(max(_a, _b), _args...);
	}

	template<typename Ty, typename... Args>
	inline constexpr Ty mid(const Ty& _a, const TypeIdentityType<Ty>& _b, const Args&... _args)
	{
		return max(min(_a, _b), min(max(_a, _b), _args...) );
	}

	template<typename Ty>
	inline constexpr Ty clamp(const Ty& _a, const TypeIdentityType<Ty>& _min, const TypeIdentityType<Ty>& _max)
	{
		return max(min(_a, _max), _min);
	}
}

