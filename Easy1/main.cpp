/*
 * Определить, у какого числа нет пары.
 * Пример входных данных: 1, 2, 1, 3, 2
 */

#include <locale>
#include <iostream>

struct comma_is_space : std::ctype<char> {
    comma_is_space() : std::ctype<char>(get_table()) {}
    static mask const* get_table()
    {
        static mask rc[table_size];
        rc[','] = std::ctype_base::space;
        rc[' '] = std::ctype_base::space;
        // rc['\n'] = std::ctype_base::space;
        return &rc[0];
    }
};

int main(int, char *[]) {
    std::cin.imbue(std::locale(std::cin.getloc(), new comma_is_space));

    uint64_t result = 0;
    uint64_t current = 0;

    while (!std::cin.eof() && std::cin >> current) {
        result ^= current;
    }
    std::cout << result;
    return 0;
}