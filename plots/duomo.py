import matplotlib.pyplot as plt

# Image: duomo.jpg, cols: 2048 rows: 1536, 55 seams

t = [1, 2, 3, 4, 5,
     6, 7, 8, 9, 10,
     11, 12, 13, 14, 15,
      16, 17, 18, 19, 20,
     21, 22, 23
     ]

plt.plot(
    t,
    [
    1279.46,
    1194.38,
    1162.31,
    1145.24,
    1151.52,
    1138.14,
    1134.26,
    1128.2,
    1131.14,
    1111.3,
    1116.08,
    1116.7,
    1103.65,
    1115.18,
    1113.02,
    1118.78,
    1119.77,
    1114.32,
    1105.25,
    1111.92,
    1102.42,
    1107.47,
    1116.96,
    ],
    'g--',
    t,
    [
        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,

        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,

        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,

        1248.16,
        1248.16,
        1248.16,
        1248.16,
        1248.16,

    ],
    'b--',


)
plt.show()

