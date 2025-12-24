import streamlit as st
import subprocess
import pandas as pd
import os
import graphviz
import re
import io  # <--- 新增：用于把字符串变成文件流给pandas读取

# ================= 配置区 =================
st.set_page_config(
    page_title="数据结构大作业集成系统",
    page_icon="🎓",
    layout="wide",
    initial_sidebar_state="expanded",
)

# 定义路径
BUILD_DIR = "build"
DICT_FILE = os.path.join(BUILD_DIR, "dictionary.txt")
MAP_FILE = os.path.join(BUILD_DIR, "map_data.txt")
STUDENT_FILE = os.path.join(BUILD_DIR, "students.txt")

# ================= 核心工具函数 =================


def run_cpp(program, args=[]):
    """运行C++程序并捕获输出"""
    exe_path = os.path.join(BUILD_DIR, program)
    if not os.path.exists(exe_path) and os.path.exists(exe_path + ".exe"):
        exe_path += ".exe"

    if not os.path.exists(exe_path):
        return f"系统错误: 找不到可执行文件 {exe_path}"

    command = [exe_path] + args
    try:
        # 运行并获取输出
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        # 简单的错误检查：如果 stderr 有内容且 stdout 为空，可能是报错了
        if result.returncode != 0:
            return f"运行错误 (Code {result.returncode}): {result.stderr}"
        return result.stdout.strip()
    except Exception as e:
        return f"系统调用错误: {str(e)}"


def ensure_files_exist():
    """初始化必要文件"""
    if not os.path.exists(BUILD_DIR):
        os.makedirs(BUILD_DIR)
    if not os.path.exists(DICT_FILE):
        with open(DICT_FILE, "w") as f:
            f.write("Apple:苹果\nBanana:香蕉\nCat:猫")
    if not os.path.exists(MAP_FILE):
        with open(MAP_FILE, "w") as f:
            f.write(
                "LOCATIONS\n0 100 北门 学校正门\n1 95 图书馆 学习中心\n2 80 食堂 吃饭的地方\n3 90 宿舍 休息区\nEDGES\n0 1 200\n1 2 150\n2 3 100\n0 3 500"
            )
    if not os.path.exists(STUDENT_FILE):
        with open(STUDENT_FILE, "w") as f:
            f.write("1001 Bob 92\n1002 David 88\n1003 Alice 95.5")


ensure_files_exist()

# ================= 可视化辅助函数 =================


def draw_bst_from_file():
    if not os.path.exists(DICT_FILE):
        return None
    dot = graphviz.Digraph()
    dot.attr(rankdir="TB")
    dot.attr("node", shape="oval", style="filled", fillcolor="lightblue")

    # 简单构建 BST 逻辑用于绘图
    class Node:
        def __init__(self, key):
            self.key = key
            self.left = None
            self.right = None

    def insert(root, key):
        if not root:
            return Node(key)
        if key < root.key:
            root.left = insert(root.left, key)
        elif key > root.key:
            root.right = insert(root.right, key)
        return root

    words = [line.split(":")[0].strip() for line in open(DICT_FILE) if ":" in line]
    root = None
    for w in words:
        root = insert(root, w)

    def traverse(node):
        if node:
            dot.node(node.key)
            if node.left:
                dot.edge(node.key, node.left.key, "L")
                traverse(node.left)
            if node.right:
                dot.edge(node.key, node.right.key, "R")
                traverse(node.right)

    traverse(root)
    return dot


def draw_map_from_file():
    if not os.path.exists(MAP_FILE):
        return None
    dot = graphviz.Graph()
    dot.attr(rankdir="LR")
    dot.attr("node", shape="doublecircle", style="filled", fillcolor="lightgrey")
    content = open(MAP_FILE).read()
    for lid, _, name in re.findall(r"^(\d+)\s+\d+\s+(\S+)", content, re.MULTILINE):
        dot.node(lid, f"{name}\n({lid})")
    for u, v, w in re.findall(r"^(\d+)\s+(\d+)\s+(\d+)", content, re.MULTILINE):
        dot.edge(u, v, label=f"{w}m")
    return dot


# ================= 主界面 =================

st.title("🎓 数据结构课程设计系统")

with st.sidebar:
    st.header("功能菜单")
    menu = st.radio(
        "选择专题", ["1. 学生成绩管理", "2. 字典系统 (BST)", "3. 校园导航 (Graph)"]
    )
    st.markdown("---")
    st.caption("后端: C++ | 前端: Streamlit")

# --- Topic 1: 成绩管理 (优化了排序显示) ---
if "1." in menu:
    st.header("📊 专题一：学生成绩管理系统")
    tab1, tab2 = st.tabs(["📝 数据管理", "🚀 排序分析"])

    with tab1:
        # 显示原始数据
        try:
            df = pd.read_csv(
                STUDENT_FILE,
                sep=r"\s+",
                header=None,
                names=["学号", "姓名", "成绩"],
                dtype={"学号": str},
            )
            st.dataframe(df, use_container_width=True)
        except Exception as e:
            st.error(f"数据读取失败: {e}")

        # 增删表单
        c1, c2 = st.columns(2)
        with c1:
            with st.form("add"):
                st.write("➕ 添加学生")
                cols = st.columns(3)
                nid = cols[0].text_input("学号")
                nname = cols[1].text_input("姓名")
                nscore = cols[2].text_input("成绩")
                if st.form_submit_button("添加"):
                    if nid and nname and nscore:
                        with open(STUDENT_FILE, "a") as f:
                            f.write(f"\n{nid} {nname} {nscore}")
                        st.success("成功")
                        st.rerun()
        with c2:
            with st.form("del"):
                st.write("➖ 删除学生")
                did = st.text_input("输入学号")
                if st.form_submit_button("删除"):
                    lines = [l for l in open(STUDENT_FILE) if not l.startswith(did)]
                    with open(STUDENT_FILE, "w") as f:
                        f.writelines(lines)
                    st.success("已删除")
                    st.rerun()

    with tab2:
        st.subheader("算法排序结果")
        c1, c2 = st.columns(2)

        # 定义一个辅助函数来解析和显示结果
        def show_sorted_result(sort_type, cmd_args):
            res_text = run_cpp("Topic1_Student", cmd_args)
            if "错误" in res_text or "Error" in res_text:
                st.error(res_text)
            else:
                try:
                    # 关键修改：把字符串当做 CSV 文件读入 pandas
                    df_sorted = pd.read_csv(
                        io.StringIO(res_text),
                        sep=r"\s+",
                        header=None,
                        names=["学号", "姓名", "成绩"],
                        dtype={"学号": str},
                    )
                    st.success(f"✅ {sort_type} 完成")
                    st.dataframe(df_sorted, use_container_width=True)  # 使用表格显示
                except Exception as e:
                    st.error("解析排序结果失败，原始输出如下：")
                    st.text(res_text)

        with c1:
            st.markdown("### ⚡ 按学号 (QuickSort)")
            if st.button("执行快速排序"):
                show_sorted_result("快速排序", ["sort_id", STUDENT_FILE])

        with c2:
            st.markdown("### 🌳 按成绩 (HeapSort)")
            if st.button("执行堆排序"):
                show_sorted_result("堆排序", ["sort_score", STUDENT_FILE])

# --- Topic 2: 字典系统 ---
elif "2." in menu:
    st.header("🌲 专题二：字典检索系统")
    t1, t2 = st.tabs(["🔍 查询", "📝 管理"])

    with t1:
        c1, c2 = st.columns([1, 1.5])
        with c1:
            w = st.text_input("输入单词")
            if st.button("精确查找"):
                st.info(run_cpp("Topic2_Dictionary", ["search", w]))
            if st.button("前缀匹配"):
                st.text(run_cpp("Topic2_Dictionary", ["fuzzy", w]))
        with c2:
            if st.button("刷新树结构"):
                try:
                    st.graphviz_chart(draw_bst_from_file())
                except:
                    st.warning("无法绘图")
    with t2:
        c1, c2 = st.columns(2)
        with c1:
            with st.form("add_w"):
                w = st.text_input("单词")
                m = st.text_input("释义")
                if st.form_submit_button("保存"):
                    with open(DICT_FILE, "a") as f:
                        f.write(f"\n{w}:{m}")
                    st.success("已保存")
        with c2:
            with st.form("del_w"):
                dw = st.text_input("删除单词")
                if st.form_submit_button("删除"):
                    lines = [l for l in open(DICT_FILE) if not l.startswith(dw + ":")]
                    with open(DICT_FILE, "w") as f:
                        f.writelines(lines)
                    st.success("已删除")
        st.text_area("当前数据", open(DICT_FILE).read(), height=150)

# --- Topic 3: 校园导航 ---
elif "3." in menu:
    st.header("🗺️ 专题三：校园导航系统")
    t1, t2 = st.tabs(["📍 导航", "🛠️ 编辑"])

    locs = []
    if os.path.exists(MAP_FILE):
        locs = [
            f"{i}: {n}"
            for i, _, n in re.findall(
                r"^(\d+)\s+\d+\s+(\S+)", open(MAP_FILE).read(), re.MULTILINE
            )
        ]

    with t1:
        c1, c2 = st.columns([1, 2])
        with c1:
            if locs:
                s = st.selectbox("起点", locs, 0)
                e = st.selectbox("终点", locs, min(1, len(locs) - 1))
                if st.button("规划路径"):
                    res = run_cpp(
                        "Topic3_Campus", ["path", s.split(":")[0], e.split(":")[0]]
                    )
                    if "Path:" in res:
                        st.success("✅ 规划成功")
                        st.code(res)
                    else:
                        st.error(res)
        with c2:
            if st.button("刷新地图"):
                try:
                    st.graphviz_chart(draw_map_from_file())
                except:
                    st.warning("绘图失败")
    with t2:
        st.text_area(
            "地图数据 (直接编辑保存)",
            open(MAP_FILE).read(),
            height=200,
            key="map_editor",
        )
        if st.button("保存修改"):
            with open(MAP_FILE, "w") as f:
                f.write(st.session_state.map_editor)
            st.success("已更新地图数据")
