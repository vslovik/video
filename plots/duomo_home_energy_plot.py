import matplotlib.pyplot as plt

# Image: duomo.jpg, cols: 2048 rows: 1536

t = [1, 2, 3, 4, 5, 6]

plt.plot(
    t,
    [
        4.418,
        13.455,
        4.432,
        10.484,
        18.294,
        13.624
    ],
    'r--',

    t,
    [
        4.418+64.133,
        13.455+39.621,
        4.432+25.082,
        10.484+23.949,
        18.294+23.284,
        13.624+19.782
    ],
    'g--',

    t,
    [
    56.81, # seq
    56.81,
    56.81,
    56.81,
    56.81,
    56.81,
    ],
    'b--',
)
plt.show()

