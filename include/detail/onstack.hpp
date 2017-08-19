#ifndef IMPL_PTR_DETAIL_ONSTACK_HPP
#define IMPL_PTR_DETAIL_ONSTACK_HPP

#include "./traits.hpp"
#include <boost/type_traits/aligned_storage.hpp>

namespace detail
{
    template<typename, size_t> struct onstack;
}

template<typename impl_type, size_t sz>
struct detail::onstack // Proof of concept
{
    struct allocator : std::allocator<impl_type>
    {
        void deallocate(impl_type*, size_t) {}
    };

    using    this_type = onstack;
    using storage_type = typename boost::aligned_storage<sz, 8>::type;
    using  traits_type = detail::copyable_traits<impl_type, allocator>;
    using   traits_ptr = typename traits_type::pointer;

   ~onstack () { if (traits_) traits_->destroy(get()); }
    onstack () {}
    onstack (this_type const& o) : traits_(o.traits_)
    {
        if (traits_)
            traits_->construct(storage_.address(), *o.get());
    }
    this_type& operator=(this_type const& o)
    {
        /**/ if (!traits_ && !o.traits_);
        else if ( traits_ &&  o.traits_) traits_->assign(get(), *o.get());
        else if ( traits_ && !o.traits_) traits_->destroy(get());
        else if (!traits_ &&  o.traits_) o.traits_->construct(storage_.address(), *o.get());

        traits_ = o.traits_;

        return *this;
    }

    template<typename derived_type, typename... arg_types>
    void
    emplace(arg_types&&... args)
    {
        static_assert(sizeof(derived_type) <= sz, "");

        ::new (storage_.address()) derived_type(std::forward<arg_types>(args)...);
        traits_ = traits_type();
    }
    impl_type* get () const { return traits_ ? (impl_type*) storage_.address() : nullptr; }

    private:

    storage_type storage_;
    traits_ptr    traits_ = nullptr;
};

#endif // IMPL_PTR_DETAIL_ONSTACK_HPP
