# -*- coding: utf-8 -*-
"""
Created on Tue Mar 19 14:42:17 2024

@author: ellix
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np 
import re

df = pd.read_excel(r'C:\Users\ellix\Downloads\Book2.xlsx')
df2 = pd.read_excel(r'C:\Users\ellix\Downloads\Book2.xlsx', "Sheet2")

df3 = pd.read_excel(r'C:\Users\ellix\Downloads\Book2.xlsx', "Sheet3")
df4 = pd.read_excel(r'C:\Users\ellix\Downloads\Book2.xlsx', "Sheet4")

sheet1 = np.array(df)
sheet2 = np.array(df2)
sheet3 = np.array(df3)
sheet4 = np.array(df4)
down = []
down2 = []
up = []
up2 = []

for l in sheet1: 
    down.append (float(re.findall(r'\b\d+\b', str(l[0]))[0]))
for l in sheet2: 
    up.append (float(re.findall(r'\b\d+\b', str(l[0]))[0]))
for l in sheet3: 
    down2.append (float(re.findall(r'\b\d+\b', str(l[0]))[0]))
for l in sheet4: 
    up2.append (float(re.findall(r'\b\d+\b', str(l[0]))[0]))

my_dict = {'down': down, 'up': up, 'down2': down2, 'up2': up2}
plt.boxplot(my_dict.values(), labels=my_dict.keys());
