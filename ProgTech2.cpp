#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

string inputFile = "data.txt";
string outputFile = "output.txt";
string timeStampsFile = "timestamps.txt";

ofstream fout(outputFile);

int batchNum = 7;

int dims[7] = { 100, 500, 1000, 2000, 5000, 10000, 100000 };

//key: name

struct Student {
    string name;
    string faculty;
    int group;
    double average;

    Student()
    {
        this->name = "name";
        this->faculty = "faculty";
        this->group = 0;
        this->average = 0;
    }

    Student(string name, string faculty, int group, double average)
    {
        this->name = name;
        this->faculty = faculty;
        this->group = group;
        this->average = average;
    }

    friend bool operator== (const Student& a, const Student& b) {
        if (a.average == b.average && a.name == b.name && a.faculty == b.faculty)
            return true;
        return false;
    }

    friend bool operator< (const Student& a, const Student& b) {
        if (a.average < b.average || ((a.average == b.average) && (a.name < b.name)) ||
            ((a.average == b.average) && (a.name == b.name) && (a.faculty < b.faculty)))
            return true;
        return false;
    }

    friend bool operator<= (const Student& a, const Student& b) {
        if (a < b || a == b)
            return true;
        return false;
    }

    friend bool operator> (const Student& a, const Student& b) {
        if (!(a < b) && !(a == b))
            return true;
        return false;
    }

    friend bool operator>= (const Student& a, const Student& b) {
        if (!(a < b))
            return true;
        return false;
    }


    friend ostream& operator<<(ostream& os, const Student& a) {
        os << a.average << ' ' << a.name << ' ' << a.faculty << '\n';
        return os;
    }
};

vector<vector<Student>> readTxtFile() {
    ifstream fin(inputFile);

    vector<vector<Student>> result;

    int dim;
    string name, faculty;
    int group;
    double average;

    for (int i = 0; i < batchNum; ++i) {
        //Ввод числа записей
        fin >> dim;
        vector<Student> v;
        for (int j = 0; j < dim; ++j) {
            //Ввод полей по порядку
            fin >> name >> faculty >> group >> average;
            Student temp(name, faculty, group, average);
            v.push_back(temp);
        }
        result.push_back(v);
    }

    return result;
}


void writeTime(string title, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    fout << title;

    fout << chrono::duration_cast<chrono::microseconds>(end - start).count() << " [микросекунд]\n\n";
}

Student straightSearch(vector<Student>& v, string key) {
    for (int i = 0; i < v.size(); ++i)
        if (v[i].name == key)
            return v[i];

    return Student();
}

Student binarySearch(vector<Student>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (key < v[mid].name)        // если искомое меньше значения в ячейке
            right = mid - 1;          // смещаем правую границу поиска
        else if (key > v[mid].name)   // если искомое больше значения в ячейке
            left = mid + 1;           // смещаем левую границу поиска
        else                          // иначе (значения равны)
            return v[mid];            // функция возвращает индекс ячейки

        if (left > right)             // если границы сомкнулись 
            return Student();
    }
}

bool comp(Student a, Student b) { return (a.name < b.name); }

int main()
{
    setlocale(LC_ALL, "Russian");

    vector<vector<Student>> initialArray = readTxtFile();

    vector<string> selectedKeys;

    for (int i = 0; i < batchNum; ++i) {
        selectedKeys.push_back(initialArray[i][rand() % initialArray[i].size()].name);
    }

    std::chrono::steady_clock::time_point start, end;
    Student foundValue;

    fout << "Прямой поиск:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        Student temp = straightSearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался элемент: " << selectedKeys[i] << "\n";
        fout << "Найден элемент: " << temp;
        writeTime("На прямой поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    fout << "Сортировка:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        std::sort(initialArray[i].begin(), initialArray[i].end(), comp);
        end = std::chrono::steady_clock::now();
        writeTime("На сортировку " + to_string(initialArray[i].size()) + " значениий ушло: ", start, end);
        start = end;
    }

    fout << "Бинарный поиск в отсортированном массиве:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        Student temp = binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался элемент: " << selectedKeys[i] << "\n";
        fout << "Найден элемент: " << temp;
        writeTime("На бинарный поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    vector<multimap<string, Student>> m;

    for (int i = 0; i < batchNum; ++i) {
        multimap<string, Student> temp;
        for (int j = 0; j < initialArray[i].size(); ++j)
            temp.insert({ initialArray[i][j].name, initialArray[i][j] });
        m.push_back(temp);
    }

    initialArray.clear();

    fout << "Поиск в ассоциативном массиве:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        Student temp = m[i].find(selectedKeys[i])->second;
        end = std::chrono::steady_clock::now();
        fout << "Искался элемент: " << selectedKeys[i] << "\n";
        fout << "Найден элемент: " << temp;
        writeTime("На поиск в ассоциативном массиве с " + to_string(m[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    return 0;
}
