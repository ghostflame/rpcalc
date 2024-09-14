# Stack Tests

# SS - stack sym
10.000000 = 1 2 3 4 SS
-20.000000 = 1 2 3 4 -30 SS

# SP - stack product
24.000000 = 1 2 3 4 SP
-24.000000 = 1 2 3 -4 SP
0.000000 = 1 2 3 4 00 SP
120.000000 = 0x1 0x2 0x3 0x4 0x5 SP

# SG - geometric mean
1.817121 = 1 2 3 SG

# SQ - quadratic mean
4.320494 = 2 4 6 SQ

# SM - normal arithmetic mean
3.000000 = 1 2 3 4 5 SM
10.000000 = 10 10 0xa 012 0b01010 SM
3.000000 = 3 SM
3.000000 = 3 3 3 3 3 3 3 SM
3.000000 = 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 SM

# SE - median
3.000000 = 1 2 3 4 5 SE
1.000000 = 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 7 7 7 10 10000000 SE
0.000000 = -20 -10 0 10 20 SE

# SO - modal value
1.000000 = 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 7 7 7 10 10000000 SO
3.000000 = 1 1 2 2 3 3 3 -100 0xa 2.298e29 SO
3.000000 = 1 3 2 3 4 3 5 3 6 3 7 7 7 7 3 3 21 3 SO

# SD - Standard Deviation
0.000000 = 5 5 5 5 5 5 5 5 5 SD
5.000000 = 20 30 20 20 30 30 20 30 20 30 SD

# SU - pick out the highest
4.000000 = 1 2 3 4 SU
2.298e+29 = 1 1 2 2 3 3 3 -100 0xa 2.298e29 SU oZ

# SL - pick out the lowest
1.000000 = 1 2 3 4 SL

# SB - span
4 = 1 2 oI 3 4 5 SB
9.500000 = -3 -0.5 6.5 4 4 4 4 4 4 4 4 4 4 4 2 2 2 2 2 2 2 2 2 2 SB

# SX - unique the stack
10 = 1 1 1 1 1 1 2 2 2 2 2 2 3 3 3 3 3 4 4 4 4 4 SX SS oI
10 = 0x1 0b1 01 0o1 1 2 0x2 02 0x3 0x3 03 03 0b11 0b11 2 2 + 2 2 + 2 2 + 4 4 0b100 SX SS oI

# SC = count the stack
5 = 1 2 1 2 -200000 SC oI
4 = 1 1 1 1 1 2 2 2 2 3 3 3 3 4 4 4 4 SX SC oI

# Sg - gcd of stack
2.000000 = 4 6 8 14 3232 0b101011010 Sg

# Sl - least common multiple
24.000000 = 4 6 8 4 6 8 4 6 8 4 6 8 2 3 12 Sl

