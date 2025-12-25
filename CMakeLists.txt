cmake_minimum_required(VERSION 3.10)
project(DataStructureFinal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 主题1：学生成绩系统
add_executable(Topic1_Student Topic1_Student/main.cpp)

# 主题2：字典系统
add_executable(Topic2_Dictionary Topic2_Dictionary/main.cpp)

# 主题3：校园导航
add_executable(Topic3_Campus Topic3_Campus/main.cpp)
