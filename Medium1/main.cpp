/*
 * https://ozondev.ru/task/473/
 * Задача об удалении лишних пробелов
 * Дана строка с избыточным количеством пробелов. Нужно удалить лишние:
 * before: _On__my___home_world
 * after: On_my_home_world
 * Что хочется увидеть: inplace-алгоритм со сложностью O(N) по времени и O(1) по памяти.
 */

#include <string>
#include <iostream>
#include <cassert>

bool is_space(char c){
    return c == ' ' || c == '_';  // Из условия задачи не понятно, какой символ называется пробелом.
}

void clean_spaces(std::string& str_with_spaces) {
    /*
     * Делаем один проход по строке.
     * Храним два указателя.
     * Первый: позиция текущего символа после обработки.
     * Второй: текущий обрабатываемый символ в исходной строке.
     * Из второй позиции в первую копируем все непробелы и ровно по одному пробелу.
     */
    size_t insert_position = 0;  // Позиция в которую вставляем символы после очистки.
    size_t check_position = 0;  // Позиция просматриваемого символа в исходной строке.
    while(check_position < str_with_spaces.size()) {  // Пока строка не закончилась
        while (check_position < str_with_spaces.size() &&
                is_space(str_with_spaces[check_position])) {
            check_position += 1;  // Пропускаем все пробелы.
        }
        while (check_position < str_with_spaces.size() &&
               !is_space(str_with_spaces[check_position])) {
            str_with_spaces[insert_position] = str_with_spaces[check_position];  // Копируем все непробелы.
            insert_position += 1;
            check_position += 1;
        }
        if (check_position < str_with_spaces.size()) {
            str_with_spaces[insert_position] = str_with_spaces[check_position];  // Оставляем ровно один пробел.
            insert_position += 1;
        }
    }
    if (is_space(str_with_spaces.back())) {  // Если строка закончилась пробелом.
        insert_position -= 1;  // Убираем его.
    }
    str_with_spaces.resize(insert_position);  // Обрезаем строку.
}

void test1() {
    std::string str = " On  my   home world  ";
    clean_spaces(str);
    assert(str == "On my home world");
}

void test2() {
    std::string str = "_On__my___home_world__";
    clean_spaces(str);
    assert(str == "On_my_home_world");
}

int main(int, char *[]) {
    test1();
    test2();

    std::string str;
    std::getline(std::cin, str);
    clean_spaces(str);
    std::cout << str;
    return 0;
}