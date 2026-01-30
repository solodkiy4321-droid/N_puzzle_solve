#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <queue>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>
#include <cstdlib>
#include <functional>
#include <utility>
#include <algorithm>

const int MAX_DEPTH_4x4 = 50;
const int MAX_DEPTH_5x5 = 100;

struct State {
    std::vector<std::vector<int>> board;
    int g;          //пройденное расстояние
    int h;          //манхетенское расстояние
    int f;          //результирующий показатель
    std::string moves;

    bool operator>(const State& other) const {
        return f > other.f;
    }
};
std::string solve_puzzle_A(const std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& goal, const int& k);
bool isSolvable(const std::vector<std::vector<int>>& board, const int& k);
int manhattanDistance(const State& state, const int& k);
std::string board_ToString(const std::vector<std::vector<int>>& board, const int& k);
void Output(const std::string& result);
std::string solve_puzzle_ida(const std::vector<std::vector<int>>& board,
    const std::vector<std::vector<int>>& goal,
    const int k);
int search(State& state, const std::vector<std::vector<int>>& goal,
    int k, int g, int bound, std::string& solution, std::unordered_set<std::string>& visited,int depth_4);
int linearConflictDistance(const State& state, const int k);

int main() {

    //ввод размерности
    std::cout << "Input dimension: " << std::endl;
    int k;
    std::cin >> k;

    //проверка условия размерности
    if (k < 2 || k > 5) {
        std::cerr << "Invalid dimension! Must be between 2 and 5." << std::endl;
        return 1;
    }

    //ввод данных
    std::cout << "Input value's: " << std::endl;

    std::vector<std::vector<int>> values(k, std::vector<int>(k));
    for (int i{ 0 }; i < k; i++) {
        for (int j{ 0 }; j < k; j++) {
            std::cin >> values[i][j];
        }
    }

    //установка целевого значения
    std::vector<std::vector<int>> goal(k, std::vector<int>(k));
    int count = 1;
    for (int i{ 0 }; i < k; i++) {
        for (int j{ 0 }; j < k; j++) {

            if (i == k - 1 && j == k - 1) {
                goal[i][j] = 0;
            }
            else {
                goal[i][j] = count++;
            }

        }
      
    }
    
    std::string result = "";    //для хранения результата

    //ветвление для размерностей
    if (k == 3) {
        result = solve_puzzle_A(values, goal, k);
    }
    else {
        result = solve_puzzle_ida(values,goal,k);
    }
    
    Output(result);
    return 0;
}
std::string solve_puzzle_A(const std::vector<std::vector<int>>& board,const std::vector<std::vector<int>>& goal,const int& k) {

    //проверка на решаемость
    if (!isSolvable(board, k)) {
        std::cout << "Not Solvable";
        return " ";
    }

    //инициализаци начального состояния
    State start_state;
    start_state.board = board;
    start_state.g = 0;
    start_state.moves = "";
    start_state.h = manhattanDistance(start_state,k);
    start_state.f = start_state.g + start_state.h;
    

    //создание приоритетной очереди
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    //инициализация приоритетной очереди
    pq.push(start_state);

    //определение направлений
    std::vector<std::pair<int, int>> directions = {
        {0, 1},  //вправо
        {0, -1}, //влево
        {-1, 0},   //вверх
        {1, 0}  //вниз
    };
    std::vector<std::string> move_directions = { "RIGHT","LEFT","UP","DOWN" };

    std::unordered_set<std::string> visited_point;      //фиксация пройденной точки
    while (!pq.empty()) {

        //инициализация текущего состояния
        State current_state;
        current_state = pq.top();
        pq.pop();

        //проверка на зацикленность
        std::string current_state_position = board_ToString(current_state.board,k);
        if (visited_point.find(current_state_position) != visited_point.end()) {

            continue;
        }

        visited_point.insert(current_state_position);

        //проверка на равенство результату
        if (current_state.board == goal) {
            return current_state.moves;
        }

        //поиск нулевой плитки
        int Zero_position_X = -1;
        int Zero_position_Y = -1;
        bool found = false;

        for (int i = 0; i < k && !found; i++) {
            for (int j = 0; j < k && !found; j++) {
                if (current_state.board[i][j] == 0) {
                    Zero_position_X = i;
                    Zero_position_Y = j;
                    found = true;
                }
            }
        }

        //поиск новых направлений
        for (int i{ 0 }; i < 4; i++) {

            int new_x = Zero_position_X + directions[i].first;
            int new_y = Zero_position_Y + directions[i].second;
                
            //проверка границ
                if (new_x >= 0 && new_x < k && new_y >= 0 && new_y < k) {

                    //создание нового состояния
                    std::vector<std::vector<int>> New_board = current_state.board;
                    std::swap(New_board[new_x][new_y], New_board[Zero_position_X][Zero_position_Y]);

                    //проверка на зацикленность
                    std::string new_position = board_ToString(New_board,k);
                    if (visited_point.find(new_position) != visited_point.end()) {
                        continue;
                    }

                    //создание нового состояния
                    State new_state;
                    new_state.board = New_board;
                    new_state.g = current_state.g + 1; 
                    new_state.h = manhattanDistance(new_state,k);
                    new_state.f = new_state.g + new_state.h;
                    new_state.moves = current_state.moves + move_directions[i] + " ";

                    //добавление в очередь
                    pq.push(new_state);

                }
        }

    }
    return "No solution found";
}
std::string solve_puzzle_ida(const std::vector<std::vector<int>>& board,
    const std::vector<std::vector<int>>& goal,
    const int k) {

    //проверка решаемости
    if (!isSolvable(board, k)) {
        return "Not Solvable";
    }

    // Начальное состояние
    State start_state;
    start_state.board = board;
    start_state.g = 0;
    start_state.moves = "";
    start_state.h = linearConflictDistance(start_state, k);
    start_state.f = start_state.h;
    
    int bound = start_state.h;  //начальная инициализация границы
   
    std::string solution;    //переменная для результата

    while (true) {
        
        std::unordered_set<std::string> visited;    //для фиксации пройденных состояний

        int depth_4 = 0;    //счетчик глубины

        int t = search(start_state, goal, k, 0, bound, solution, visited, depth_4);
        if (t == -1) {
            return solution;  // Найдено решение
        }
        if (t == std::numeric_limits<int>::max()) {
            return "No solution found";
        }
        bound = t;  //обновление границы(увеличение)
    }
}

int search(State& state, const std::vector<std::vector<int>>& goal,
    int k, int g, int bound, std::string& solution, std::unordered_set<std::string>& visited,int depth) {

    //проверка глубины
    if (k == 4 && depth > MAX_DEPTH_4x4) {

        return std::numeric_limits<int>::max();
    }
    else if (k == 4 && depth > MAX_DEPTH_5x5) {

        return std::numeric_limits<int>::max();
    }
    depth++;

    //оценка состояния
    int f = g + state.h;

    //проверка текущего состояния
    if (f > bound) return f;
    if (state.board == goal) {
        solution = state.moves;
        return -1;  
    }

    //проверка предыдущего шага
    std::string state_str = board_ToString(state.board, k);

    if (visited.find(state_str) != visited.end()) {

        return std::numeric_limits<int>::max();
    }

    visited.insert(state_str);

    
   

    //исследование дочерних состояний
    int zero_x = -1, zero_y = -1;
    bool found_zero = false;

    for (int i = 0; i < k && !found_zero; i++) {

        for (int j = 0; j < k && !found_zero; j++) {

            if (state.board[i][j] == 0) {

                zero_x = i;
                zero_y = j;
                found_zero = true;
            }
        }
    }

    // инициализация направлений
    std::vector<std::pair<int, int>> directions = {
        {0, 1},    // вправо
        {0, -1},   // влево
        {-1, 0},   // вверх
        {1, 0}     // вниз
    };

    std::vector<std::string> move_names = { "RIGHT", "LEFT", "UP", "DOWN" };

    //инициализация min пустым значением
    int min = std::numeric_limits<int>::max();

    // 6. определение направлений
    for (int i = 0; i < 4; i++) {

        int new_x = zero_x + directions[i].first;
        int new_y = zero_y + directions[i].second;

        //проверка границ
        if (new_x >= 0 && new_x < k && new_y >= 0 && new_y < k) {

            //новая доска
            std::vector<std::vector<int>> new_board = state.board;
            std::swap(new_board[zero_x][zero_y], new_board[new_x][new_y]);

            //новое состояние
            State next_state;
            next_state.board = new_board;
            next_state.g = state.g + 1;
            next_state.moves = state.moves + move_names[i] + " ";
            next_state.h = linearConflictDistance(next_state, k);
            next_state.f = next_state.g + next_state.h;

            // Рекурсивный вызов search
            int t = search(next_state, goal, k, next_state.g, bound, solution, visited, depth);

            // Проверка результата
            if (t == -1) {
                return -1;  // Решение найдено
            }

            // минимальное значение границ
            if (t < min) {
                min = t;
            }
        }
    }

    visited.erase(state_str);

    return min;
}
void Output(const std::string& result) {
    //вывод результата

    if (result.empty() || result == " ") {
        std::cout << "Already solved!" << std::endl;
        return;
    }

    if (result == "Not Solvable" || result == "No solution found") {
        std::cout << result << std::endl;
        return;
    }

    std::istringstream iss(result);
    std::string word;

    while (iss >> word) {
        std::cout << word << std::endl;
    }
    return;
}
bool isSolvable(const std::vector<std::vector<int>>& board, const int& k) {

    // выравнивание структуры доски в один ряд
    std::vector<int> linear;
    for (const auto& row : board) {
        for (int col : row) {
            if (col != 0) linear.push_back(col);
        }
    }

    // подсчет инверсий
    int inversions = 0;
    for (size_t i = 0; i < linear.size(); i++) {
        for (size_t j = i + 1; j < linear.size(); j++) {
            if (linear[i] > linear[j]) inversions++;
        }
    }

    // номер строки с пустой клеткой СНИЗУ 
    int emptyRowFromBottom = -1;
    bool found = false;
    for (int i = 0; i < k && !found; i++) {
        for (int j = 0; j < k && !found; j++) {
            if (board[i][j] == 0) {
                emptyRowFromBottom = k - i;  
                found = true;
            }
        }
    }

    // для нечетных размеров
    if (k % 2 == 1) {
        return inversions % 2 == 0;  // инверсии должны быть чётными
    }

    // для четных размеров
    return (inversions % 2 == 0) == (emptyRowFromBottom % 2 == 1);
}
int manhattanDistance(const State& state, const int& k) {

    int sum_result = 0;

    for (int i{ 0 }; i < k; i++) {
        for (int j{ 0 }; j < k; j++) {
            int value = state.board[i][j];
            if (value != 0) {
                int X_diff = (value - 1) / k;
                int Y_diff = (value - 1) % k;
                sum_result += abs(i - X_diff) + abs(j - Y_diff);
            }
           
        }
    }
    return sum_result;
}
int linearConflictDistance(const State& state, const int k) {
    int distance = manhattanDistance(state, k);

    // Линейные конфликты по строкам
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            int val1 = state.board[i][j];
            if (val1 == 0) continue;

            // Проверка, должна ли плитка находиться в этой строке
            int target_i = (val1 - 1) / k;
            if (target_i != i) continue;

            for (int m = j + 1; m < k; m++) {
                int val2 = state.board[i][m];
                if (val2 == 0) continue;

                int target_i2 = (val2 - 1) / k;
                if (target_i2 != i) continue;

                // Если две плитки в одной строке и в неправильном порядке
                int target_j1 = (val1 - 1) % k;
                int target_j2 = (val2 - 1) % k;

                if (target_j1 > target_j2) {
                    distance += 2;  // Линейный конфликт
                }
            }
        }
    }

    // Линейные конфликты по столбцам 
    for (int j = 0; j < k; j++) {
        for (int i = 0; i < k; i++) {
            int val1 = state.board[i][j];
            if (val1 == 0) continue;

            int target_j = (val1 - 1) % k;
            if (target_j != j) continue;

            for (int m = i + 1; m < k; m++) {
                int val2 = state.board[m][j];
                if (val2 == 0) continue;

                int target_j2 = (val2 - 1) % k;
                if (target_j2 != j) continue;

                int target_i1 = (val1 - 1) / k;
                int target_i2 = (val2 - 1) / k;

                if (target_i1 > target_i2) {
                    distance += 2;
                }
            }
        }
    }

    return distance;
}
std::string board_ToString(const std::vector<std::vector<int>>& board,const int& k) {
    //перевод состояния доски в строку(расположение плиток)

    std::string result = "";
    for (int i{ 0 }; i < k; i++) {
        for (int j{ 0 }; j < k; j++) {
            result += std::to_string(board[i][j]) + ",";
        }
    }
    return result;
}



