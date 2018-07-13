#!/bin/sh

basedir="/pnfs/nova/scratch/users/${USER}/test_flux"

#####
njobs="--n_jobs=1"
pot="--pot=5"

outdirs=( "dir1" "dir2" )

#tgt_opt_config=( "--number_of_fins=10 --distance_between_fins=0.25 --target_position_Z=-143.3" "--number_of_fins=20 --distance_between_fins=0.35 --target_position_Z=-163.3" )

tgt_opt_config=( "--number_of_fins=48 --target_position_Z=-143.3" "--number_of_fins=46 --target_position_Z=-138.4" )

####
exe=ProcessG4NuMI.py
bspot="--beam_spotsize_X=1.5 --beam_spotsize_Y=1.5"
bmode="--beamconfig=me000z-200i"

len=${#tgt_opt_config[*]} 

echo "len $len"

for (( i=0; i<len; i++ )) 
do
    outdir="${basedir}/${outdirs[${i}]}"
    mkdir -p ${outdir}
    outdir="--outdir=${outdir}"
    echo "====================================="
    echo "==>> Sendeiung this commend to the GRID:"
    echo "python ${exe} ${outdir} ${njobs} ${pot} ${bspot} ${bmode} ${tgt_opt_config[${i}]}"
    echo ""
    echo "====================================="
 python ${exe} ${outdir} ${njobs} ${pot} ${bspot} ${bmode} ${tgt_opt_config[${i}]}

done
 

