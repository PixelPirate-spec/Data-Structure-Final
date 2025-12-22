import streamlit as st
import subprocess
import pandas as pd
import io
import os
import json

# Configuration for Executable Paths
# Assuming the build was done in 'build' directory and executables are named as defined in CMakeLists.txt
BUILD_DIR = "build"
TOPIC1_EXE = os.path.join(BUILD_DIR, "Topic1_Student")
TOPIC2_EXE = os.path.join(BUILD_DIR, "Topic2_Dictionary")
TOPIC3_EXE = os.path.join(BUILD_DIR, "Topic3_Campus")

def run_cpp_command(executable, args):
    """
    Helper function to run C++ executable and capture output.
    """
    command = [executable] + args
    try:
        if not os.path.exists(executable):
             return False, f"Executable not found: {executable}. Please compile the project first."

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
        return False, f"An unexpected error occurred: {str(e)}"

st.set_page_config(page_title="Data Structure Final Project", layout="wide")
st.title("Data Structure Final Project - C++ & Python Integration")

# Sidebar for Navigation
topic = st.sidebar.radio(
    "Select Topic",
    ("Topic 1: Student Grade System", "Topic 2: Dictionary System", "Topic 3: Campus Navigation")
)

# --- Topic 1: Student Grade System ---
if topic == "Topic 1: Student Grade System":
    st.header("Topic 1: Student Grade System")
    st.markdown("Sort students by ID (QuickSort) or Score (HeapSort).")

    col1, col2 = st.columns(2)

    with col1:
        if st.button("Load & Sort by ID (Ascending)"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_id"])
            if success:
                try:
                    df = pd.read_csv(io.StringIO(output))
                    st.dataframe(df, use_container_width=True)
                    st.success("Sorted by ID successfully.")
                except Exception as e:
                    st.error(f"Failed to parse CSV output: {e}")
                    st.text(output)
            else:
                st.error(output)

    with col2:
        if st.button("Load & Sort by Score (Descending)"):
            success, output = run_cpp_command(TOPIC1_EXE, ["sort_score"])
            if success:
                try:
                    df = pd.read_csv(io.StringIO(output))
                    st.dataframe(df, use_container_width=True)
                    st.success("Sorted by Score successfully.")
                except Exception as e:
                    st.error(f"Failed to parse CSV output: {e}")
                    st.text(output)
            else:
                st.error(output)

# --- Topic 2: Dictionary System ---
elif topic == "Topic 2: Dictionary System":
    st.header("Topic 2: Dictionary System")
    st.markdown("Search for words or visualize the Binary Search Tree.")

    search_word = st.text_input("Enter a word to search:")

    if st.button("Search Meaning"):
        if search_word:
            success, output = run_cpp_command(TOPIC2_EXE, ["search", search_word])
            if success:
                if "not found" in output.lower():
                    st.warning(output)
                else:
                    st.success(f"**Meaning:** {output}")
            else:
                st.error(output)
        else:
            st.warning("Please enter a word.")

    st.divider()

    if st.button("Visualize Tree Structure"):
        success, output = run_cpp_command(TOPIC2_EXE, ["print_tree"])
        if success:
            st.subheader("Tree JSON Structure")
            st.code(output, language='json')

            # Optional: Simple recursive visualizer using text if JSON is too complex for basic st
            # But prompt asked for st.code or st.text.
            # If we wanted to go fancy, we could use graphviz, but standard request is simple.
            try:
                json_data = json.loads(output)
                st.json(json_data)
            except:
                st.text("Could not parse JSON for pretty print, showing raw output:")
                st.text(output)
        else:
            st.error(output)

# --- Topic 3: Campus Navigation ---
elif topic == "Topic 3: Campus Navigation":
    st.header("Topic 3: Campus Navigation")

    # Load locations for dropdowns
    success, output = run_cpp_command(TOPIC3_EXE, ["locations"])

    if success:
        try:
            df_locs = pd.read_csv(io.StringIO(output))
            # Create a dictionary for mapping Name -> ID
            # Assuming CSV has columns: id,name,popularity,info

            # Helper to format option
            def format_func(row):
                return f"{row['name']} (ID: {row['id']})"

            # We need a list of names/labels for the selectbox, and a way to get ID back
            # Let's verify columns exist
            if 'name' in df_locs.columns and 'id' in df_locs.columns:
                options = df_locs.apply(format_func, axis=1).tolist()

                col1, col2 = st.columns(2)
                with col1:
                    start_option = st.selectbox("Start Location", options)
                with col2:
                    end_option = st.selectbox("End Location", options, index=min(1, len(options)-1))

                # Extract IDs
                # Assuming format "Name (ID: X)"
                start_id = int(start_option.split("ID: ")[1].replace(")", ""))
                end_id = int(end_option.split("ID: ")[1].replace(")", ""))

                if st.button("Find Shortest Path"):
                    p_success, p_output = run_cpp_command(TOPIC3_EXE, ["path", str(start_id), str(end_id)])
                    if p_success:
                        st.subheader("Shortest Path")
                        # C++ outputs CSV: Name1,Name2,Name3
                        # Let's make it look like a path
                        path_steps = p_output.split(',')
                        if len(path_steps) > 0 and path_steps[0]:
                            st.success(" -> ".join(path_steps))
                        else:
                            st.warning("No path found or invalid path.")
                    else:
                        st.error(p_output)
            else:
                 st.error("CSV format from C++ app unexpected. Missing 'id' or 'name' columns.")
                 st.dataframe(df_locs)

        except Exception as e:
            st.error(f"Error parsing location data: {e}")
            st.text(output)
    else:
        st.error("Failed to load locations from C++ backend.")
        st.error(output)

st.markdown("---")
st.caption("Backend powered by C++ Executables. Frontend by Streamlit.")
