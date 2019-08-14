namespace traits {

// Remove deepest const removes the const from something like 'char const *'
template <typename T>
struct remove_deepest_const_impl { typedef T type; };

template <typename T>
struct remove_deepest_const_impl<const T> { typedef T type; };

template <typename T>
struct remove_deepest_const_impl<T*>
{ typedef typename remove_deepest_const_impl<T>::type* type; };

template <typename T>
struct remove_deepest_const_impl<T* const>
{ typedef typename remove_deepest_const_impl<T>::type* const type; };

template <typename T>
struct remove_deepest_const_impl<T&>
{ typedef typename remove_deepest_const_impl<T>::type& type; };

template <typename T>
struct remove_deepest_const_impl<T&&>
{ typedef typename remove_deepest_const_impl<T>::type&& type; };

template <typename T> 
using remove_deepest_const_t = typename remove_deepest_const_impl<T>::type;

// Strip a type of its qualifiers
template<typename T>
using StripT = remove_deepest_const_t<std::decay_t<T>>;

// Compare two types, decays the types, and removes all constness
template<typename T, typename Y>
using IsSameType = std::is_same<StripT<T>, StripT<Y>>;

template<typename T, typename Y>
constexpr auto IsSameTypeV = IsSameType<T, Y>::value;

}	// namespace traits
