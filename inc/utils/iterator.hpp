#pragma once

#include <string_view>

namespace d1::utils::iterator
{
using namespace std::literals;

constexpr auto MODULE_NAME{"utils/iterator.hpp"sv};

// Why the std::back_insert_iterator not constexpr?
template <typename Container>
class back_insert_iterator
{
public:
    constexpr explicit back_insert_iterator(Container& cont) : _cont(cont){};

    constexpr back_insert_iterator& operator=(const typename Container::value_type& value)
    {
        _cont.push_back(value);
        return *this;
    }

    constexpr back_insert_iterator& operator*()
    {
        return *this;
    }

    constexpr back_insert_iterator& operator++()
    {
        return *this;
    }

    constexpr back_insert_iterator& operator++(int)
    {
        return *this;
    }

private:
    Container& _cont;
};

}  // namespace d1::utils::iterator