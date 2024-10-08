module;

#include <cassert>

export module ResourceManager;

import com.yrm.libcpp.std.compat;

export namespace Std {
    template<typename T>
    // T should be trivially copyable, trivially move constructible and should have a trivial destructor
    class UniqueHolder {
#pragma region Asserts
#ifdef __cpp_concepts
        static_assert(requires(T *t)
        {
            t->destroy();
        }, "Template argument either does not have a destroy method or it is not accessible");
#endif
#pragma endregion

    private:
        T m_Resource;

        bool m_HoldsResource;

    public:
        T drop() {
            m_HoldsResource = false;
            return std::move(m_Resource);
        }

        UniqueHolder(const UniqueHolder &) = delete;

        UniqueHolder(UniqueHolder &&other) noexcept : m_Resource{other.drop()}, m_HoldsResource{true} {
        }

        UniqueHolder &operator=(const UniqueHolder &) = delete;

        UniqueHolder &operator=(UniqueHolder &&other) noexcept {
            if (this != &other) {
                m_Resource.destroy();
                m_Resource = other.drop();
            }
            return *this;
        }

    public:
        ~UniqueHolder() {
            if (m_HoldsResource)
                m_Resource.destroy();
        }

        T *operator->() {
            return &(this->m_Resource);
        }

        const T *operator->() const {
            return &(this->m_Resource);
        }

        T &operator*() {
            return this->m_Resource;
        }

        const T &operator*() const {
            return this->m_Resource;
        }

        T &get() {
            return this->m_Resource;
        }

        const T &get() const {
            return this->m_Resource;
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        operator bool() const {
            return m_HoldsResource;
        }

    private:
        template<typename First, typename... Args
            , typename = std::enable_if_t<!std::is_same_v<std::decay_t<First>, UniqueHolder>>
            // Don't think this is necessary
        >
        explicit UniqueHolder(First &&first, Args &&... args) : m_Resource(std::forward<First>(first),
                                                                           std::forward<decltype(args)>(args)...),
                                                                m_HoldsResource(true) {
        }

    public:
        static UniqueHolder makeUnique(auto &&... args) {
            return UniqueHolder(
                std::forward<decltype(args)>(args)...);
        }
    };

    template<typename T>
    UniqueHolder<T> makeUniqueHolder(auto &&... args) {
        return UniqueHolder<T>::makeUnique(std::forward<decltype(args)>(args)...);
    }


    template<typename T>
    // T should be trivially copyable, trivially move constructible and should have a trivial destructor
    class VirtualUniqueHolder {
#pragma region Asserts
#ifdef __cpp_concepts
        static_assert(requires(T *t)
        {
            t->destroy();
        }, "Template argument either does not have a destroy method or it is not accessible");
#endif
#pragma endregion

    private:
        std::unique_ptr<T> m_Resource;

        bool m_HoldsResource;

    public:
        std::unique_ptr<T> drop() {
            m_HoldsResource = false;
            return std::move(m_Resource);
        }

        VirtualUniqueHolder(const VirtualUniqueHolder &) = delete;

        VirtualUniqueHolder(VirtualUniqueHolder &&other) noexcept : m_Resource{std::move(other.m_Resource)},
                                                                    m_HoldsResource{true} {
            assert(other.m_HoldsResource);
            other.m_HoldsResource = false;
        }

        template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U> > >
        VirtualUniqueHolder(VirtualUniqueHolder<U> &&other) noexcept : m_Resource{other.drop()}, m_HoldsResource{true} {
        }

        VirtualUniqueHolder &operator=(const VirtualUniqueHolder &) = delete;

        VirtualUniqueHolder &operator=(VirtualUniqueHolder &&other) noexcept {
            if (this != &other) {
                m_Resource->destroy();
                m_Resource = other.drop();
            }
            return *this;
        }

        template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U> > >
        VirtualUniqueHolder &operator=(VirtualUniqueHolder<U> &&other) noexcept {
            if (this != &other) {
                m_Resource->destroy();
                m_Resource = other.drop();
            }
            return *this;
        }

    public:
        ~VirtualUniqueHolder() {
            if (m_HoldsResource)
                m_Resource->destroy();
        }

        T *operator->() {
            return this->m_Resource.get();
        }

        const T *operator->() const {
            return this->m_Resource.get();
        }

        T &operator*() {
            return *(this->m_Resource);
        }

        const T &operator*() const {
            return *(this->m_Resource);
        }

        T &get() {
            return *(this->m_Resource);
        }

        const T &get() const {
            return *(this->m_Resource);
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        operator bool() const {
            return m_HoldsResource;
        }

    private:
        template<typename First, typename... Args
            , typename = std::enable_if_t<!std::is_same_v<std::decay_t<First>, VirtualUniqueHolder>>
            // Don't think this is necessary
        >
        explicit VirtualUniqueHolder(First &&first, Args &&... args) : m_Resource(std::make_unique<T>(
                                                                           std::forward<First>(first),
                                                                           std::forward<decltype(args)>(args)...)),
                                                                       m_HoldsResource(true) {
        }

    public:
        static VirtualUniqueHolder makeVirtualUnique(auto &&... args) {
            return VirtualUniqueHolder(
                std::forward<decltype(args)>(args)...);
        }
    };

    template<typename T>
    VirtualUniqueHolder<T> makeVirtualUniqueHolder(auto &&... args) {
        return VirtualUniqueHolder<T>::makeVirtualUnique(std::forward<decltype(args)>(args)...);
    }
};
