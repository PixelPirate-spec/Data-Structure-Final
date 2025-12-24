#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include "Student.h"

using namespace std;

// 快速排序的前置函数
int partition(vector<Student> &students, int low, int high)
{
    string pivot = students[high].id;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        // 如果当前的小于 pivot（基准）-》交换
        if (students[j].id < pivot)
        {
            i++;
            swap(students[i], students[j]);
        }
    }
    swap(students[i + 1], students[high]);
    return (i + 1);
}

void quickSort(vector<Student> &students, int low, int high)
{
    if (low < high)
    {
        int pi = partition(students, low, high);
        quickSort(students, low, pi - 1);
        quickSort(students, pi + 1, high);
    }
}

// 堆排序（小根堆）

void heapify(vector<Student> &students, int n, int i)
{
    int smallest = i;  // 初始化最小的为根
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // 如果左孩子 小于 根
    if (l < n && students[l].score < students[smallest].score)
        smallest = l;
    // 如果右孩子 小于 最小值
    if (r < n && students[r].score < students[smallest].score)
        smallest = r;

    // 如果最小的不是根
    if (smallest != i)
    {
        swap(students[i], students[smallest]);
        // 递归子树
        heapify(students, n, smallest);
    }
}

void heapSort(vector<Student> &students)
{
    int n = students.size();

    // 初始化小根堆
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(students, n, i);

    // 一个一个提取出来排序
    for (int i = n - 1; i > 0; i--)
    {
        // 将堆顶元素移动至末尾
        swap(students[0], students[i]);
        heapify(students, i, 0);
    }
}

bool isStudentListEmpty(const vector<Student> &students)
{
    return students.empty();
}

void printStudents(const vector<Student> &students)
{
    printStudentHeader();
    for (const auto &s : students)
    {
        printStudent(s);
    }
    cout << endl;
}

void printStudentsCSV(const vector<Student> &students)
{
    cout << "学号,姓名,成绩" << endl;
    for (const auto &s : students)
    {
        cout << s.id << "," << s.name << "," << s.score << endl;
    }
}

void inputStudents(vector<Student> &students)
{
    int n;
    cout << "请输入要添加的学生人数：";
    if (!(cin >> n))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    for (int i = 0; i < n; ++i)
    {
        Student s;
        cout << "学生 " << (i + 1) << ":" << endl;
        cout << "  学号：";
        cin >> s.id;
        cout << "  姓名：";
        cin >> s.name;
        cout << "  成绩：";
        cin >> s.score;
        students.push_back(s);
    }
    cout << n << " 名学生已添加。" << endl;
}

void initStudents(vector<Student> &students)
{
    students.push_back({"1003", "爱丽丝", 85.5});
    students.push_back({"1001", "鲍勃", 92.0});
    students.push_back({"1005", "查理", 78.5});
    students.push_back({"1002", "大卫", 88.0});
    students.push_back({"1004", "伊芙", 95.5});
}

void loadStudentsFromFile(vector<Student> &students, const string &filename)
{
    students.clear();
    ifstream infile(filename);
    if (!infile.is_open())
    {
        cerr << "无法打开文件：" << filename << endl;
        return;
    }

    string line;
    while (getline(infile, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        Student s;
        if (ss >> s.id >> s.name >> s.score)
        {
            students.push_back(s);
        }
    }
    infile.close();
}

int main(int argc, char *argv[])
{
    vector<Student> students;

    if (argc > 1)
    {
        string command = argv[1];
        string filename;
        if (argc > 2)
        {
            filename = argv[2];
            loadStudentsFromFile(students, filename);
        }
        else
        {
            initStudents(students);
        }

        if (command == "sort_id")
        {
            if (!students.empty())
            {
                quickSort(students, 0, students.size() - 1);
            }
            printStudentsCSV(students);
        }
        else if (command == "sort_score")
        {
            if (!students.empty())
            {
                heapSort(students);
            }
            printStudentsCSV(students);
        }
        else
        {
            cerr << "未知命令：" << command << endl;
            return 1;
        }
        return 0; 
    }

    initStudents(students);
    int choice;
    do
    {
        cout << "========================================" << endl;
        cout << "主题 1：学生成绩系统" << endl;
        cout << "========================================" << endl;
        cout << "1. 添加学生" << endl;
        cout << "2. 查看学生（当前顺序）" << endl;
        cout << "3. 按学号快速排序（升序）" << endl;
        cout << "4. 按成绩堆排序（降序）" << endl;
        cout << "0. 退出" << endl;
        cout << "请输入选项：";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            choice = -1;
        }

        switch (choice)
        {
        case 1:
            inputStudents(students);
            break;
        case 2:
            if (isStudentListEmpty(students))
            {
                cout << "没有学生可显示。" << endl;
            }
            else
            {
                printStudents(students);
            }
            break;
        case 3:
            if (!isStudentListEmpty(students))
            {
                quickSort(students, 0, students.size() - 1);
                cout << "按学号排序（升序）：" << endl;
                printStudents(students);
            }
            else
            {
                cout << "没有学生可显示。" << endl;
            }
            break;
        case 4:
            if (!isStudentListEmpty(students))
            {
                heapSort(students);
                cout << "按成绩排序（降序）：" << endl;
                printStudents(students);
            }
            else
            {
                cout << "没有学生可显示。" << endl;
            }
            break;
        case 0:
            cout << "正在退出..." << endl;
            break;
        default:
            cout << "无效选项，请重试。" << endl;
        }
        cout << endl;

    } while (choice != 0);

    return 0;
}
