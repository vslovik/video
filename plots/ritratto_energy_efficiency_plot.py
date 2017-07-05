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
    #     57.757 / 57.757/1,
    #     57.757 / 38.081/2,
    #     57.757 / 27.977/3,
    #     57.757 / 25.471/4,
    #     57.757 / 19.962/5,
    #     57.757 / 18.579/6,
    #     57.757 / 17.582/7,
    #     57.757 / 19.526/8,
    #     57.757 / 10.015/9,
    #     57.757 / 13.673/10,
    #     57.757 / 24.241/11,
    #     57.757 / 18.682/12,
    #     57.757 / 22.035/13,
    #     57.757 / 7.53/14,
    #     57.757 / 11.053/15,
    #     57.757 / 13.57/16,
    #     57.757 / 10.453/17,
    #     57.757 / 35.629/18,
    #     57.757 / 29.241/19,
    #     57.757 / 8.736/20,
    #     57.757 / 9.046/21,
    #     57.757 / 9.375/22,
    #     57.757 / 8.789/23
    # ],
    # 'r--',

    t,
    [
        (17.448+ 57.757)/(17.448+ 57.757)/1,
        (17.448+ 57.757)/(17.622+38.081)/2,
        (17.448+ 57.757)/(16.29+27.977)/3,
        (17.448+ 57.757)/(15.335+25.471)/4,
        (17.448+ 57.757)/(17.927+19.962)/5,
        (17.448+ 57.757)/(18.581+18.579)/6,
        (17.448+ 57.757)/(16.884+17.582)/7,
        (17.448+ 57.757)/(16.644+19.526)/8,
        (17.448+ 57.757)/(20.639+10.015)/9,
        (17.448+ 57.757)/(24.304+13.673)/10,
        (17.448+ 57.757)/(21.282+24.241)/11,
        (17.448+ 57.757)/(17.302+18.682)/12,
        (17.448+ 57.757)/(17.038+22.035)/13,
        (17.448+ 57.757)/(20.42+7.53)/14,
        (17.448+ 57.757)/(17.833+11.053)/15,
        (17.448+ 57.757)/(20.151+13.57)/16,
        (17.448+ 57.757)/(21.614+10.453)/17,
        (17.448+ 57.757)/(21.781+35.629)/18,
        (17.448+ 57.757)/(17.646+29.241)/19,
        (17.448+ 57.757)/(23.594+8.736)/20,
        (17.448+ 57.757)/(23.589+9.046)/21,
        (17.448+ 57.757)/(24.376+9.375)/22,
        (17.448+ 57.757)/(22.9+8.789)/23
    ],
    'g--'
)
plt.show()

