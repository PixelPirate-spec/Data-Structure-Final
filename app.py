import streamlit as st
import subprocess
import pandas as pd
import io
import os
import json
import graphviz

# Configuration
BUILD_DIR = "build"
TOPIC1_EXE = os.path.join(BUILD_DIR, "Topic1_Student")
TOPIC2_EXE = os.path.join(BUILD_DIR, "Topic2_Dictionary")
TOPIC3_EXE = os.path.join(BUILD_DIR, "Topic3_Campus")

STUDENTS_FILE = "students.txt"


def run_cpp_command(executable, args):
    """Run C++ executable and capture output."""
    command = [executable] + args
    try:
        if not os.path.exists(executable):
            return False, f"Executable not found: {executable}. Please compile."

        result = subprocess.run(command, capture_output=True, text=True, check=True)
        return True, result.stdout.strip()
    except subprocess.CalledProcessError as e:
        return False, f"Error running command: {e}\nStderr: {e.stderr}"
    except Exception as e:
        return False, f"Error: {str(e)}"


def ensure_students_file():
    if not os.path.exists(STUDENTS_FILE):
        with open(STUDENTS_FILE, "w") as f:
            # Add some initial data matching initStudents in C++
            f.write("1003 爱丽丝 85.5\n")
            f.write("1001 鲍勃 92.0\n")
            f.write("1005 查理 78.5\n")
            f.write("1002 大卫 88.0\n")
            f.write("1004 伊芙 95.5\n")


st.set_page_config(page_title="数据结构期末项目", layout="wide")
st.title("数据结构期末项目 - 增强版")

topic = st.sidebar.radio(
    "选择主题",
    (
        "主题 1：成绩系统 (文件读写)",
        "主题 2：字典系统 (可视化)",
        "主题 3：校园地图 (可视化)",
    ),
)

# --- Topic 1 ---
if topic == "主题 1：成绩系统 (文件读写)":
    st.header("主题 1：成绩系统管理")
    ensure_students_file()

    # Manage File
    st.subheader("管理学生")

    # Read current file to display raw or for delete
    try:
        with open(STUDENTS_FILE, "r") as f:
            lines = f.readlines()

        # Parse for display
        data = []
        for line in lines:
            parts = line.split()
            if len(parts) >= 3:
                data.append({"学号": parts[0], "姓名": parts[1], "成绩": parts[2]})
        st.dataframe(pd.DataFrame(data), use_container_width=True)

        # Add Student
        with st.expander("添加新学生"):
            with st.form("add_student"):
                new_id = st.text_input("学号")
                new_name = st.text_input("姓名 (无空格)")
                new_score = st.text_input("成绩")
                submitted = st.form_submit_button("添加")
                if submitted:
                    if new_id and new_name and new_score:
                        with open(STUDENTS_FILE, "a") as f:
                            f.write(f"\n{new_id} {new_name} {new_score}")
                        st.success("学生已添加！")
                        st.rerun()
                    else:
                        st.error("请填写所有字段。")

        # Delete Student
        with st.expander("删除学生"):
            del_id = st.text_input("输入要删除的学号")
            if st.button("删除"):
                new_lines = [l for l in lines if not l.startswith(del_id + " ")]
                if len(new_lines) < len(lines):
                    with open(STUDENTS_FILE, "w") as f:
                        f.writelines(new_lines)
                    st.success(f"学生 {del_id} 已删除。")
                    st.rerun()
                else:
                    st.warning("未找到该学号。")

    except Exception as e:
        st.error(f"读取文件错误: {e}")

    st.subheader("排序 (C++ 后端)")
    col1, col2 = st.columns(2)
    with col1:
        if st.button("按学号排序"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_id", STUDENTS_FILE])
            if success:
                st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
            else:
                st.error(output)
    with col2:
        if st.button("按成绩排序"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_score", STUDENTS_FILE])
            if success:
                st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
            else:
                st.error(output)

# --- Topic 2 ---
elif topic == "主题 2：字典系统 (可视化)":
    st.header("主题 2：字典系统")

    col1, col2 = st.columns([1, 2])

    with col1:
        st.subheader("搜索")
        word = st.text_input("单词")
        if st.button("精确搜索"):
            success, output = run_cpp_command(TOPIC2_EXE, ["search", word])
            if success:
                st.info(output)
            else:
                st.error(output)

        prefix = st.text_input("模糊前缀")
        if st.button("模糊搜索"):
            success, output = run_cpp_command(TOPIC2_EXE, ["fuzzy", prefix])
            if success:
                st.text(output)  # Output is formatted text from C++
            else:
                st.error(output)

    with col2:
        st.subheader("树形可视化")
        if st.button("生成树形图"):
            success, output = run_cpp_command(TOPIC2_EXE, ["print_tree"])
            if success:
                try:
                    tree_json = json.loads(output)

                    # Build Graphviz
                    graph = graphviz.Digraph()
                    graph.attr(rankdir="TB")

                    def add_nodes(node, parent_name=None):
                        name = node.get("name")
                        if not name:
                            return

                        graph.node(name, label=name)
                        if parent_name:
                            graph.edge(parent_name, name)

                        children = node.get("children", [])
                        for child in children:
                            add_nodes(child, name)

                    add_nodes(tree_json)
                    st.graphviz_chart(graph)
                except Exception as e:
                    st.error(f"解析 JSON 可视化错误: {e}")
                    st.text(output)
            else:
                st.error(output)

# --- Topic 3 ---
elif topic == "主题 3：校园地图 (可视化)":
    st.header("主题 3：校园导航")

    # Load Data
    success_loc, out_loc = run_cpp_command(TOPIC3_EXE, ["locations"])
    success_edge, out_edge = run_cpp_command(TOPIC3_EXE, ["edges"])

    if success_loc and success_edge:
        try:
            df_loc = pd.read_csv(io.StringIO(out_loc))
            df_edge = pd.read_csv(io.StringIO(out_edge))

            # 1. Visualization
            st.subheader("校园地图")
            graph = graphviz.Graph()  # Undirected for campus paths usually
            graph.attr(rankdir="LR")

            # Add nodes
            # df_loc columns: id,name,popularity,info
            for _, row in df_loc.iterrows():
                label = f"{row['name']}\n(热度: {row['popularity']})"
                graph.node(str(row["id"]), label=label, shape="box")

            # Add edges
            # df_edge columns: u,v,weight
            for _, row in df_edge.iterrows():
                graph.edge(str(row["u"]), str(row["v"]), label=str(row["weight"]))

            st.graphviz_chart(graph)

            # 2. Path Finding
            st.subheader("查找最短路径")
            # Map ID to Name for dropdown
            id_to_name = {
                row["id"]: f"{row['name']} (ID:{row['id']})"
                for _, row in df_loc.iterrows()
            }

            c1, c2 = st.columns(2)
            with c1:
                start = st.selectbox(
                    "起点",
                    options=list(id_to_name.keys()),
                    format_func=lambda x: id_to_name[x],
                )
            with c2:
                end = st.selectbox(
                    "终点",
                    options=list(id_to_name.keys()),
                    format_func=lambda x: id_to_name[x],
                    index=min(1, len(id_to_name) - 1),
                )

            if st.button("计算路径"):
                s, o = run_cpp_command(TOPIC3_EXE, ["path", str(start), str(end)])
                if s:
                    # Output: Path: A->B->C | Total Distance: 500
                    # Let's parse it to display nicely
                    if "|" in o:
                        parts = o.split("|")
                        path_str = parts[0].replace("Path: ", "").strip()
                        dist_str = parts[1].strip()
                        dist_str = dist_str.replace("Total Distance", "总距离")

                        st.success(f"**{dist_str}**")

                        # Visual path
                        nodes = path_str.split("->")
                        st.write(" -> ".join([f"`{n}`" for n in nodes]))
                    else:
                        st.info(o)
                else:
                    st.error(o)

        except Exception as e:
            st.error(f"处理地图数据错误: {e}")
    else:
        st.error("无法从 C++ 后端加载地图数据。")

st.markdown("---")
st.caption("高级数据结构项目")
