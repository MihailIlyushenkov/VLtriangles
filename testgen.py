import random
import sys

#point [x, y, z]
#line [r:point, a:point]

#generates random point in cube with side of rad
count = 5

with open("tests/generated.txt", "w") as f:
    f.write(str(count*2) + "\n")
    for i in range(count):
        f.write("0 ")
    f.write('\n')
    for j in range(count):
        for i in range(6):
            f.write(f"{random.randint(-10, 10)} {random.randint(-10, 10)} {random.randint(-10, 10)}\n")


sys.exit()

def get_point(center, rad):
    p = []
    for i in range(3):
        p.append(center + random.randint(0, rad*1000)/1000)
    return p

def get_line(point: list):
    if point == []:
        return [get_point(0, 10), get_point(0, 1)]
    else: 
        return [[i for i in point], get_point(0, 1)]

def get_lines_intersected(base_point = []):
    if base_point == []:
        base_point = get_point(random.randint(0, 10), 10) 
    
    line1 = get_line(base_point)
    line2 = get_line(base_point)

    c1 = random.randint(0, 10)
    c2 = random.randint(0, 10)

    for i in range(3):
        line1[0][i] += c1*line1[1][i]
        line2[0][i] += c2*line2[1][i]

        line1[0][i] = (line1[0][i]//0.001)/1000
        line2[0][i] = (line2[0][i]//0.001)/1000

    return [line1, line2]


def printline(line: list):
    for i in line:
        print(i)
    print('\n')


mypoint = [1, 1, 1]

line1, line2 = get_lines_intersected(mypoint)

print(line1)

printline(line1)
printline(line2)