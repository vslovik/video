import matplotlib.pyplot as plt

# Image: duomo.jpg, cols: 2048 rows: 1536, 55 seams

t = [1, 2, 3, 4, 5, 6]

plt.plot(
    t,
    [
        433.434,
        422.38,
        401.374,
        418.416,
        397.998,
        407.415
    ],
    'r--',
    t,
    [
        484.675,
        484.675,
        484.675,
        484.675,
        484.675,
        484.675,
    ],
    'g--',


)
plt.show()

