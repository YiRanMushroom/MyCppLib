module;

#include <cassert>

export module ResourceManager;

import std.compat;

export namespace Std {
    template<typename T>
    // T should be trivially copyable, trivially move constructible and should have a non-trivial destructor
    class UniqueHolder {

#pragma region Asserts
#ifdef __cpp_concepts
    static_assert(requires(T* t) {
        t->destroy();
    }, "Template argument either does not have a destroy method or it is not accessible");
#endif
#pragma endregion

    private:
        T m_Resource;

        bool m_HoldsResource;

    public:
        UniqueHolder(UniqueHolder &) = delete;

        UniqueHolder(UniqueHolder &&other) noexcept : m_Resource{std::move(other.m_Resource)}, m_HoldsResource{true} {
            assert(other.m_HoldsResource);
            other.m_HoldsResource = false;
        }

        UniqueHolder &operator=(UniqueHolder &) = delete;

        UniqueHolder &operator=(UniqueHolder &&other) noexcept {
            if (this != &other) {
                other.m_HoldsResource = false;
                m_Resource = std::move(other.m_Resource);
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
        UniqueHolder() : m_Resource{}, m_HoldsResource{true} {
        }

        template<typename... Args,
        typename = std::enable_if_t<!std::is_same_v<std::decay_t<Args>..., UniqueHolder>, void> // Don't think this is necessary
        >
        UniqueHolder(Args&&... args) : m_Resource{std::forward<decltype(args)>(args)...}, m_HoldsResource{true} {
        }

    public:
        static UniqueHolder makeUnique(auto &&... args) {
            return UniqueHolder(
                std::forward<decltype(args)>(args)...);
        }
    };

    template<typename T>
    UniqueHolder<T> makeUnique(auto &&... args) {
        return UniqueHolder<T>::makeUnique(std::forward<decltype(args)>(args)...);
    }
};
