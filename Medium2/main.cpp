/*
 * https://ozondev.ru/task/487/
 * Задача о цепочке знакомых
 * Дано отображение Ф.И.О. человека на список знакомых (также Ф.И.О.) map<string, [ ]string>.
 * Нужно написать алгоритм поиска цепочки знакомых от одного человека до другого.
 * func search Acquaintances Сhain(map[string][]string, first, second string) []string
 */

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iostream>
#include <cassert>
#include <algorithm>

/**
 * Поиск кратчайшей цепочки знакомых, связывающей двух людей.
 * @param peoples Граф знакомств.
 * @param first Начало цепочки.
 * @param second Конец цепочки.
 * @return Цепочка знакомых.
 */
std::vector<std::string> searchAcquaintancesChain(const std::map<std::string, std::vector<std::string>>& peoples,
                                                  const std::string& first,
                                                  const std::string& second) {
    /*
     * Производим поиск в ширину.
     * Возможная оптимизация:
     * Конечно, копировать строки - это неэффективно. Лучше назначить каждому человеку номер и работать с числами.
     * Логика решения от этого не изменится, но появится дополнительный шаг - назначение числовых идентификаторов.
     * В конце нужно будет цепочку идентификаторов преобразовать обратно в цепочку строк.
     */
    std::queue<std::string> q;  // Очередь посещения вершин.
    q.push(first);  // Стартовая вершина.
    std::map<std::string, bool> used;  // Пометки о посещении вершин графа.
    for(auto& people: peoples) {
        used[people.first] = false;  // Ни одна вершина пока не посещена.
    }
    std::map<std::string, std::string> parents;  // Запоминаем родительские вершины.
    used[first] = true;  // Посещаем стартовую вершину.
    parents[first] = first;  // Назначаем для стартовой вершины родителем саму себя (можно другое значение, которое будет легко обнаружить, например, пустую строку).
    bool success = false;  // Отметка о том, что путь найден.
    while (!q.empty()) {
        auto v = q.front();  // Берём очередную вершину из очереди.
        q.pop();
        for (size_t i = 0; i < peoples.at(v).size(); ++i) {  // Рассматриваем всех знакомых.
            auto to = peoples.at(v)[i];
            if (!used[to]) {  // Если человек ещё не был рассмотрен.
                used[to] = true;
                q.push(to);  // Добавляем человека к рассмотрению.
                parents[to] = v;  // Фиксируем родительскую вершину.
                if (to == second) {  // Когда дошли до нужного человека.
                    q = std::queue<std::string>();  // Очищаем очередь, чтобы закончить работу.
                    success = true;  // Путь найден.
                    break;
                }
            }
        }
    }
    if (!success) {
        return {};  // Если путь не найден.
    }
    std::vector<std::string> result;  // Если путь найден, то восстанавливаем его.
    result.push_back(second);
    while (parents[result.back()] != result.back()) {
        result.push_back(parents[result.back()]);
    }
    std::reverse(result.begin(), result.end());  // Разворачиваем цепочку.
    return result;
}

void test1() {
    std::map<std::string, std::vector<std::string>> peoples {
            {"a", {"b"}},
            {"b", {"c"}},
            {"c", {"d"}},
            {"d", {"e"}},
    };
    auto chain = searchAcquaintancesChain(peoples, "a", "e");
    std::vector<std::string> true_chain{"a", "b", "c", "d", "e"};
    assert(chain == true_chain);
}

void test2() {
    std::map<std::string, std::vector<std::string>> peoples {
            {"a", {"b", "c"}},
            {"b", {"c", "d"}},
            {"c", {"d", "a"}},
            {"d", {"e", "b"}},
    };
    auto chain = searchAcquaintancesChain(peoples, "a", "e");
    std::vector<std::string> true_chain{"a", "b", "d", "e"};
    assert(chain == true_chain);
}

void test3() {
    std::map<std::string, std::vector<std::string>> peoples {
            {"a", {"b"}},
            {"b", {"c"}},
            {"c", {}},
            {"d", {"e"}},
    };
    auto chain = searchAcquaintancesChain(peoples, "a", "e");
    std::vector<std::string> true_chain;
    assert(chain == true_chain);
}

int main(int, char *[]) {
    test1();
    test2();
    test3();
    return 0;
}