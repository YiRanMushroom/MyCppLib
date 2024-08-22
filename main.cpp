import com.yrm.libcpp.std.compat;
import com.yrm.container.DLList;

int main() {

    {
        auto functions = Std::DLList<std::function<bool(int)>>{};

        functions.emplaceBack([](int a)-> bool { return a % 2 == 0; });
        functions.emplaceBack([](int a)-> bool { return a % 3 == 0; });
        functions.emplaceBack([](int a)-> bool { return a % 5 == 0; });

        std::cout << std::endl;

        for (int i = 0; i < 32; i++) {
            bool result = false;
            for (auto it = functions.begin(); it != functions.end(); ++it) {
                result = (*it)(i);
                if (result) {
                    break;
                }
            }
            std::cout << i << ": " << result << std::endl;
        }

        functions.popBack();
        functions.popFront();

        auto fun = std::move(functions.popBack());

        std::cout << (*fun)(3) << std::endl;
    }

    return 0;
}
