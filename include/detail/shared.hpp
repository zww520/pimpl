#ifndef IMPL_PTR_DETAIL_SHARED_HPP
#define IMPL_PTR_DETAIL_SHARED_HPP

#include "./first.hpp"
#include <memory>

namespace detail
{
    template<typename, typename...> struct shared;
}

template<typename impl_type, typename... more_types>
struct detail::shared : std::shared_ptr<impl_type>
{
    using   base_ref = std::shared_ptr<impl_type>&;
    using alloc_type = typename std::conditional<
                       sizeof...(more_types) == 0,
                       std::allocator<impl_type>,
                       typename first<more_types...>::type>::type;

    template<typename derived_type, typename... arg_types>
    void
    emplace(arg_types&&... args)
    {
        base_ref(*this) = std::allocate_shared<derived_type>(alloc_type(), std::forward<arg_types>(args)...);
    }
};

#endif // IMPL_PTR_DETAIL_SHARED_HPP
