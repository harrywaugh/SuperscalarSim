import glob
import os 
import subprocess

def main():
    path = os.path.dirname(os.path.realpath(__file__))[:-3]
    print("Current directory: " + path)
    tests = glob.glob("programs/*.asm")
    print("Tests Found: ", tests, "\n")

    correct_tests = 0

    for test in tests:
        short_testname = test.split('/')[-1]
        p = subprocess.Popen([path + "simulator.exe", test], stdout=subprocess.PIPE)

        guess = int(p.communicate()[0].decode("utf-8"))

        result_fname = short_testname[:-3] + "result"
        file = open(path + "programs/" + result_fname,"r")
        result = int(file.read())

        result_str = ("(" + str(guess) + " == " + str(result) + ") PASSED\n") if (result==guess) \
                                                                              else ("(" + str(guess) + " != " + str(result) + ") FAILED\n")

        print("Test " + short_testname + ": \n\t" + result_str)

        if result==guess:
            correct_tests+=1

    print("Tests passed: ", correct_tests, "/", len(tests))

if __name__ == "__main__":
    main()