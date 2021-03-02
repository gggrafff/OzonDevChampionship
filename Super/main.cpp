/**
 * УСЛОВИЕ
 * https://ozondev.ru/task/524/
 * Олимпиадная задача
 * Ozon и новая база данных
 * Ozon решил придумать новую систему хранения информации пользователей,
 * в данной базе каждому пользователю будет сопоставлен его номер - id (целое неотрицательное число).
 * Так как Ozon — крупная компания, то постоянно регистрируются новые пользователи,
 * для выделения id нового пользователя берется минимальное неотрицательное число,
 * которого еще нет в множестве (боле формально берется Mex набора чисел), например,
 * mex([4, 33, 0, 1, 1, 5]) = 2, а mex([1, 2, 3]) = 0.
 *
 * К сожалению, любая база должна быть защищена от злоумышленников,
 * для этого иногда базу перестраивают — выбирается какое-то число x и
 * для каждого пользователя его id заменяется на idLx (операци побитового сложения по модулю 2),
 * надо реализовать такую структуру, возвращающую Mex множества и выполняющую операцию шифрования для заданого x.
 *
 * Формат входных данных
 * Считайте, что данные подаются в любом удобном вам виде, вам требуется просто реализовать
 * структуру, выполняющая два вида запросов:
 * 1) Зарегистрировать нового пользователя.
 * 2) Зашифровать все текущие данные с помощью нового ключа x.
 *
 * Для примера разберем следующий случай:
 * Пусть изначально у нас есть 2 пользователя с id = 1 и 3,
 * пусть сначала мы решили зашифровать нашу базу с помощью числа 1,
 * затем создать нового пользователя, тогда его номер будет 1, так как база будет состоять из номеров 0 и 2,
 * допустим теперь текущую базу требуется зашифровать еще раз с помощью снова ключа 1,
 * тогда текущая база состоит из чисел 0, 1, 2 и она станет 0, 1, 3,
 * теперь при регистрации нового пользователя нам нужно будет выдать ему номер 2.
 *
 * Формат выходных данных
 * Для каждого нового пользователя вывести его id.
 * Замечание
 * От вас требуется придумать наиболее оптимальное решение для задачи с точки зрения асимптотики.
 */





/**
 * РЕШЕНИЕ
 * Давайте для хранения чисел реализуем структуру битовый бор.
 * Эта структура была предложена к реализации на открытой олимпиаде по программированию (https://olympiads.ru/zaoch/2013-14/final/analysis1.pdf).
 *
 * Рассмотрим бор, построенный на двоичной записи содержащихся в множестве чисел.
 * Глубина бора не превосходит logN, где N - максимальное хранимое число (и максимальное количество хранимых чисел, соответственно).
 *
 * Как быстро посчитать MEX на битовом боре?
 * Поставим флаг во все вершины бора, поддерево которых полное.
 * Тогда с помощью этих пометок легко найти MEX за один спуск — идём налево, если левое поддерево не полное, иначе направо.
 *
 * Что значит проксорить все числа в боре с какой-то константой c?
 * Это означает, что надо в некоторых слоях (соответствующих 1 в двоичной записи c) поменять
 * у всех вершин левого и правого сына местами.
 * Всего таких вершин может быть много (например, в последнем слое их линейное количество).
 * Нам на помощь приходят... отложенные операции!
 * В каждой вершине храним число-маску, в соответствии с которой надо изменить поддерево.
 * Приходя в вершину меняем местами, если надо, сыновей, раздаём маску детям и двигаемся дальше.
 *
 * Таким образом, операция вставки в бор работает за время O(log N).
 * Не забываем при добавлении числа в бор пересчитывать флаги.
 * Операция взятия MEX работает за O(log N) для каждой вершины.
 * Xor для бора сводится к изменению маски в корне, а значит выполняется за O(1).
 * Реальная перестановка узлов для операции xor происходит отложенно и не ухудшает асимптотику.
 */


#include <cstdint>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <random>
#include <algorithm>

/**
 * @brief Битовый бор для чисел.
 * @tparam NumberType Тип данных хранимых чисел. Должен быть беззнаковым.
 */
template<typename NumberType = uint8_t>
class BitTrie {
public:
    /**
     * @brief Удаление бора.
     * @details Сводится к удалению корня дерева. Потомки рекурсивно удалятся в деструкторах узла.
     */
    ~BitTrie() {
        delete root_;
    }

    /**
     * @brief Добавить в бор минимально возможное число (минимальное положительное целое, не хранящееся в боре).
     * @return Число, которое было добавлено в бор.
     */
    NumberType add_number() {
        auto id = mex();  // Ищем минимальное положительное целое, не хранящееся в боре.
        add_number(id);  // Добавляем его в бор.
        return id;
    }

    /**
     * @brief Добавить в бор число.
     * @param value Число, которое нужно добавить в бор.
     */
    void add_number(NumberType value) {
        /**
         * Нужно спуститься по дереву от корня к листьям, при необходимости создавая узлы.
         * Спуск производится следующим образом:
         *  * Начинаем со старшего бита добавляемого числа (старший бит - корень бора).
         *  * Если очередной бит равен 0, идём в левого потомка, иначе - в правого.
         */
        Node* current = root_;
        for(int bit_no = max_depth_trie_ - 1; bit_no >= 0; --bit_no) {  // Перебираем биты числа.
            auto bit_value = (value >> bit_no) & 1;  // Получаем значение бита.
            current = current->get_or_create_child(bit_value);  // Спускаемся в нужного потомка.
        }
        if (current->get_fullness()) {  // Если лист уже соответствовал какому-то числу, то возвращаем ошибку.
            throw std::range_error("The ID is already taken.");
        }
        current->set_fullness(true);  // Ставим отметку о полноте поддерева. Она распространится вверх по дереву.
    }

    /**
     * @brief Все хранимые числа сложить по модулю 2 с заданным числом.
     * @param value Второе слагаемое для операции xor.
     */
    void xor_all_values(NumberType value) {
        root_->xor_values(value);  // Ставим отметку для отложенной операции в корне дерева.
    }
private:
    /**
     * @brief Поиск минимального положительного целого числа, не хранящегося в боре.
     * @return Значение минимального положительного целого числа, не хранящегося в боре.
     */
    NumberType mex() {
        /**
         * Спускаемся по дереву, заполняя биты результирующего числа:
         *  * Корень соответствует старшему биту результата.
         *  * Если левое поддерево неполное, то идём в него.
         *  * Если идём в левого потомка, то соответствующий бит равен 0, иначе - 1.
         *  * Если поддерево пустое (потомок отсутствует), то оставшиеся биты числа равны 0.
         */
        if (root_->get_fullness()) {  // Если бор полный, то сообщаем об ошибке.
            throw std::out_of_range("Trie is full.");
        }
        NumberType result = 0;  // Установим все биты в 0.
        Node* current = root_;  // Начнём со старшего бита.
        for(int bit_no = max_depth_trie_ - 1; bit_no >= 0; --bit_no) {  // Перебираем все биты числа.
                auto bit_value = 0;
                if (current->get_child(0) == nullptr) {  // Если левое поддерево пустое, то можно выходить.
                    return result;
                } else if (current->get_child(0)->get_fullness()){
                    // Если левое поддерево полное, то идём в правое, а бит устанавливаем в 1.
                    bit_value = 1;
                }
                result |= bit_value << bit_no;  // Устанавливаем значение текущего бита.
                if (current->get_child(bit_value) == nullptr) {  // Если правое поддерево пустое, то можно выходить.
                    return result;
                }
                current = current->get_child(bit_value);  // Перемещаемся в левого или правого потомка.
            }
        return result;
    }

    /**
     * @brief Класс, описывающий узел битового бора.
     */
    class Node {
    public:
        /**
         * @brief Создание узла.
         * @param parent Указываем родителя узла. Если это корень, то родитель = nullptr.
         */
        explicit Node(Node* parent){
            parent_ = parent;
        }

        /**
         * @brief Уничтожаем узел.
         * @details Уничтожаем потомков, у родителя снимаем отметку о полноте.
         */
        ~Node() {
            if (parent_ != nullptr) {
                parent_->is_full_ = false;
            }
            delete children_[0];
            delete children_[1];
        }

        /**
         * @brief Получить указатель на потомка.
         * @param index 0, если нужен левый потомок, 1 - правый.
         * @return Указатель на потомка (может быть равен nullptr).
         */
        Node* get_child(uint8_t index) const {
            if (index > 1) {
                throw std::range_error("Invalid index");
            }
            if (children_[index] != nullptr) {
                children_[index]->push_inconsistency();  // При посещении узла нужно произвести отложенный xor.
            }
            return children_[index];
        };

        /**
         * @brief Получить указатель на потомка. Если потомок отсутствует, то создать его.
         * @param index 0, если нужен левый потомок, 1 - правый.
         * @return Указатель на потомка (не может быть равен nullptr).
         */
        Node* get_or_create_child(uint8_t index) {
            if (index > 1) {
                throw std::range_error("Invalid index");
            }
            if (children_[index] == nullptr) {
                children_[index] = new Node{this};
            } else {
                children_[index]->push_inconsistency();  // При посещении узла нужно произвести отложенный xor.
            }
            return children_[index];
        }

        /**
         * @brief Установить отметку о полноте поддерева.
         * @param value Значение полноты.
         */
        void set_fullness(bool value) {
            is_full_ = value;
            if (parent_ != nullptr) {
                bool new_parent_fullness = parent_->children_[0] != nullptr &&
                                           parent_->children_[1] != nullptr &&
                                           parent_->children_[0]->is_full_ &&
                                           parent_->children_[1]->is_full_;
                if (parent_->is_full_ != new_parent_fullness) {
                    parent_->set_fullness(new_parent_fullness);  // Распространить полноту вверх по дереву к корню.
                }
            }
        }

        /**
         * @brief Получить значение полноты поддерева.
         * @return True, если поддерево полное, иначе - false.
         */
        bool get_fullness() const {
            return is_full_;
        }

        /**
         * @brief Произвести операцию xor для всех чисел дереве.
         * @param value Второе слагаемое для операции xor.
         */
        void xor_values(NumberType value){
            xor_mask_ ^= value;
            push_inconsistency();  // Не оставляем в корне несогласованность, чтобы облегчить обращения к корню.
        }
    private:
        /**
         * @brief Произвести отложенный xor для текущего узла и протолкнуть отложенную операцию потомкам.
         */
        void push_inconsistency() {
            if (xor_mask_ == 0) {
                return;
            }
            if (((xor_mask_ >> (max_depth_trie_ - 1)) & 1) == 1) {  // Если соответствующий бит равен 1.
                swap_children();  // Меняем детей местами
            }
            for (auto & node : children_) {
                if (node != nullptr) {
                    node->xor_mask_ ^= xor_mask_ << 1;  // Проталкиваем несогласованность потомкам.
                }
            }
            xor_mask_ = 0;
        }

        /**
         * @brief Поменять потомков местами.
         */
        void swap_children() {
            std::swap(children_[0], children_[1]);
        }

        Node* children_[2]{nullptr, nullptr};  // Потомки.
        Node* parent_{nullptr}; // Родитель.
        bool is_full_{false};  // Полное ли поддерево?
        NumberType xor_mask_{0};  // Пометки о необходимости отложенной операции xor.
    };

    Node* root_{new Node{nullptr}};  // Корень дерева.
    static const uint8_t max_depth_trie_{sizeof(NumberType) * 8};  // Максимальная глубина дерева.
};

/**
 * @brief Обёртка для работы с битовым бором в терминах задачи.
 */
class DataBase {
public:
    /**
     * @brief Зарегистрировать пользователя.
     * @return Id нового пользователя.
     */
    uint64_t register_new_user() {
        return trie.add_number();
    }
    /**
     * @brief Зашифровать данные.
     * @param key Ключ.
     */
    void encrypt(uint64_t key) {
        trie.xor_all_values(key);
    }
private:
    BitTrie<uint64_t> trie;  // Битовый бор для хранения id-шников.
};


// Тесты и примеры использования.

void test_8bit_from_task() {
    BitTrie<uint8_t> trie;
    trie.add_number(1);
    trie.add_number(3);
    trie.xor_all_values(1);
    auto id = trie.add_number();
    assert(id == 1);
    trie.xor_all_values(1);
    id = trie.add_number();
    assert(id == 2);
}

void test_8bit_sequential_registrations() {
    BitTrie<uint8_t> trie;
    auto id = trie.add_number();
    assert(id == 0);
    id = trie.add_number();
    assert(id == 1);
    id = trie.add_number();
    assert(id == 2);
    id = trie.add_number();
    assert(id == 3);
    id = trie.add_number();
    assert(id == 4);
}

void test_8bit_full_trie() {
    BitTrie<uint8_t> trie;
    for (auto i = 0; i < 256; ++i) {
        auto id = trie.add_number();
        assert(id == i);
    }
    try {
        auto id = trie.add_number();
        assert(false);
    } catch(std::out_of_range& e) {
        assert(true);
    }
}

void test_8bit_mex_all_values() {
    std::random_device rd;

    for (auto mex = 0; mex < 256; ++mex) {
        BitTrie<uint8_t> trie;
        for (auto i = 0; i < 256; ++i) {
            if ((i < mex) || ((i > mex) && (rd() % 2 == 0))) {
                trie.add_number(i);
            }
        }
        auto id = trie.add_number();
        assert(id == mex);
    }
}

void test_8bit_xor_all_values() {
    std::random_device rd;

    for (auto mex = 0; mex < 256; ++mex) {
        BitTrie<uint8_t> trie;
        std::vector<uint8_t> ids;
        for (auto i = 0; i < 256; ++i) {
            if ((i < mex) || ((i > mex) && (rd() % 2 == 0))) {
                trie.add_number(i);
                ids.push_back(i);
            }
        }

        uint8_t key = rd();
        for(auto& id: ids) {
            id ^= key;
        }
        std::sort(ids.begin(), ids.end());
        uint8_t new_mex = 0;
        while(new_mex == ids[new_mex]) {
            new_mex += 1;
        }
        trie.xor_all_values(key);

        auto id = trie.add_number();
        assert(id == new_mex);
    }
}


void test_16bit_from_task() {
    BitTrie<uint16_t> trie;
    trie.add_number(1);
    trie.add_number(3);
    trie.xor_all_values(1);
    auto id = trie.add_number();
    assert(id == 1);
    trie.xor_all_values(1);
    id = trie.add_number();
    assert(id == 2);
}

void test_16bit_sequential_registrations() {
    BitTrie<uint16_t> trie;
    for (auto i = 0; i < 10000; ++i) {
        auto id = trie.add_number();
        assert(id == i);
    }
    auto id = trie.add_number();
    assert(id == 10000);
}

void test_16bit_full_trie() {
    BitTrie<uint16_t> trie;
    for (auto i = 0; i < 65536; ++i) {
        auto id = trie.add_number();
        assert(id == i);
    }
    try {
        auto id = trie.add_number();
        assert(false);
    } catch(std::out_of_range& e) {
        assert(true);
    }
}

void test_16bit_mex_all_values() {
    std::random_device rd;

    for (auto mex = 0; mex < 65536; mex += rd() % 1000) {
        BitTrie<uint16_t> trie;
        for (auto i = 0; i < 65536; ++i) {
            if ((i < mex) || ((i > mex) && (rd() % 10 == 0))) {
                trie.add_number(i);
            }
        }
        auto id = trie.add_number();
        assert(id == mex);
    }
}

void test_16bit_xor_all_values() {
    std::random_device rd;

    for (auto mex = 0; mex < 65536; mex += rd() % 1000) {
        BitTrie<uint16_t> trie;
        std::vector<uint16_t> ids;
        for (auto i = 0; i < 65536; ++i) {
            if ((i < mex) || ((i > mex) && (rd() % 10 == 0))) {
                trie.add_number(i);
                ids.push_back(i);
            }
        }

        uint16_t key = rd();
        for(auto& id: ids) {
            id ^= key;
        }
        std::sort(ids.begin(), ids.end());
        uint16_t new_mex = 0;
        while(new_mex == ids[new_mex]) {
            new_mex += 1;
        }
        trie.xor_all_values(key);

        auto id = trie.add_number();
        assert(id == new_mex);
    }
}

void test_64bit_from_task() {
    BitTrie<uint64_t> trie;
    trie.add_number(1);
    trie.add_number(3);
    trie.xor_all_values(1);
    auto id = trie.add_number();
    assert(id == 1);
    trie.xor_all_values(1);
    id = trie.add_number();
    assert(id == 2);
}


void run_all_tests() {
    test_8bit_from_task();
    test_8bit_sequential_registrations();
    test_8bit_full_trie();
    test_8bit_mex_all_values();
    test_8bit_xor_all_values();

    test_16bit_from_task();
    test_16bit_sequential_registrations();
    test_16bit_full_trie();
    test_16bit_mex_all_values();
    test_16bit_xor_all_values();

    test_64bit_from_task();
}


int main(int, char *[]) {
    run_all_tests();
    return 0;
}

