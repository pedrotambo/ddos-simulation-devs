#!/bin/bash

# Default Python binary
default_python_path="python3"

# Check if a Python binary path is provided as an argument
if [ -z "$1" ]; then
    python_path="$default_python_path"
else
    python_path="$1"
fi

# Check if Python 3.7 binary exists
if [ ! -x "$(command -v "$python_path")" ]; then
    echo "Python 3.7 binary not found or not executable. Aborting."
    exit 1
fi

# Check Python version
python_version="$("$python_path" --version 2>&1 | awk '{print $2}')"
required_version_prefix="3.7"
if [[ "${python_version:0:3}" != "$required_version_prefix" ]]; then
    echo "Python version $python_version does not start with $required_version_prefix. Aborting."
    exit 1
fi

# Create virtual environment
"$python_path" -m venv venv

# Activate virtual environment
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

echo "Setup complete."
