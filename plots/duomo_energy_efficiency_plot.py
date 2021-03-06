import matplotlib.pyplot as plt

# Image: duomo.jpg, cols: 2048 rows: 1536, 55 seams

t = [1, 2, 3, 4, 5,
     6, 7, 8, 9, 10,
     11, 12, 13, 14, 15,
      16, 17, 18, 19, 20,
     21, 22, 23
     ]

plt.plot(
    # t,
    # [
    #     1,
    #     199.474 / 105.908 / 2,
    #     199.474 / 75.45 / 3,
    #     199.474 / 60.979 / 4,
    #     199.474 / 49.981 / 5,
    #
    #     199.474 / 43.581 / 6,
    #     199.474 / 39.154 / 7,
    #     199.474 / 38.408 / 8,
    #     199.474 / 31.983 / 9,
    #     199.474 / 28.544 / 10,
    #
    #     199.474 / 27.375 / 11,
    #     199.474 / 25.564 / 12,
    #     199.474 / 28.911 / 13,
    #     199.474 / 23.496 / 14,
    #     199.474 / 22.76 / 15,
    #
    #     199.474 / 29.785 / 16,
    #     199.474 / 23.35 / 17,
    #     199.474 / 21.92 / 18,
    #     199.474 / 25.48 / 19,
    #     199.474 / 19.529 / 20,
    #
    #     199.474 / 17.98 / 21,
    #     199.474 / 23.206 / 22,
    #     199.474 / 22.923 / 23
    # ],
    # 'r--',

    t,
    [
        1 ,
        (17.06 + 199.474) / (16.719 + 105.908) / 2,
        (17.06 + 199.474) /  (17.648 + 75.45) / 3,
        (17.06 + 199.474) / (17.284 + 60.979) / 4,
        (17.06 + 199.474) / (23.191 + 49.981) / 5,

        (17.06 + 199.474) / (21.34 + 43.581) / 6,
        (17.06 + 199.474) / (20.184 + 39.154) / 7,
        (17.06 + 199.474) / (20.444 + 38.408) / 8,
        (17.06 + 199.474) / (18.982 + 31.983) / 9,
        (17.06 + 199.474) / (19.437 + 28.544) / 10,

        (17.06 + 199.474) / (18.07 + 27.375) / 11,
        (17.06 + 199.474) / (21.385 + 25.564) / 12,
        (17.06 + 199.474) / (20.099 + 28.911) / 13,
        (17.06 + 199.474) / (21.36 + 23.496) / 14,
        (17.06 + 199.474) / (21.4 + 22.76) / 15,

        (17.06 + 199.474) / (17.943 + 29.785) / 16,
        (17.06 + 199.474) / (22.979 + 23.35) / 17,
        (17.06 + 199.474) / (19.236 + 21.92) / 18,
        (17.06 + 199.474) / (18.187 + 25.48) / 19,
        (17.06 + 199.474) / (21.446 + 19.529) / 20,

        (17.06 + 199.474) / (18.034 + 17.98) / 21,
        (17.06 + 199.474) / (21.282 + 23.206) / 22,
        (17.06 + 199.474) / (21.712 + 22.923) / 23
    ],
    'g--'
)
plt.show()

