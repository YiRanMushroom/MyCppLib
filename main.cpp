import std.compat;

import ResourceManager;

struct Int {
    int value;

    Int() : value{0} {
        std::cout << "Creating Int with default value" << std::endl;
    }

    Int(int value) : value{value} {
        std::cout << "Creating Int with value " << value << std::endl;
    }

    virtual void destroy() {
        std::cout << "Destroying Int with value " << value << std::endl;
    }

    virtual bool isValid() const {
        return true;
    }

};

struct IntDerived : Int {
public:
    IntDerived() : Int{0} {
        std::cout << "Creating IntDerived with default value" << std::endl;
    }

    IntDerived(int value) : Int{value} {
        std::cout << "Creating IntDerived with value " << value << std::endl;
    }

    void destroy() override {
        std::cout << "Destroying IntDerived with value " << value << std::endl;
    }

    bool isValid() const override {
        return true;
    }
};

int main() {
    std::cout << "Hello, World!" << std::endl;

    Std::VirtualUniqueHolder<Int> holder = Std::makeVirtualUniqueHolder<IntDerived>(0);

    auto holder2 = std::move(holder);

    return 0;
}
