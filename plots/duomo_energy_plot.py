import matplotlib.pyplot as plt

# Image: duomo.jpg, cols: 2048 rows: 1536

t = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]

plt.plot(
    t,
    [
    17.06, # spinup
    16.719,
    17.648,
    17.284,
    23.191,

    21.34,
    20.184,
    20.444,
    18.982,
    19.437,

    18.07,
    21.385,
    20.099,
    21.36,
    21.4,

    17.943,
    22.979,
    19.236,
    18.187,
    21.446,

    18.034,
    21.282,
    21.712
    ],
    'r--',

    t,
    [
    17.06 + 199.474, # energy
    16.719 + 105.908,
    17.648 + 75.45,
    17.284 + 60.979,
    23.191 + 49.981,

    21.34 + 43.581,
    20.184 + 39.154,
    20.444 + 38.408,
    18.982 + 31.983,
    19.437 + 28.544,

    18.07 + 27.375,
    21.385 + 25.564,
    20.099 + 28.911,
    21.36 + 23.496,
    21.4 + 22.76,

    17.943 + 29.785,
    22.979 + 23.35,
    19.236 + 21.92,
    18.187 + 25.48,
    21.446 + 19.529,

    18.034 + 17.98,
    21.282 + 23.206,
    21.712 + 22.923
    ],
    'g--',

    t,
    [
    187.215, # seq
    187.215,
    187.215,
    187.215,
    187.215,

    187.215,
    187.215,
    187.215,
    187.215,
    187.215,

    187.215,
    187.215,
    187.215,
    187.215,
    187.215,

    187.215,
    187.215,
    187.215,
    187.215,
    187.215,

    187.215,
    187.215,
    187.215
    ],
    'b--',
)
plt.show()

