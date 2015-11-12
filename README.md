# caicos

This tool is designed to build hardware accelerators, suitable for use in Xilinx Vivado HLS, 
from C code that has been produced by Jamaica Builder from aicas.

This work is part of the EU Seventh Framework project JUNIPER (http://www.juniper-project.org/).

##Usage
caicos is invoked by running the following:

	python caicos.py <configfile>

The format of the config file is described in caicos.py. An example is shown below:

	outputdir = /Users/me/caicosout
	jamaicaoutputdir = /Users/me/JamaicaGeneratedCode
	fpgapart = xc7vx690tffg1761-2
	jamaicatarget = /Users/me/jamaica/current32/target/linux-x86/include
	signature = dataProcessing/SumPrimitiveArrays.run()V
	signature = dataProcessing/SumClass.run()V
	signature = unitTests/Types.types(IFDLjava/lang/Float;Ljava/lang/Double;)D

For a more complete description of caicos and how it is operated, please see JUNIPER deliverable D2.5, which will be made public in the first quarter of 2016.
