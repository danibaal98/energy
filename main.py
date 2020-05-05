#!/usr/bin/python3

import csv
import matplotlib.pyplot as plt
from datetime import datetime

path = "results/initial_assignment-2.csv"
file = open(path, newline='')
reader = csv.reader(file)

header = next(reader)

data = []
for row in reader:
    slot = int(row[0])
    date = datetime.strptime(row[1], '%d-%m-%Y:%H:%M:%S')
    plan = int(row[2])
    QoS = float(row[3])
    battery = float(row[4])
    consumption = float(row[5])
    ep_slot = float(row[6])

    data.append([slot, date, plan, QoS, battery, consumption, ep_slot])

battery_values = []
plan_values = []

for d in data:
    plan_values.append(d[2])
    battery_values.append(d[4])

battery_values = battery_values[-300:]
plan_values = plan_values[-300:]

plt.figure()
plt.subplot(211)
plt.plot(list(range(1, 301)), battery_values, 'b--')
plt.ylabel('Battery level')
plt.title('Battery level per slot')

plt.subplot(212)
plt.plot(list(range(1, 301)), plan_values, 'r--')
plt.xlabel('Slots')
plt.ylabel('Plans')
plt.title('Plans per slot')
plt.show()