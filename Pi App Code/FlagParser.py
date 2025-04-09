import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
    arr = []
    with open('Pi App Code\log.txt') as file:
        for line in file:
            words = line.split()
            arr.append(words)
    arr = np.array(arr)
    boolArr = arr[:, 0]
    trueCount = sum(1 if x == "True"  else 0 for x in boolArr)
    print("Players pick a valid move " + str(trueCount / len(boolArr) * 100) + "% of the time.")

    arrFalse = []
    for row in arr:
        if row[0] == "False":
            arrFalse.append(row[1])

    plt.hist(arrFalse)
    plt.title("Common Failure Conditions")
    plt.xlabel("Failure Enums")
    plt.ylabel("Average Counts")
    plt.show()