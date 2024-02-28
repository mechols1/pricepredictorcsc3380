#!/bin/bash

# Navigate to the project directory
cd /Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3

# Create Python environment
mkdir -p env
conda create --prefix ./env python=3.10 -y
source activate ./env
conda install tensorflow pandas numpy plotly -y

# Verify pip path
pip_path=$(which pip)
expected_path="/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/env/bin/pip"

if [ "$pip_path" != "$expected_path" ]; then
    export PATH="/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/env/bin:$PATH"
fi

# Install dependencies using pip
pip install alpaca_trade_api nltk yfinance

# Install qt
brew install qt

# Run project.cpp
mkdir -p build
cd build
cmake .. && make
./project
