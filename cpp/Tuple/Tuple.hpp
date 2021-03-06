#pragma once

#include "../IndexSequence/IndexSequence.hpp"
#include "../Utility/Utility.hpp"
#include <utility>

namespace hsd
{
    template < typename... T >
    class tuple
    {
    public:
        static constexpr size_t size()
        {
            return sizeof...(T);
        }
    };

    template< typename... T > static constexpr tuple<T...> make_tuple(T&&... args);

    template< typename Tuple1, typename Tuple2 >
    using is_same_tuple = std::is_same<Tuple1, Tuple2>;

    template< typename T, typename... Rest >
    class tuple<T, Rest...>
    {   
    public:   
        T _first;
        tuple<Rest...> _rest;        

    private:
        template< size_t N, typename U >
        struct get_helper {};

        template< typename U, typename... Args >
        struct get_helper<0, tuple<U, Args...>>
        {
            static constexpr U get(const tuple<U, Args...>& data)
            {
                return data._first;
            }
        };

        template< size_t N, typename U, typename... Args >
        struct get_helper<N, tuple<U, Args...>>
        {
            static constexpr auto get(const tuple<U, Args...>& data)
            {
                return get_helper<N - 1, tuple<Args...>>::get(data._rest);
            }
        };

        template<size_t N, typename... U>
        static constexpr auto _get(const tuple<U...>& tup) 
        {
            return get_helper<N, tuple<U...>>::get(tup);
        }
    public:
        tuple() = default;

        constexpr tuple(const T& first, const Rest&... rest) 
            : _first(first), _rest(rest...)
        {}

        constexpr tuple(const tuple& other)
            : _first{other._first}, _rest{other._rest}
        {}

        template< typename... Args >
        constexpr tuple& operator=(const tuple<Args...>& other)
        {
            _first = other._first;
            _rest = other._rest;

            return *this;
        }

        template< typename... Args >
        constexpr auto operator+(const tuple<Args...>& rhs) 
        {
            auto _add = [&]< size_t... Ints1, size_t... Ints2>(
                index_sequence<Ints1...> int_seq1, 
                index_sequence<Ints2...> int_seq2
            ) -> tuple<T, Rest..., Args...> {

                [](auto... args)
                {
                    static_assert(is_same_tuple<tuple<T, Rest..., Args...>, tuple<decltype(args)...>>{});
                }(get<Ints1>()..., rhs.template get<Ints2>()...);

                return make_tuple<T, Rest..., Args...>(
                    get<Ints1>()..., rhs.template get<Ints2>()...
                );
            };

            return _add(index_sequence_for<T , Rest...>{}, index_sequence_for<Args...>{});
        }

        template<size_t N>
        constexpr auto get() 
        {
            return _get<N, T, Rest...>(*this);
        }

        template<size_t N>
        constexpr auto get() const
        {
            return _get<N, T, Rest...>(*this);
        }

        static constexpr size_t size()
        {
            return 1 + sizeof...(Rest);
        }
    };

    template<class... UTypes> tuple(UTypes...) -> tuple<UTypes...>;
    template<class T1, class T2> tuple(pair<T1, T2>) -> tuple<T1, T2>;

    template< typename... T >
    static constexpr tuple<T...> make_tuple(T&&... args)
    {
        return tuple<T...>(std::forward<T>(args)...);
    }

    template< typename... T >
    static constexpr tuple<T...> tie(T&... args)
    {
        return tuple<T...>(args...);
    }

    template< typename Func, typename...Args, size_t... Is >
    static constexpr auto apply_impl(Func&& func, index_sequence<Is...>, const tuple<Args...>& args) 
    {
        return func(args.template get<Is>()...);
    }
    
    template< typename Func, typename...Args >
    static constexpr auto apply(Func&& func, const tuple<Args...>& args)
    {
      return apply_impl(std::forward<Func>(func), make_index_sequence<sizeof...(Args)>{}, args);
    }
}