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

    void destroy() {
        std::cout << "Destroying Int with value " << value << std::endl;
    }

    bool isValid() const {
        return true;
    }

};

int main() {
    std::cout << "Hello, World!" << std::endl;

    Std::UniqueHolder<Int> holder = Std::makeUniqueHolder<Int>(0);

    auto holder2 = std::move(holder);

    return 0;
}
