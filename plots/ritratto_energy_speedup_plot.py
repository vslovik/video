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
    #     54.294 / 57.757,
    #     54.294 / 38.081,
    #     54.294 / 27.977,
    #     54.294 / 25.471,
    #     54.294 / 19.962,
    #     54.294 / 18.579,
    #     54.294 / 17.582,
    #     54.294 / 19.526,
    #     54.294 / 10.015,
    #     54.294 / 13.673,
    #     54.294 / 24.241,
    #     54.294 / 18.682,
    #     54.294 / 22.035,
    #     54.294 / 7.53,
    #     54.294 / 11.053,
    #     54.294 / 13.57,
    #     54.294 / 10.453,
    #     54.294 / 35.629,
    #     54.294 / 29.241,
    #     54.294 / 8.736,
    #     54.294 / 9.046,
    #     54.294 / 9.375,
    #     54.294 / 8.789
    # ],
    # 'r--',

    t,
    [
        54.294 /(17.448+ 57.757),
        54.294 /(17.622+38.081),
        54.294 /(16.29+27.977),
        54.294 /(15.335+25.471),
        54.294 /(17.927+19.962),
        54.294 /(18.581+18.579),
        54.294 /(16.884+17.582),
        54.294 /(16.644+19.526),
        54.294 /(20.639+10.015),
        54.294 /(24.304+13.673),
        54.294 /(21.282+24.241),
        54.294 /(17.302+18.682),
        54.294 /(17.038+22.035),
        54.294 /(20.42+7.53),
        54.294 /(17.833+11.053),
        54.294 /(20.151+13.57),
        54.294 /(21.614+10.453),
        54.294 /(21.781+35.629),
        54.294 /(17.646+29.241),
        54.294 /(23.594+8.736),
        54.294 /(23.589+9.046),
        54.294 /(24.376+9.375),
        54.294 /(22.9+8.789)
    ],
    'g--',

    t,
    [
        1, 2, 3, 4, 5,
        6, 7, 8, 9, 10,
        11, 12, 13, 14, 15,
        16, 17, 18, 19, 20,
         21, 22, 23
    ],
    'b--',
)
plt.show()
