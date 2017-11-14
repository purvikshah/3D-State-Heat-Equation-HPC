#!/usr/bin/env python
import subprocess

eps = 0.01
init = 100

cores = [1, 2, 4, 6, 8 ,12]
time_serial=[]
size=[]
iter_serial = []
print "This is Serial Execution"
for i in range(1,6):
	
	size.append(init)			
	print 'Size is',init
	subprocess.call(["gcc","-fopenmp","heated_plate.c","-lm"])	
	cmd = ["./a.out",str(init),str(eps)]
	output = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]
	print output
	iter_serial.append(float(output.split(' ')[0]))
	time_serial.append(float(output.split(' ')[1]))

	init = init * 2


time_parallel=[]
badi_list=[]
iter_parallel=[]
init = 100

print "This is Parallel Execution"
for i in range(1,6):			
	print 'Size is',init
	for j in range(0,6):
		print "Number of threads are "+str(cores[j])
		subprocess.call(["gcc","-fopenmp","heated_plate_p.c","-lm"])	
		cmd = ["./a.out",str(init),str(eps),str(cores[j])]
		output = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]
		print output
		badi_list.append(float(output.split(' ')[1]))
		if(j>4):
			iter_parallel.append(float(output.split(' ')[0]))
			time_parallel.append(float(output.split(' ')[1]))

	init = init *2
			
print "Size = "	
print size
print "Iter_serial = "
print iter_serial
print "Iter_parallel = "
print iter_parallel
print "Time_serial = "
print time_serial
print "Time_parallel_12 = "
print time_parallel
speedup =[]
for i in range(0,5):
	speedup.append(time_serial[i]/time_parallel[i])

print "Speedup12 = "
print speedup

overhead=[]
speedup2=[]
speedup4=[]
speedup6=[]
speedup8 = []
speedup12 = []
eff2=[]
eff4=[]
eff6=[]
eff8 = []
eff12 = []

for j in range(0,5):
	overhead.append(time_serial[j]-badi_list[j*6])
	speedup2.append(time_serial[j]/badi_list[j*6 + 1])
	eff2.append(speedup2[j]/2)
	speedup4.append(time_serial[j]/badi_list[j*6 + 2])
	eff4.append(speedup4[j]/4)
	speedup6.append(time_serial[j]/badi_list[j*6 + 3])
	eff6.append(speedup6[j]/6)
	speedup8.append(time_serial[j]/badi_list[j*6 + 4])
	eff8.append(speedup8[j]/8)
	speedup12.append(time_serial[j]/badi_list[j*6 + 5])
	eff12.append(speedup12[j]/12)



print "Overhead"
print overhead
print "Speedup of different threads"
print 'Speedup2 = '
print speedup2
print 'Speedup4 = '
print speedup4
print 'Speedup6 = '
print speedup6
print 'Speedup8 = '
print speedup8
print 'Speedup12 = '
print speedup12

print "Efficiency of different threads"
print 'Eff2 = '
print eff2
print 'Eff4 = '
print eff4
print 'Eff6 = '
print eff6
print 'Eff8 = '
print eff8
print 'Eff12 = '
print eff12
