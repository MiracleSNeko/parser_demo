#pragma once

namespace d1::utils::iterator
{

// Why the std::back_insert_iterator not constexpr?
template <typename Container>
class back_insert_iterator
{
public:
    constexpr explicit back_insert_iterator(Container& cont) : _cont(cont){};

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