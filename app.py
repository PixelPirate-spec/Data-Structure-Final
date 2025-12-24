import streamlit as st
import subprocess
import pandas as pd
import os
import graphviz
import re
import io

# ================= 配置区 =================
st.set_page_config(
    page_title="数据结构大作业集成系统",
    page_icon="🎓",
    layout="wide",  # 保持宽屏模式
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
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        if result.returncode != 0:
            return f"运行异常 (Code {result.returncode}):\n{result.stderr}\n{result.stdout}"
        return result.stdout.strip()
    except Exception as e:
        return f"系统调用错误: {str(e)}"


def ensure_files_exist():
    if not os.path.exists(BUILD_DIR):
        os.makedirs(BUILD_DIR)
    if not os.path.exists(DICT_FILE):
        with open(DICT_FILE, "w") as f:
            f.write("Apple:苹果\nBanana:香蕉\nCat:猫")
    if not os.path.exists(MAP_FILE):
        content = """LOCATIONS
0 100 北门 学校正门
1 95 图书馆 学习中心
2 80 食堂 吃饭的地方
3 90 宿舍 休息区
4 85 体育馆 运动场所
EDGES
0 1 200
1 2 150
2 3 100
0 3 500
1 4 300
3 4 200"""
        with open(MAP_FILE, "w") as f:
            f.write(content)
    if not os.path.exists(STUDENT_FILE):
        with open(STUDENT_FILE, "w") as f:
            f.write("1001 Bob 92\n1002 David 88\n1003 Alice 95.5")


ensure_files_exist()

# ================= 更加健壮的解析器 =================


def parse_map_data_robust():
    """严格分段解析地图文件"""
    locations = {}  # {id: name}
    edges = []  # [(u, v, w)]

    if not os.path.exists(MAP_FILE):
        return locations, edges

    mode = None
    with open(MAP_FILE, "r") as f:
        lines = f.readlines()

    for line in lines:
        line = line.strip()
        if not line:
            continue

        if "LOCATIONS" in line:
            mode = "LOC"
            continue
        elif "EDGES" in line:
            mode = "EDGE"
            continue

        parts = line.split()
        if mode == "LOC":
            if len(parts) >= 3:
                lid = parts[0]
                name = parts[2]
                locations[lid] = name
        elif mode == "EDGE":
            if len(parts) >= 3:
                u, v, w = parts[0], parts[1], parts[2]
                edges.append((u, v, w))

    return locations, edges


# ================= 可视化绘图函数 (恢复自适应) =================


def draw_bst_from_file():
    if not os.path.exists(DICT_FILE):
        return None
    dot = graphviz.Digraph()

    # 恢复标准参数，移除强制放大
    dot.attr(rankdir="TB")
    dot.attr("node", shape="oval", style="filled", fillcolor="lightblue")

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


def draw_map_viz():
    """使用新的解析器绘制地图"""
    locs, edges = parse_map_data_robust()
    if not locs:
        return None

    dot = graphviz.Graph()

    # 恢复标准参数，移除强制宽高
    dot.attr(rankdir="LR")
    # 不再强制设置 width="1.8" 和 fontsize="18"，让它自动适应
    dot.attr("node", shape="doublecircle", style="filled", fillcolor="#f0f2f6")

    # 画节点
    for lid, name in locs.items():
        dot.node(lid, f"{name}\n({lid})")

    # 画边
    for u, v, w in edges:
        if u in locs and v in locs:
            dot.edge(u, v, label=f"{w}m")
        else:
            dot.node(u, u)
            dot.node(v, v)
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

# --- Topic 1: 成绩管理 ---
if "1." in menu:
    st.header("📊 专题一：学生成绩管理系统")
    tab1, tab2 = st.tabs(["📝 数据管理", "🚀 排序分析"])

    with tab1:
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
            st.warning(f"数据读取提示: {e}")

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

        def show_sorted_result(sort_type, cmd_args):
            res_text = run_cpp("Topic1_Student", cmd_args)
            if "Error" in res_text:
                st.error(res_text)
            else:
                try:
                    separator = "," if "," in res_text else r"\s+"
                    df_sorted = pd.read_csv(
                        io.StringIO(res_text), sep=separator, dtype={"学号": str}
                    )
                    st.success(f"✅ {sort_type} 完成")
                    st.dataframe(df_sorted, use_container_width=True)
                except Exception as e:
                    st.error("解析失败")
                    st.text(res_text)

        with c1:
            if st.button("按学号排序 (QuickSort)"):
                show_sorted_result("快速排序", ["sort_id", STUDENT_FILE])
        with c2:
            if st.button("按成绩排序 (HeapSort)"):
                show_sorted_result("堆排序", ["sort_score", STUDENT_FILE])

# --- Topic 2: 字典系统 ---
elif "2." in menu:
    st.header("🌲 专题二：字典检索系统")
    t1, t2 = st.tabs(["🔍 查询", "📝 管理"])

    with t1:
        # 保持 1:3 比例，这样图还是有足够空间，但不强制放大了
        c1, c2 = st.columns([1, 3])
        with c1:
            w = st.text_input("输入单词")
            if st.button("精确查找"):
                st.info(run_cpp("Topic2_Dictionary", ["search", w]))
            if st.button("前缀匹配"):
                st.text(run_cpp("Topic2_Dictionary", ["fuzzy", w]))
            st.divider()
            if st.button("刷新树结构"):
                st.rerun()

        with c2:
            st.subheader("树形结构可视化")
            try:
                # use_container_width=True 会让图自动适应这个 3/4 宽度的容器，不会溢出
                st.graphviz_chart(draw_bst_from_file(), use_container_width=True)
            except:
                st.warning("请安装 graphviz")
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

    locs_dict, _ = parse_map_data_robust()
    loc_options = [f"{lid}: {name}" for lid, name in locs_dict.items()]

    with t1:
        # 保持 1:3 比例
        c1, c2 = st.columns([1, 3])
        with c1:
            if loc_options:
                s = st.selectbox("起点", loc_options, 0)
                e = st.selectbox("终点", loc_options, min(1, len(loc_options) - 1))
                if st.button("规划路径", type="primary"):
                    sid = s.split(":")[0]
                    eid = e.split(":")[0]
                    res = run_cpp("Topic3_Campus", ["path", sid, eid, MAP_FILE])
                    if "Path:" in res or "->" in res:
                        st.success("✅ 规划成功")
                        st.info(res)
                    else:
                        st.error(res)
            else:
                st.error("地图数据为空")

            st.divider()
            if st.button("刷新地图"):
                st.rerun()

        with c2:
            st.subheader("校园地图拓扑")
            try:
                chart = draw_map_viz()
                if chart:
                    # 关键：不强制 Graphviz 大小，而是让 Streamlit 容器去自适应它
                    st.graphviz_chart(chart, use_container_width=True)
                else:
                    st.warning("无数据")
            except Exception as e:
                st.warning(f"绘图失败: {e}")
    with t2:
        st.markdown("⚠️ **注意**: 编辑时请保留 `LOCATIONS` 和 `EDGES` 标记。")
        st.text_area("地图数据", open(MAP_FILE).read(), height=300, key="map_editor")
        if st.button("保存修改"):
            with open(MAP_FILE, "w") as f:
                f.write(st.session_state.map_editor)
            st.success("已更新")
