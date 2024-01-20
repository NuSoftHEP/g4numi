#!/usr/bin/perl
#
########################################################################
########################################################################

use File::Path;
use File::Basename;
use Cwd;

########################################################################
#
# Retrieve command line arguments:
#
########################################################################

sub show_basic{
  print STDOUT "==============================================================================================\n";
  print STDOUT "./process_na49Grid.pl [Momentum] [firstRun] [lastRun]  [Optional]\n";
  print STDOUT "                     <required> \n";
  print STDOUT "REQUIRED OPTIONS:\n\n";
  print STDOUT "   firstRun               : fisrt run number to process \n";
  print STDOUT "   lastRun               : last run number to process \n";
}

sub show_usage{
  &show_basic;
  print STDOUT "GENERAL OPTIONS:\n";
  print STDOUT "    -------------------------------------------------------------------------\n";
  print STDOUT "   (-p) Had Physics : NOT WORKING. MUST EDIT BY HAND. Hadronic physics to use (QGSP/FTFP_BERT)  default is FTFP_BERT \n";
  print STDOUT "   (-n) evtmax      : Maximum number of events per run         default 26.2M events\n";         
  print STDOUT "   (-o)output dir   : Directory for output                     default /minerva/data/users/laliaga\n"; 
  print STDOUT "==============================================================================================\n";
}

$inc_mom   = $ARGV[0];
$first_run = $ARGV[1];
$last_run  = $ARGV[2];

$firstseed = $inc_mom;

$mom_gev = $inc_mom;

$inc_mom = 1000.*$inc_mom; #From GeV to MeV:
$massProton = 938.27201;
$inc_energy = sqrt($inc_mom*$inc_mom+$massProton*$massProton);

if( $first_run > $last_run ){
   print STDERR "\n First run number should be less than last run number.\n";
   exit 1;
}

if( $#ARGV < 0 ){
  &show_usage;
  exit 1;
}

#help
$help     = 0;
for(0 .. $#ARGV){
#help options
  $help      = 1 if $ARGV[$_]=~/^-(-)?h(elp)?$/i;
}
#check help
if($help==1){
  &show_usage;
  exit 1;
}

$hadphys   = "FTFP_BERT";
$evtmax    = 26200000;
$outputtopdir = "/minerva/data/users/laliaga/HadProd/test";

$randsteps = int(200*rand())+1;

for(0 .. $#ARGV){
#entries:
  $hadphys      = $ARGV[$_+1] if $ARGV[$_]=~/^-p$/;
  $evtmax       = $ARGV[$_+1] if $ARGV[$_]=~/^-n$/;
  $outputtopdir = $ARGV[$_+1] if $ARGV[$_]=~/^-o$/;
} 
  $outputdir     = "$outputtopdir/$hadphys";
  $log_dir       = "$outputdir/logs";
  $mac_dir       = "$outputdir/macros";
  $tuple_dir     = "$outputdir/tuples";
  $histo_dir     = "$outputdir/histos";

mkpath( $outputdir, 1, 0775 );
chmod 0775, $outputdir;

mkpath( $log_dir, 1, 0775 );
chmod 0775, $log_dir;

mkpath( $mac_dir, 1, 0775 );
chmod 0775, $mac_dir;

mkpath( $tuple_dir, 1, 0775 );
chmod 0775, $tuple_dir;

mkpath( $histo_dir, 1, 0775 );
chmod 0775, $histo_dir;


for ($runnum = $first_run; $runnum <= $last_run; $runnum++) {

 $macrofile = "${mac_dir}/g4na49_${mom_gev}_R${runnum}_${hadphys}.in"; 
 $logfile = "${log_dir}/g4na49_${mom_gev}_R${runnum}_${hadphys}.log";

 open (NEW,">$macrofile") or die "ERROR: Can't open $macrofile\n";
 print NEW "/control/verbose 0\n";
 print NEW "/run/verbose 0\n";
 print NEW "/tracking/verbose 0\n";
# print NEW "/testhadr/Physics $hadphys\n";
# print NEW "/testhadr/Update\n";
 print NEW "/run/initialize\n";
 print NEW "/random/setSeeds $firstseed $runnum\n";
 print NEW "/gun/particle proton\n";
 print NEW "/gun/energy $inc_energy MeV\n";
 print NEW "/run/beamOn $evtmax\n";
 print NEW;
 close (NEW); 
# system "nohup ./g4na49 ${macrofile} > ${logfile}&";


 my $working_dir=getcwd;

 $keep_tuple=0;

#Q:why are we calling system here??
#A: so that we can test "jobsub -g ... " by printing it to the screen rather than calling system (i.e., comment it out)
#system "jobsub -g -L ${logfile} -dTUPLES ${tuple_dir} -dHISTOS ${histo_dir} g4na49_job.sh ${working_dir} ${macrofile} ${mom_gev} ${runnum} ${keep_tuple} ${evtmax}";
system "jobsub_submit  --OS=SL6 -g --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC --use_gftp --role=Analysis -L ${logfile} -dTUPLES ${tuple_dir} -dHISTOS ${histo_dir}  ${working_dir} ${macrofile} ${mom_gev} ${runnum} ${keep_tuple} ${evtmax} file://g4na49_job.sh";
 

}

###########################################################################

