$PSDefaultParameterValues['*:Encoding'] = 'UTF-8'

$OutputEncoding = [System.Text.UTF8Encoding]::new()

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition

Set-Location $SCRIPT_DIR

if ($IsWindows) {
    $PYTHON_BIN = "python"
}
else {
    $PYTHON_BIN = "python3"
}

& $PYTHON_BIN -m pip install --user --upgrade pip
& $PYTHON_BIN -m pip install --user --upgrade -r "$SCRIPT_DIR/requirements-vscode.txt" ;
& $PYTHON_BIN -m pip install --user --upgrade -r "$SCRIPT_DIR/requirements.txt" ;
