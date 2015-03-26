# caicos

This tool is designed to build hardware accelerators, suitable for use in Xilinx Vivado HLS, 
from C code that has been produced by Jamaica Builder from aicas.

This work is internal to the EU Seventh Framework project JUNIPER and should not be 
externally distributed until cleared by project review, whereupon it will become GPLv2.

##Usage

The entry function is `prepareproject.build_from_functions()`. Example use follows:

    #Path to the output C code from Jamaica Builder
    jamaicaoutputdir = "/path/to/c/code/from/jamaicabuilder"
    
    #The target directory. This will not be cleared, but files may be overwritten.
    outputdir = "/path/to/targetdirectory"
    
    fpgapartcode = "xc7vx690tffg1761-2"
    
    #A list of Java method signatures in the original Java to roll into the created accelerator
    funcs = [
    	"unitTests/Types.types(IFDLjava/lang/Float;Ljava/lang/Double;)D",
    	"dataProcessing/SumPrimitiveArrays.sumArrayInt([I)I"
    ]
    
    #Absolute paths to any additional C files that will be required by HLS can be specified here.
    additionalsourcefiles = []
    
    prepareproject.build_from_functions(funcs, jamaicaoutputdir, outputdir, additionalsourcefiles, fpgapartcode)
