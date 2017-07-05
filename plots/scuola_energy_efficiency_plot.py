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
        39.817 /39.817 /1,
        39.817 /25.303/2,
        39.817 /20.296/3,
        39.817 /15.702/4,
        39.817 /15.523/5,
        39.817 /17.538/6,
        39.817 /10.817/7,
        39.817 /11.078/8,
        39.817 /11.135/9,
        39.817 /10.072/10,
        39.817 /11.049/11,
        39.817 /10.776/12,
        39.817 /20.905/13,
        39.817 /8.587/14,
        39.817 /7.579/15,
        39.817 /17.946/16,
        39.817 /14.182/17,
        39.817 /28.497/18,
        39.817 /15.785/19,
        39.817 /26.459/20,
        39.817 /19.162/21,
        39.817 /17.522/22,
        39.817 /19.696/23
    ],
    'r--',

    t,
    [
        (17.962 + 39.817) /(17.962 + 39.817)/1,
        (17.962 + 39.817) /(17.174 + 25.303)/2,
        (17.962 + 39.817) /(16.562 + 20.296)/3,
        (17.962 + 39.817) /(14.838 + 15.702)/4,
        (17.962 + 39.817) /(18.421 + 15.523)/5,
        (17.962 + 39.817) /(15.592 + 17.538)/6,
        (17.962 + 39.817) /(16.32 + 10.817)/7,
        (17.962 + 39.817) /(18.591 + 11.078)/8,
        (17.962 + 39.817) /(19.268 + 11.135)/9,
        (17.962 + 39.817) /(16.04 + 10.072)/10,
        (17.962 + 39.817) /(14.787 + 11.049)/11,
        (17.962 + 39.817) /(20.577 + 10.776)/12,
        (17.962 + 39.817) /(19.737 + 20.905)/13,
        (17.962 + 39.817) /(19.153 + 8.587)/14,
        (17.962 + 39.817) /(17.573 + 7.579)/15,
        (17.962 + 39.817) /(20.995 + 17.946)/16,
        (17.962 + 39.817) /(16.208 + 14.182)/17,
        (17.962 + 39.817) /(24.437 + 28.497)/18,
        (17.962 + 39.817) /(19.082 + 15.785)/19,
        (17.962 + 39.817) /(15.471 + 26.459)/20,
        (17.962 + 39.817) /(19.599 + 19.162)/21,
        (17.962 + 39.817) /(22.181 + 17.522)/22,
        (17.962 + 39.817) /(21.433 + 19.696)/23
    ],
    'g--'
)
plt.show()
