import subprocess
from subprocess import PIPE, STDOUT

stdinStr = "ls -l -a\nexit\n"
expectedOutput = """test_onecmd.py
test_parse.py
test_pipes.py
test_timeit.py
pshell: Terminated
"""

print("[py] Testing with stdin: '%s'" % stdinStr)

proc = subprocess.run(["../bin/pshell"], input=stdinStr, stdout=PIPE, stderr=STDOUT, encoding="ascii")

def matchesAllTerms(terms):
    matchesAllTerms = True
    for term in terms:
        matchesAllTerms &= term in proc.stdout
    return matchesAllTerms

if expectedOutput in proc.stdout or matchesAllTerms(expectedOutput.split("\n")):
    print("[py] Test passed")
else: 
    print("[py] --------- expected ---------")
    print(expectedOutput)
    print("[py] --------- but got ----------")
    print(proc.stdout)
    print("[py] ============================")
    print("[py] Test failed")

print("[py] Program done with:");
print("[py] Return code: %d" % (proc.returncode))
print("\n")