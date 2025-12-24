#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <iostream>
#include <iomanip>

struct Student
{
    std::string id;
    std::string name;
    float score;
};


inline void printStudentHeader()
{
    std::cout << std::left << std::setw(15) << "学号"
              << std::setw(20) << "姓名"
              << std::setw(10) << "成绩" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
}

inline void printStudent(const Student &s)
{
    std::cout << std::left << std::setw(15) << s.id
              << std::setw(20) << s.name
              << std::setw(10) << s.score << std::endl;
}

#endif 
