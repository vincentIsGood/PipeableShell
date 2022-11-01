#!/usr/bin/bash
cd tests

python3 test_parse.py
python3 test_onecmd.py
python3 test_pipes.py
python3 test_timeit.py

cd ..