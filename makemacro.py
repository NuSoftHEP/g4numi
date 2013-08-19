#!/usr/bin/env python
import os, re, sys, getopt,string
from string import Template

def usage():
    
    print """
NAME
    makemacro.py - makes g4numi.mac control files from a template
SYNOPSIS
    makemacro.py [OPTIONS]
DESCRIPTION
    Reads 
    %s/template.mac 
    and writes a new .mac file to standard output, 
    substituting some ${flags} according to defaults or the command line.
    -h, --help
          this message 
    -b, --beamconfig [default=LE010z185i]
          Configure the neutrino beam.   
          This sets the TargetZ0 and Baffle Z0 and HornCurrent.
          Must be in the form 'LE#z#i' or 'le#z#i', where # is any number. 
          Examples are le010z185i, LE025.3z-200i, LE250z185.6i....etc.
    -o, --outfile [default=g4numi$G4NUMIVER_<beamconfig>]
          Directory and filename (without the .root) to write output ntuple
          the run number will be appended to the filename 
    -r, --run [default=1]
          The run number
    -p, --pot [default=500000]
          The number of protons on target to simulate
    -s, --seed [default=run]
          The random seed used in the g4numi run. By default this will
          be set equal to the run number
    -n, --nametag [default='']
          If not '', '_<nametag>' will be appended to the filename
          g4numi$G4NUMIVER_<beamconfig>_<nametag>
          useful for recording a sort of 'subrun'
    -w, --dowater [default=false]
          simulate water in the target  
    -L, --watercm [default=3]
          cm of water in the target 

"""%(os.environ['BEAMSIM']+"/macros/template.mac")

def main(argv=None):
    if argv is None:
        argv = sys.argv
        
    dowater='false'    
    watercm='3'
    beamconfig='le010z185i'
    outfile=''
    seed=''
    nametag=''
    run='1'
    pot='500000'    
    templatefile=os.environ['BEAMSIM']+"/macros/template.mac"

    try:
        opts, args = getopt.getopt(argv[1:], "hwL:b:o:s:r:p:t:n:", 
                                   ["help","dowater","watercm",
                                    "beamconfig","outfile","seed","run","pot","template","nametag"])
    except getopt.error, msg:
        raise Usage(msg)
    # more code, unchanged
    for o,a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o in ("-w", "--dowater"):
            dowater='true'
        if o in ("-L","--watercm"):
            watercm=a
        if o in ("-b", "--beamconfig"):
            beamconfig=a
        if o in ("-o", "--output"):
            outfile = a
            outfile.replace('.root','')
        if o in ("-s","--seed"):
            seed=a
        if o in ("-r","--run"):
            run=a
        if o in ("-p","--pot"):
            pot=a
        if o in ("-t","--template"):
            templatefile=a
        if o in ("-n","--nametag"):
            nametag=a

# set seed to run if still null
    if len(seed)==0: 
        seed=run    
    if len(outfile)==0:
        outfile='g4numi_%s_%s'%(os.environ['G4NUMIVER'],beamconfig)
        if len(nametag)>0:
            outfile=outfile+'_'+nametag
    
    filestring=open(templatefile,'r').read()
    t=string.Template(filestring)
    print t.substitute({'dowater':dowater,'watercm':watercm,'beamconfig':beamconfig,'outfile':outfile,'seed':seed,'run':run,'pot':pot})


if __name__ == "__main__":
    sys.exit(main())
