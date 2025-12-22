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

        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=True
        )
        return True, result.stdout.strip()
    except subprocess.CalledProcessError as e:
        return False, f"Error running command: {e}\nStderr: {e.stderr}"
    except Exception as e:
        return False, f"Error: {str(e)}"

def ensure_students_file():
    if not os.path.exists(STUDENTS_FILE):
        with open(STUDENTS_FILE, "w") as f:
            # Add some initial data matching initStudents in C++
            f.write("1003 Alice 85.5\n")
            f.write("1001 Bob 92.0\n")
            f.write("1005 Charlie 78.5\n")
            f.write("1002 David 88.0\n")
            f.write("1004 Eve 95.5\n")

st.set_page_config(page_title="Data Structure Final Project", layout="wide")
st.title("Data Structure Final Project - Enhanced Version")

topic = st.sidebar.radio(
    "Select Topic",
    ("Topic 1: Grade System (File I/O)", "Topic 2: Dictionary (Visualized)", "Topic 3: Campus Map (Visualized)")
)

# --- Topic 1 ---
if topic == "Topic 1: Grade System (File I/O)":
    st.header("Topic 1: Grade System Management")
    ensure_students_file()

    # Manage File
    st.subheader("Manage Students")

    # Read current file to display raw or for delete
    try:
        with open(STUDENTS_FILE, "r") as f:
            lines = f.readlines()

        # Parse for display
        data = []
        for line in lines:
            parts = line.split()
            if len(parts) >= 3:
                data.append({"ID": parts[0], "Name": parts[1], "Score": parts[2]})
        st.dataframe(pd.DataFrame(data), use_container_width=True)

        # Add Student
        with st.expander("Add New Student"):
            with st.form("add_student"):
                new_id = st.text_input("ID")
                new_name = st.text_input("Name (No spaces)")
                new_score = st.text_input("Score")
                submitted = st.form_submit_button("Add")
                if submitted:
                    if new_id and new_name and new_score:
                        with open(STUDENTS_FILE, "a") as f:
                            f.write(f"\n{new_id} {new_name} {new_score}")
                        st.success("Student added!")
                        st.rerun()
                    else:
                        st.error("Please fill all fields.")

        # Delete Student
        with st.expander("Delete Student"):
            del_id = st.text_input("Enter ID to Delete")
            if st.button("Delete"):
                new_lines = [l for l in lines if not l.startswith(del_id + " ")]
                if len(new_lines) < len(lines):
                    with open(STUDENTS_FILE, "w") as f:
                        f.writelines(new_lines)
                    st.success(f"Student {del_id} deleted.")
                    st.rerun()
                else:
                    st.warning("ID not found.")

    except Exception as e:
        st.error(f"Error reading file: {e}")

    st.subheader("Sorting (C++ Backend)")
    col1, col2 = st.columns(2)
    with col1:
        if st.button("Sort by ID"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_id", STUDENTS_FILE])
            if success:
                st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
            else:
                st.error(output)
    with col2:
        if st.button("Sort by Score"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_score", STUDENTS_FILE])
            if success:
                st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
            else:
                st.error(output)

# --- Topic 2 ---
elif topic == "Topic 2: Dictionary (Visualized)":
    st.header("Topic 2: Dictionary System")

    col1, col2 = st.columns([1, 2])

    with col1:
        st.subheader("Search")
        word = st.text_input("Word")
        if st.button("Exact Search"):
            success, output = run_cpp_command(TOPIC2_EXE, ["search", word])
            if success:
                st.info(output)
            else:
                st.error(output)

        prefix = st.text_input("Fuzzy Prefix")
        if st.button("Fuzzy Search"):
            success, output = run_cpp_command(TOPIC2_EXE, ["fuzzy", prefix])
            if success:
                st.text(output) # Output is formatted text from C++
            else:
                st.error(output)

    with col2:
        st.subheader("Tree Visualization")
        if st.button("Visualize Tree"):
            success, output = run_cpp_command(TOPIC2_EXE, ["print_tree"])
            if success:
                try:
                    tree_json = json.loads(output)

                    # Build Graphviz
                    graph = graphviz.Digraph()
                    graph.attr(rankdir='TB')

                    def add_nodes(node, parent_name=None):
                        name = node.get("name")
                        if not name: return

                        graph.node(name, label=name)
                        if parent_name:
                            graph.edge(parent_name, name)

                        children = node.get("children", [])
                        for child in children:
                            add_nodes(child, name)

                    add_nodes(tree_json)
                    st.graphviz_chart(graph)
                except Exception as e:
                    st.error(f"Error parsing JSON for visualization: {e}")
                    st.text(output)
            else:
                st.error(output)

# --- Topic 3 ---
elif topic == "Topic 3: Campus Map (Visualized)":
    st.header("Topic 3: Campus Navigation")

    # Load Data
    success_loc, out_loc = run_cpp_command(TOPIC3_EXE, ["locations"])
    success_edge, out_edge = run_cpp_command(TOPIC3_EXE, ["edges"])

    if success_loc and success_edge:
        try:
            df_loc = pd.read_csv(io.StringIO(out_loc))
            df_edge = pd.read_csv(io.StringIO(out_edge))

            # 1. Visualization
            st.subheader("Campus Map")
            graph = graphviz.Graph() # Undirected for campus paths usually
            graph.attr(rankdir='LR')

            # Add nodes
            # df_loc columns: id,name,popularity,info
            for _, row in df_loc.iterrows():
                label = f"{row['name']}\n(Pop: {row['popularity']})"
                graph.node(str(row['id']), label=label, shape='box')

            # Add edges
            # df_edge columns: u,v,weight
            for _, row in df_edge.iterrows():
                graph.edge(str(row['u']), str(row['v']), label=str(row['weight']))

            st.graphviz_chart(graph)

            # 2. Path Finding
            st.subheader("Find Shortest Path")
            # Map ID to Name for dropdown
            id_to_name = {row['id']: f"{row['name']} (ID:{row['id']})" for _, row in df_loc.iterrows()}

            c1, c2 = st.columns(2)
            with c1:
                start = st.selectbox("Start", options=list(id_to_name.keys()), format_func=lambda x: id_to_name[x])
            with c2:
                end = st.selectbox("End", options=list(id_to_name.keys()), format_func=lambda x: id_to_name[x], index=min(1, len(id_to_name)-1))

            if st.button("Calculate Path"):
                s, o = run_cpp_command(TOPIC3_EXE, ["path", str(start), str(end)])
                if s:
                    # Output: Path: A->B->C | Total Distance: 500
                    # Let's parse it to display nicely
                    if "|" in o:
                        parts = o.split("|")
                        path_str = parts[0].replace("Path: ", "").strip()
                        dist_str = parts[1].strip()

                        st.success(f"**{dist_str}**")

                        # Visual path
                        nodes = path_str.split("->")
                        st.write(" -> ".join([f"`{n}`" for n in nodes]))
                    else:
                        st.info(o)
                else:
                    st.error(o)

        except Exception as e:
            st.error(f"Error processing map data: {e}")
    else:
        st.error("Failed to load map data from C++ backend.")

st.markdown("---")
st.caption("Advanced Data Structures Project")
