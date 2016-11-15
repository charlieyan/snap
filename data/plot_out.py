#!/usr/bin/env python
import glob, os
import csv
import matplotlib.pyplot as plt
import sys

def get_files_with_pattern(pattern):
  for file in glob.glob(pattern):
    return file

if len(sys.argv) < 2:
  print "missing dataset"
  sys.exit(0)
dataset = sys.argv[1]

sda_times = []
sda_voltages = []

sda_file_name = './'+dataset+'/F*CH1.CSV'
sda_file_name = get_files_with_pattern(sda_file_name)

with open(sda_file_name, 'rb') as csvfile:
  reader = csv.reader(csvfile, delimiter=',')
  for row in reader:
    sda_times.append(float(row[3]))
    sda_voltages.append(float(row[4]))

scl_times = []
scl_voltages = []

scl_file_name = './'+dataset+'/F*CH3.CSV'
scl_file_name = get_files_with_pattern(scl_file_name)

with open(scl_file_name, 'rb') as csvfile:
  reader = csv.reader(csvfile, delimiter=',')
  for row in reader:
    scl_times.append(float(row[3]))
    scl_voltages.append(float(row[4]))

plt.figure(1)

ax1 = plt.subplot(211)
plt.plot(sda_times, sda_voltages, '-')

ax2 = plt.subplot(212, sharex=ax1)
plt.plot(scl_times, scl_voltages, '-')

plt.show()