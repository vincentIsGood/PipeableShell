import subprocess
from subprocess import PIPE, STDOUT

stdinStr = "a a a -f ls shele& -p sad -- l -l | &\nexit\n"
expectedOutput = """pshell: '&' should not appear in the middle of the command line
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