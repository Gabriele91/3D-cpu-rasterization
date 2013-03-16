import sys
name=raw_input("Name files:")
nameupper=name.upper()

#make h file
ofile = open(name+".h","w")
ofile.write("#ifndef "+nameupper+"_H\n")
ofile.write("#define "+nameupper+"_H\n\n")
ofile.write("#endif")
ofile.close()
#make cppfile
ofile = open(name+".cpp","w")
ofile.write("#include \""+name+".h\"\n\n")
ofile.close()