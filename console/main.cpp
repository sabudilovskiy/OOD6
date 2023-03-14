#include "Handler.hpp"
#include <iostream>

int main() {
    auto handler = CreateAll();
    std::string path;
    std::cout << "Введите путь к файлу: ";
    std::cin >> path;
    if (!handler(path)){
        std::cout << "Не найден обработчик";
    }
}
