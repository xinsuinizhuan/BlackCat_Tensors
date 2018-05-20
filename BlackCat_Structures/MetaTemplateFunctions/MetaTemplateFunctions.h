/*
 * BC_MetaTemplate_Simple.h
 *
 *  Created on: Dec 12, 2017
 *      Author: joseph
 */

#ifndef SIMPLE_H_
#define SIMPLE_H_
#include <type_traits>
/*
 * THIS IS TAKEN FROM BLACKCAT_TENSORS
 * THE NEWEST VERSION OF THIS FILE WILL BE IN:
 *
 * BlackCat_Tensors3.3/BC_Internals_BC_Tensor_Base/BC_Tensor_Types/BC_Utility/MetaTemplateFunctions.h
 *
 * This file is simply in this directory for other people to access more easily
 */

namespace BC {
namespace MTF {

	template<class T> struct isTrue 				 { static constexpr bool conditional = true; };
	template<> 		  struct isTrue<std::false_type> { static constexpr bool conditional = false; };

	template<class T> auto& cc(const T& var) { return const_cast<T&>(var); }

		//EQUIVALENT OF std::conditional
		template<bool iff, class THEN, class ELSE> struct IF_ELSE { using type = THEN; };
		template<class THEN, class ELSE> 		   struct IF_ELSE<false, THEN, ELSE> { using type = ELSE;};
		//shorthand of std::conditional
		template<bool var, class a, class b>
		using ifte = typename std::conditional<var, a, b>::type; //ifte -- if than, else

		template<class var, class... lst			 > struct isOneOf 					{ static constexpr bool conditional = false; };
		template<class var, class... lst			 > struct isOneOf<var,var,lst...> 	{ static constexpr bool conditional = true; };
		template<class var, class front, class... lst> struct isOneOf<var,front,lst...> { static constexpr bool conditional = isOneOf<var, lst...>::conditional; };


	template<class> struct isPrimitive 					{ static constexpr bool conditional = false; };
	template<> struct isPrimitive<bool> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<short> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<unsigned short> 		{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<int> 					{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<unsigned> 			{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<long> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<unsigned long> 		{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<long long> 			{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<unsigned long long> 	{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<char> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<unsigned char>		{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<float> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<double> 				{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<long double> 			{ static constexpr bool conditional = true; };
	template<> struct isPrimitive<wchar_t> 				{ static constexpr bool conditional = true; };

	template<class T>
	static constexpr bool isPrim = MTF::isPrimitive<T>::conditional;

	template<class... T> struct isIntList {
		static constexpr bool conditional = true;
	};
	template<class T, class... Ts> struct isIntList<T,Ts...> {
		static constexpr bool conditional = false;
	};
	template<class... Ts> struct isIntList<int,Ts...> {
		static constexpr bool conditional = true && isIntList<Ts...>::conditional;
	};

	template<class... ts>
	static constexpr bool is_integer_sequence = isIntList<ts...>::conditional;

	template<class...> struct isTypeList { static constexpr bool conditional = false; };

	template<class> struct lst;

	template<class T> struct lst {
		using front = T;
		using last = T;
	};
	template<template<class...> class LIST, class T, class... V>
	struct lst<LIST<T, V...>>  {
		using front = T;
		using last = T;
	};
	template<template<class...> class LIST, class T, class V>
	struct lst<LIST<T, V>>  {
		using front = T;
		using last = V;
	};

	template<template<class...> class LIST, class T, class n, class... V>
	struct lst<LIST<T, n, V...>> {
		using front = T;
		using last = typename lst<LIST<n,V...>>::last;
	};


	template<class, class> struct same  { static constexpr bool conditional = false; };
	template<class T> struct same<T, T> { static constexpr bool conditional = true; };

	//If the collection type is the same (but the inside type is different IE std::vector<int> and std::vector<double> will be true
	template<class, class> 	struct same_shell  	   	{ static constexpr bool conditional = false; };
	template<class T>  		struct same_shell<T, T> { static constexpr bool conditional = true;  };
	template<template <class...> class T, class... p1, class... p2>
	struct same_shell<T<p1...>, T<p2...>> { static constexpr bool conditional = true;  };


	//same as same_shell but accepts two template_types opposed to "filled" types
	template<template<class...> class, 	 template<class...> class  > struct same_empty_shell 	  { static constexpr bool conditional = false; };
	template<template<class...> class T> struct same_empty_shell<T,T> { static constexpr bool conditional = true; };

	template<template<class...> class T, template<class...> class U>
	static constexpr bool same_empty_shell_b = same_empty_shell<T, U>::conditional;

	//get front of list
	template<class> struct front;
	template<template<class... > class set, class T, class... s>
	struct front<set<T, s...>> {
		using type = T;
	};

	//get second of list (or tail in haskell)
	template<class> struct second;
	template<template<class... > class set, class T, class U, class... s>
	struct second<set<T, U, s...>> {
		using type = U;
	};

	template<class T>
	using head = typename lst<T>::front;

	template<class T>
	using tail = typename lst<T>::last;

	template<class T>
	using front_t = typename front<T>::type;

	template<class T>
	using second_t = typename second<T>::type;

	template<class> struct shell_of;
	template<template<class ...> class param, class... ptraits>
	struct shell_of<param<ptraits...>> { template<class... ntraits> using type = param<ntraits...>; };


	//Determines if type A is castable to type B, obviously must be complete type
	template<class from, class to, class voider = void>
	struct castable {
		static constexpr bool conditional= false;
	};
	template<class from, class to>
	struct castable<from, to,  decltype(static_cast<to>(std::declval<from>()))> {
		static constexpr bool conditional= true;
	};

	template<class F, class T>
	static constexpr bool castable_b = castable<F, T>::conditional;


	}
}
#endif /* SIMPLE_H_ */