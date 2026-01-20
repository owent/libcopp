#!/bin/bash

SCRIPT_DIR="$(dirname "$0")";
SCRIPT_DIR="$(cd "$SCRIPT_DIR" && pwd)";

# PATH for editor ${workspaceFolder}/py3env/bin

if [[ -n "$WINDIR" ]] && [[ "x${OS:0:7}" == "xWindows" ]]; then
    PYTHON_BIN="python"
else
    PYTHON_BIN="python3"
fi

$PYTHON_BIN -m pip install --user --upgrade pip
$PYTHON_BIN -m pip install --user --upgrade -r "$SCRIPT_DIR/requirements-vscode.txt" ;
$PYTHON_BIN -m pip install --user --upgrade -r "$SCRIPT_DIR/requirements.txt" ;
$PYTHON_BIN -m mkdocs --version


