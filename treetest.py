count = 40
lenght = 1 #smallest cube side size

cubes = [[[[] for i in range(count*2)] for j in range(count*2)] for k in range(count*2)]
dots = [[[[] for i in range(count*2)] for j in range(count*2)] for k in range(count*2)]


# for i in range(count*2):
#     for j in range(count*2):
#         print(cubes[i][j])

for i in range(count*2-1, -1, -1):
    for j in range(count*2-1, -1, -1):
        for k in range(count*2 - 1, -1, -1):
            print(i-count+1, j-count+1, k-count+1)
            cubes[i][j][k] = [(i-count+1)*lenght, (j-count+1)*lenght, (k-count+1)*lenght]

# for i in range(count*2):
#     for j in range(count*2):
#         for k in range(count*2):
#             print(f"{i, j, k} {cubes[i][j][k]}")
#         print('\n')



for i in range(count*2):
    for j in range(count*2):
        for k in range(count*2):
            dots[i][j][k] = [cubes[i][j][k][0] - lenght/2, cubes[i][j][k][1] - lenght/2, cubes[i][j][k][2] - lenght/2] 


for i in range(count*2):
    for j in range(count*2):
        for k in range(count*2):
            print(f"{i, j, k} {dots[i][j][k]}")
        print('\n')

with open("tests/treetest_generated.txt", "w") as f:
    f.write(f"{8*(count**3)}\n")
    for x in range(count*2):
        for y in range(count*2):
            for z in range(count*2):
                    elem = dots[x][y][z]
                    f.write(f"{elem[0]} {elem[1]} {elem[2]}\n")
                    f.write(f"{elem[0]} {elem[1]} {elem[2]}\n")
                    f.write(f"{elem[0]} {elem[1]} {elem[2]}\n")
                    f.write('\n')
