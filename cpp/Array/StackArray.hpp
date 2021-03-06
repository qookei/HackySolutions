#pragma once

#include <utility>
#include <stdexcept>

#include "../InitializerList/InitializerList.hpp"

namespace hsd
{
    template< typename T, size_t N >
    class stack_array
    {
    private:
        T _array[N];

    public:
        using iterator = T*;
        using const_iterator = const T*;

        stack_array() = default;

        template< typename L, typename... U >
        constexpr stack_array(const L& value, const U&... values)
        {
            T arr[] = {value, values...};
            std::copy(arr, arr + N, begin());
        }

        constexpr stack_array(T* data)
        {
            std::copy(data, data + N, _array);
        }

        constexpr stack_array(const stack_array& other)
        {
            std::copy(other.begin(), other.end(), begin());
        }

        constexpr stack_array& operator=(const stack_array& rhs)
        {
            std::copy(rhs.begin(), rhs.end(), begin());
            return *this;
        }

        constexpr stack_array& operator=(initializer_list<T>&& rhs)
        {
            if(rhs.size() != N)
            {
                throw std::out_of_range("");
            }

            std::move(rhs.begin(), rhs.begin() + N, _array);
            return *this;
        }

        constexpr T& operator[](size_t index)
        {
            return _array[index];
        }

        constexpr T& at(size_t index)
        {
            if(index >= N)
            {
                throw std::out_of_range("");
            }

            return _array[index];
        }

        template< size_t U, size_t L >
        constexpr auto gen_range()
        {
            static_assert(L - U <= N, "Out of range\n");

            return stack_array<T, L - U>(&_array[U]);
        }

        constexpr size_t size()
        {
            return N;
        }

        constexpr iterator data()
        {
            return _array;
        }

        constexpr iterator begin()
        {
            return data();
        }

        constexpr iterator end()
        {
            return begin() + size();
        }

        constexpr const_iterator cbegin()
        {
            return begin();
        }

        constexpr const_iterator cend()
        {
            return end();
        }
    };

    template< typename L, typename... U > stack_array(const L&, const U&...) -> stack_array<L, 1 + sizeof...(U)>;
}