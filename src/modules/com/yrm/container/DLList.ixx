export module com.yrm.container.DLList;

import com.yrm.libcpp.std.compat;

export namespace Std {
    template<typename T>
    class DLList {
        class Node;
        std::shared_ptr<Node> m_Head;
        std::weak_ptr<Node> m_Tail;

        size_t m_Size{};

        friend class Node;

    public:
        using value_type = T;

        std::shared_ptr<Node> getHead() const {
            return m_Head;
        }

        std::shared_ptr<Node> getTail() const {
            return m_Tail.lock();
        }

        [[nodiscard]] size_t size() const {
            return m_Size;
        }

    private:
        class Node {
            std::shared_ptr<Node > m_Next;
            std::weak_ptr<Node > m_Prev;
            std::shared_ptr<T> m_Value;
            DLList<T> *m_List;

            friend class DLList;

        public:
            explicit Node(DLList<T> *list) : m_List{list} {
                ++(m_List->m_Size);
            }

            std::shared_ptr<T> &getPtr() {
                return m_Value;
            }

            const std::shared_ptr<T> &getPtr() const {
                return m_Value;
            }

            T& operator*() {
                return *m_Value;
            }

            const T& operator*() const {
                return *m_Value;
            }

            T& getValue() {
                return *m_Value;
            }

            const T& getValue() const {
                return *m_Value;
            }

            void remove() {
                if (auto prev = m_Prev.lock()) {
                    prev->m_Next = m_Next;
                } else {
                    m_List->m_Head = m_Next;
                }

                if (m_Next) {
                    m_Next->m_Prev = m_Prev;
                } else {
                    m_List->m_Tail = m_Prev.lock();
                }

                --(m_List->m_Size);
            }

            std::shared_ptr<Node> getThis() const {
                if (m_Prev.lock()) {
                    return m_Prev.lock()->m_Next;
                } else {
                    return m_List->m_Head;
                }
            }

            std::shared_ptr<Node> getNext() const {
                return m_Next;
            }

            std::shared_ptr<Node> getPrev() const {
                return m_Prev.lock();
            }

            void insertAfter(const std::shared_ptr<T> &value) {
                auto newNode = std::make_shared<Node>(m_List);
                newNode->m_Value = value;
                newNode->m_Next = m_Next;
                newNode->m_Prev = getThis();
                m_Next = newNode;
                if (newNode->m_Next) {
                    newNode->m_Next->m_Prev = newNode;
                } else {
                    m_List->m_Tail = newNode;
                }
            }

            void insertBefore(const std::shared_ptr<T> &value) {
                auto newNode = std::make_shared<Node>(m_List);
                newNode->m_Value = value;
                newNode->m_Prev = m_Prev;
                newNode->m_Next = getThis();
                if (auto prev = m_Prev.lock()) {
                    prev->m_Next = newNode;
                } else {
                    m_List->m_Head = newNode;
                }
                m_Prev = newNode;
            }

            void emplaceAfter(auto &&... args) {
                insertAfter(std::make_shared<T>(std::forward<decltype(args)>(args)...));
            }

            void emplaceBefore(auto &&... args) {
                insertBefore(std::make_shared<T>(std::forward<decltype(args)>(args)...));
            }
        };

    public:
        void insertFront(const std::shared_ptr<T> value) {
            if (this->size())
                this->getHead()->insertBefore(value);
            else {
                this->m_Head = std::make_shared<Node>(this);
                this->m_Head->m_Value = value;
                this->m_Tail = this->m_Head;
                ++(this->m_Size);
            }
        }

        void insertBack(const std::shared_ptr<T> value) {
            if (this->size())
                this->getTail()->insertAfter(value);
            else {
                this->m_Head = std::make_shared<Node>(this);
                this->m_Head->m_Value = value;
                this->m_Tail = this->m_Head;
                ++(this->m_Size);
            }
        }

        void emplaceFront(auto &&... args) {
            insertFront(std::make_shared<T>(std::forward<decltype(args)>(args)...));
        }

        void emplaceBack(auto &&... args) {
            insertBack(std::make_shared<T>(std::forward<decltype(args)>(args)...));
        }

        [[nodiscard]] bool empty() const {
            return !m_Size;
        }

        // Iterator
        class Iterator {
            std::shared_ptr<Node> m_Node;

        public:
            explicit Iterator(const std::shared_ptr<Node>& node) : m_Node{node} {}
            Iterator() = default;

            operator bool() const {
                return m_Node;
            }

            T& operator*() {
                return this->getValue();
            }

            const T& operator*() const {
                return this->getValue();
            }

            T& getValue() {
                return m_Node->getValue();
            }

            const T& getValue() const {
                return m_Node->getValue();
            }

            std::shared_ptr<Node> getNode() const {
                return m_Node;
            }

            Node *operator->() const {
                return m_Node.get();
            }

            bool operator==(const Iterator &other) const {
                return m_Node.get() == other.m_Node.get();
            }

            bool operator!=(const Iterator &other) const {
                return !(*this == other);
            }

            operator T&() {
                return *m_Node->getPtr();
            }

            operator const T&() const {
                return *m_Node->getPtr();
            }

            operator std::shared_ptr<T>&() {
                return m_Node->getPtr();
            }

            operator const std::shared_ptr<T>&() const {
                return m_Node->getPtr();
            }

            Iterator &operator++() {
                m_Node = m_Node->getNext();
                return *this;
            }

            Iterator operator++(int) {
                auto copy = *this;
                ++(*this);
                return copy;
            }

            Iterator &operator--() {
                m_Node = m_Node->getPrev();
                return *this;
            }

            Iterator operator--(int) {
                auto copy = *this;
                --(*this);
                return copy;
            }
        };

        Iterator begin() const {
            return Iterator{this->getHead()};
        }

        Iterator end() const {
            return Iterator{nullptr};
        }

        Iterator rbegin() const {
            return Iterator{this->getTail()};
        }

        Iterator rend() const {
            return Iterator{nullptr};
        }

        static void erase(const Iterator &it) {
            it.getNode()->remove();
        }

        void clear() {
            m_Head = nullptr;
            m_Tail = nullptr;
            m_Size = 0;
        }

        DLList() = default;

        DLList(std::initializer_list<std::shared_ptr<T> > list) {
            for (const auto &value: list) {
                insertBack(value);
            }
        }

        DLList(std::initializer_list<T> list) {
            for (const auto &value: list) {
                insertBack(std::make_shared<T>(value));
            }
        }
    };
}
