/*
 * TypeList.h
 *
 *  Created on: 19.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef TYPELIST_H_
#define TYPELIST_H_


struct NullType {};

template <typename T, typename U>
struct Typelist
{
	typedef T Head;
	typedef U Tail;
};


#define TYPELIST_1(T1) 										Typelist<T1, NullType>
#define TYPELIST_2(T1, T2) 									Typelist<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) 								Typelist<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) 							Typelist<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) 						Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) 					Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) 				Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) 			Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) 		Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)	Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)	Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >



namespace TL
{
	// Length
	template <typename TList> struct Length;

	template <>
	struct Length<NullType>
	{
		enum { value = 0 };
	};

	template <typename T, typename U>
	struct Length< Typelist<T, U> >
	{
		enum { value = 1 + Length<U>::value };
	};

	// TypeAt
	template <typename TList, unsigned int index> struct TypeAt;

	template <typename Head, typename Tail>
	struct TypeAt<Typelist<Head, Tail>, 0>
	{
		typedef Head Result;
	};

	template <typename Head, typename Tail, unsigned int i>
	struct TypeAt<Typelist<Head, Tail>, i>
	{
		typedef typename TypeAt<Tail, i - 1>::Result Result;
	};

	// IndexOf
	template <typename TList, typename T> struct IndexOf;

	//template <typename T>
	//struct IndexOf<NullType, T>
	//{
	//	enum { value = -1 };
	//};

	template <typename T, typename Tail>
	struct IndexOf<Typelist<T, Tail>, T>
	{
		enum { value = 0 };
	};

	template <typename Head, typename Tail, typename T>
	struct IndexOf<Typelist<Head, Tail>, T>
	{
	private:
		enum { temp = IndexOf<Tail, T>::value };
	public:
		enum { value = (temp == -1 ? -1 : 1 + temp) };
	};
}


#endif /* TYPELIST_H_ */
