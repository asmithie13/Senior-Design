import numpy as np
from gameBoard import *
from gamePiece import *

def processMove(gb, visArr):
    s = input("Enter move coordinates in #### form: ")
    nums = np.array([["0", "1", "2", "3", "4", "5", "6", "7"]])
    move = Move([int(s[0]), int(s[1])], [int(s[2]), int(s[3])])
    result = gb.validateMove(move)

    for i in range(8):
        for j in range(8):
            if gb.tiles[i][j] is not None:
                if gb.tiles[i][j].player == Player.BLACK:
                    visArr[i][j] = "B"
                else:
                    visArr[i][j] = "R"
            else:
                visArr[i][j] = " "


    print(result)
    print(nums)
    print(np.hstack((visArr, nums.T)))
    print("Current Player: " + str(gb.currentPlayer))

gb = GameBoard()

visArr = [[" "] * 8 for _ in range(8)]
 
for i in range(8):
    for j in range(8):
        if gb.tiles[i][j] is not None:
            if gb.tiles[i][j].player == Player.BLACK:
                visArr[i][j] = "B"
            else:
                visArr[i][j] = "R"

nums = np.array([["0", "1", "2", "3", "4", "5", "6", "7"]])
visArr = np.array(visArr)
print(nums)
print(np.hstack((visArr, nums.T)))

while(True):
    processMove(gb, visArr)