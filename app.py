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
DICT_FILE = "dictionary.txt"
MAP_FILE = "map_data.txt"

# --- Helpers ---
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

def ensure_files():
    # Topic 1
    if not os.path.exists(STUDENTS_FILE):
        with open(STUDENTS_FILE, "w") as f:
            f.write("1003 Alice 85.5\n1001 Bob 92.0\n1005 Charlie 78.5\n1002 David 88.0\n1004 Eve 95.5\n")

    # Topic 2
    if not os.path.exists(DICT_FILE):
        with open(DICT_FILE, "w") as f:
            f.write("Apple:A fruit\nBanana:A long yellow fruit\nCat:A small animal\n")

    # Topic 3
    if not os.path.exists(MAP_FILE):
        with open(MAP_FILE, "w") as f:
            f.write("LOCATIONS\n")
            f.write("1 80 Main_Gate The_main_entrance.\n")
            f.write("2 95 Library A_quiet_place_to_study.\n")
            f.write("3 90 Canteen Tasty_and_cheap_food.\n")
            f.write("EDGES\n")
            f.write("1 2 500\n")
            f.write("2 3 200\n")

# --- UI Setup ---
st.set_page_config(page_title="Data Structure Final Project", layout="wide")
st.title("Data Structure Final Project - Interactive System")
ensure_files()

topic = st.sidebar.radio(
    "Select Topic",
    ("Topic 1: Grade System", "Topic 2: Dictionary System", "Topic 3: Campus Navigation")
)

# ==========================================
# Topic 1: Grade System
# ==========================================
if topic == "Topic 1: Grade System":
    st.header("Topic 1: Grade System Management")

    tab1, tab2 = st.tabs(["Sort & View", "Manage Data"])

    with tab1:
        st.subheader("Sorting (C++ Backend)")
        c1, c2 = st.columns(2)
        with c1:
            if st.button("Sort by ID"):
                success, output = run_cpp_command(TOPIC1_EXE, ["sort_id", STUDENTS_FILE])
                if success:
                    st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
                else:
                    st.error(output)
        with c2:
            if st.button("Sort by Score"):
                success, output = run_cpp_command(TOPIC1_EXE, ["sort_score", STUDENTS_FILE])
                if success:
                    st.dataframe(pd.read_csv(io.StringIO(output)), use_container_width=True)
                else:
                    st.error(output)

    with tab2:
        st.subheader("Manage Students (Python)")
        # Display current file
        if os.path.exists(STUDENTS_FILE):
            with open(STUDENTS_FILE, "r") as f:
                lines = f.readlines()
            data = []
            for line in lines:
                parts = line.split()
                if len(parts) >= 3:
                    data.append({"ID": parts[0], "Name": parts[1], "Score": parts[2]})
            st.dataframe(pd.DataFrame(data), use_container_width=True)

        c1, c2 = st.columns(2)
        with c1:
            with st.form("add_stud"):
                nid = st.text_input("ID")
                nname = st.text_input("Name")
                nscore = st.text_input("Score")
                if st.form_submit_button("Add Student"):
                    with open(STUDENTS_FILE, "a") as f:
                        f.write(f"\n{nid} {nname} {nscore}")
                    st.success("Added.")
                    st.rerun()
        with c2:
            del_id = st.text_input("Delete ID")
            if st.button("Delete Student"):
                with open(STUDENTS_FILE, "r") as f: lines = f.readlines()
                with open(STUDENTS_FILE, "w") as f:
                    for l in lines:
                        if not l.startswith(del_id + " "): f.write(l)
                st.success("Deleted.")
                st.rerun()

# ==========================================
# Topic 2: Dictionary System
# ==========================================
elif topic == "Topic 2: Dictionary System":
    st.header("Topic 2: Dictionary System")
    tab1, tab2 = st.tabs(["Search & Visualize", "Manage Dictionary"])

    with tab1:
        st.subheader("Search")
        c1, c2 = st.columns(2)
        with c1:
            s_word = st.text_input("Exact Search")
            if st.button("Search"):
                s, o = run_cpp_command(TOPIC2_EXE, ["search", s_word])
                if s: st.info(o)
                else: st.error(o)
        with c2:
            f_word = st.text_input("Fuzzy Search (Prefix)")
            if st.button("Fuzzy Search"):
                s, o = run_cpp_command(TOPIC2_EXE, ["fuzzy", f_word])
                if s: st.text(o)
                else: st.error(o)

        st.divider()
        st.subheader("Tree Visualization")
        if st.button("Visualize Tree"):
            s, o = run_cpp_command(TOPIC2_EXE, ["print_tree"])
            if s:
                try:
                    tree_json = json.loads(o)
                    graph = graphviz.Digraph()
                    graph.attr(rankdir='TB')
                    def add_nodes(node, parent=None):
                        name = node.get("name")
                        if not name: return
                        graph.node(name, label=name)
                        if parent: graph.edge(parent, name)
                        for child in node.get("children", []):
                            add_nodes(child, name)
                    add_nodes(tree_json)
                    st.graphviz_chart(graph)
                except: st.error("JSON Error")

    with tab2:
        st.subheader("Manage Dictionary")
        # Display
        if os.path.exists(DICT_FILE):
            with open(DICT_FILE, "r") as f: lines = f.readlines()
            data = []
            for l in lines:
                if ":" in l:
                    w, m = l.strip().split(":", 1)
                    data.append({"Word": w, "Meaning": m})
            st.dataframe(pd.DataFrame(data), use_container_width=True)

        c1, c2 = st.columns(2)
        with c1:
            with st.form("add_word"):
                nw = st.text_input("Word")
                nm = st.text_input("Meaning")
                if st.form_submit_button("Insert"):
                    with open(DICT_FILE, "a") as f:
                        f.write(f"{nw}:{nm}\n")
                    st.success("Inserted.")
                    st.rerun()
        with c2:
            del_w = st.text_input("Delete Word")
            if st.button("Delete"):
                with open(DICT_FILE, "r") as f: lines = f.readlines()
                with open(DICT_FILE, "w") as f:
                    for l in lines:
                        if not l.startswith(del_w + ":"): f.write(l)
                st.success("Deleted.")
                st.rerun()

# ==========================================
# Topic 3: Campus Navigation
# ==========================================
elif topic == "Topic 3: Campus Navigation":
    st.header("Topic 3: Campus Navigation")
    tab1, tab2 = st.tabs(["Navigation", "Map Editor"])

    # Common Data Load
    s_loc, o_loc = run_cpp_command(TOPIC3_EXE, ["locations"])
    df_loc = pd.DataFrame()
    if s_loc:
        try: df_loc = pd.read_csv(io.StringIO(o_loc))
        except: pass

    with tab1:
        st.subheader("Navigation")
        if not df_loc.empty:
            id_map = {row['id']: f"{row['name']} ({row['id']})" for _, row in df_loc.iterrows()}
            c1, c2 = st.columns(2)
            with c1: start = st.selectbox("Start", options=id_map.keys(), format_func=lambda x: id_map[x])
            with c2: end = st.selectbox("End", options=id_map.keys(), format_func=lambda x: id_map[x], index=min(1, len(id_map)-1))

            if st.button("Find Path"):
                s, o = run_cpp_command(TOPIC3_EXE, ["path", str(start), str(end)])
                if s:
                    if "|" in o:
                        path_p, dist_p = o.split("|")
                        st.success(f"**{dist_p.strip()}**")
                        st.info(path_p.strip())
                    else: st.warning(o)
                else: st.error(o)

        st.subheader("Keyword Search")
        k = st.text_input("Search Location")
        if st.button("Search Spot"):
            s, o = run_cpp_command(TOPIC3_EXE, ["search", k])
            if s: st.text(o)
            else: st.error(o)

    with tab2:
        st.subheader("Map Editor")
        st.dataframe(df_loc, use_container_width=True)

        with st.expander("Add Location"):
            with st.form("add_loc"):
                lid = st.number_input("ID", step=1)
                lname = st.text_input("Name")
                lpop = st.number_input("Popularity")
                linfo = st.text_input("Info")
                if st.form_submit_button("Add"):
                    # Insert before EDGES
                    with open(MAP_FILE, "r") as f: lines = f.readlines()
                    idx = 0
                    for i, l in enumerate(lines):
                        if "EDGES" in l:
                            idx = i
                            break
                    if idx == 0: idx = len(lines) # Append if no edges
                    lines.insert(idx, f"{lid} {lpop} {lname} {linfo}\n")
                    with open(MAP_FILE, "w") as f: f.writelines(lines)
                    st.success("Added Location.")
                    st.rerun()

        with st.expander("Add Path"):
            with st.form("add_path"):
                u = st.number_input("Start ID", step=1)
                v = st.number_input("End ID", step=1)
                w = st.number_input("Weight", step=1)
                if st.form_submit_button("Add Edge"):
                    # Append to end (after EDGES)
                    with open(MAP_FILE, "a") as f:
                        f.write(f"{u} {v} {w}\n")
                    st.success("Added Edge.")
                    st.rerun()

        st.subheader("Map Visualization")
        s_edge, o_edge = run_cpp_command(TOPIC3_EXE, ["edges"])
        if st.button("Refresh Map"):
            if s_loc and s_edge:
                try:
                    df_edge = pd.read_csv(io.StringIO(o_edge))
                    g = graphviz.Graph()
                    g.attr(rankdir='LR')
                    for _, r in df_loc.iterrows():
                        g.node(str(r['id']), label=f"{r['name']}\nPop:{r['popularity']}")
                    for _, r in df_edge.iterrows():
                        g.edge(str(r['u']), str(r['v']), label=str(r['weight']))
                    st.graphviz_chart(g)
                except: st.error("Viz Error")

st.markdown("---")
st.caption("Auto-syncs with .txt files. C++ performs calculations.")
