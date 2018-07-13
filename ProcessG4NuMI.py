#!/usr/bin/env python
##################################################
# g4numi grid job submitter
# Replacing g4numi_grid_submit.sh.
# Made to work with macros/template_ME.mac.
# 2018.01 -- B. Messerly
##################################################
import os, optparse, random, shutil, tarfile, sys
import subprocess, string

CACHE_PNFS_AREA = "/pnfs/{EXPERIMENT}/scratch/users/{USER}/grid_cache/".format(EXPERIMENT = os.getenv("EXPERIMENT"),
                                                                               USER = os.getenv("USER"))
PWD = os.getenv("PWD")

##################################################
# Job Defaults
##################################################
POT                   = 400000
N_JOBS                = 1
RUN_NUMBER            = 1
OUTDIR                = "/pnfs/{EXPERIMENT}/persistent/users/{USER}/flux/test/".format(EXPERIMENT = os.getenv("EXPERIMENT"),
                                                                                       USER = os.getenv("USER"))
TEMPLATE              = "{0}/macros/template_ME.mac".format(PWD)
FILETAG               = ""
TARFILE_NAME          = "local_install.tar.gz"

##################################################
# g4numi Defaults
##################################################
BEAMCONFIG            = "me000z200i"
PLAYLIST              = "minervame"
HORN1_POSITION_X      = 0      #cm
HORN1_POSITION_Y      = 0      #cm
HORN1_POSITION_Z      = 3      #cm
HORN2_POSITION_X      = 0      #cm
HORN2_POSITION_Y      = 0      #cm
HORN_WATER_MM         = 1      #mm
DO_HORN1_NEW_GEOMETRY = True

BEAM_POSITION_X       = 0      #m
BEAM_POSITION_Y       = 0      #m
BEAM_SPOTSIZE_X       = 1.4    #mm (ME!)
BEAM_SPOTSIZE_Y       = 1.4    #mm (ME!)

TARGET_POSITION_X     = 0.0    #cm
TARGET_POSITION_Y     = 0.0    #cm
TARGET_POSITION_Z     = -143.3 #cm (ME!)

##################################################
# Depreciated Options
##################################################
DO_HORN1_FINE_SEGMENTATION = False
NO_IMPORTANCE_WEIGHTING    = False
DO_TARGET_WATER            = False
TARGET_WATER_CM            = 3 #cm

##################################################
# Target optimnization work
##################################################
NUMBER_OF_FINS = 48   #default ME
DISTANCE_BETWEEN_FINS=0.5  #mm

##################################################
# beamconfig/playlist/targetZpos lookup
##################################################
LEbeamconfig_dict = { "le010z185i"  : {"minerva1"   : "-44.50",    #tgt2H1 = 9.50
                                     "minerva7"   : "-44.18",    #tgt2H1 = 9.18
                                     "minerva9"   : "-45.40",    #tgt2H1 = 10.40 
                                     "minerva13"  : "-44.17"},   #tgt2H1 = 9.17
                    "le010z-185i" : {"downstream" : "-44.50",    #tgt2H1 = 9.5
                                     "minerva5"   : "-43.85",    #tgt2H1 = 8.85
                                     "minerva10"  : "-44.18"},   #tgt2H1 = 9.18
                    "le010z000i"  : {"minerva6"   : "-44.18"},   #tgt2H1 = 9.18
                    "le100z200i"  : {"minerva2"   : "-134.57",   #tgt2H1 = 99.57
                                     "minerva11"  : "-134.17"},  #tgt2H1 = 99.17
                    "le100z-200i" : {"minerva3"   : "-134.57",   #tgt2H1 = 99.57
                                     "minerva12"  : "-134.17"},  #tgt2H1 = 99.17
                    "le250z200i"  : {"minerva4"   : "-284.57",   #tgt2H1 = 249.57
                                     "minerva8"   : "-285.09"}   #tgt2H1 = 250.09
                  }
##################################################

def main():
  options = get_options()

  g4_macro = make_macro(options)

# scratch /pnfs area from which to send tarfile to grid
  cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
  os.mkdir(cache_folder)

  print "\nTarring up local area..."
  make_tarfile(TARFILE_NAME, ".")
  
  shutil.move(TARFILE_NAME,    cache_folder) # temp file -> remove it from pwd
  shutil.move(g4_macro,        cache_folder) # temp file -> remove it form pwd
  shutil.copy("g4numi_job.sh", cache_folder)

  print "\nTarball of local area:", cache_folder + TARFILE_NAME

  logfile = options.outdir + "/g4numi_{BEAMCONFIG}_{RUN}_\$PROCESS.log".format(BEAMCONFIG = options.beamconfig,
                                                                               RUN        = options.run_number)

  print "\nOutput logfile(s):",logfile

  submit_command = ("jobsub_submit {GRID} {MEMORY} -N {NJOBS} -dG4NUMI {OUTDIR} "
      "-G {EXPERIMENT} "
      "-e BEAMCONFIG={BEAMCONFIG} " 
      "-e PLAYLIST={PLAYLIST} "
      "-e RUN={RUN} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/g4numi_job.sh".format(
      GRID       = ("--OS=SL6 -g "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC "
                    "--role=Analysis "),
      MEMORY     = "--memory 200MB ",
      NJOBS      = options.n_jobs,
      OUTDIR     = options.outdir,
      EXPERIMENT = os.getenv("EXPERIMENT"),
      BEAMCONFIG = options.beamconfig,
      PLAYLIST   = options.playlist,
      RUN        = options.run_number,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      CACHE      = cache_folder)
  )

  #Ship it
  print "\nSubmitting to grid:\n"+submit_command+"\n"
  status = subprocess.call(submit_command, shell=True)  

def get_options():
  parser       = optparse.OptionParser(usage="usage: %prog [options]")
  grid_group   = optparse.OptionGroup(parser, "Grid Options")

  grid_group.add_option("--outdir",
                default = OUTDIR,
                help    = "Output flux histograms location. Default = %default.")

  grid_group.add_option("--n_jobs",
        default = N_JOBS,
        help = "Number of g4numi jobs. Default = %default.")

  grid_group.add_option("--run_number",
        default = RUN_NUMBER,
        help = "Tag on the end of outfiles. Doubles as random # seed. Default = %default.")

  grid_group.add_option('--pot',
        default = POT,
        help="Number of protons on target to simulate. Default = %default.")

  grid_group.add_option('--filetag', default = FILETAG)

  horn_group   = optparse.OptionGroup(parser, "Horn Options")

  horn_group.add_option('--do_horn1_old_geometry',   action="store_true", 
        default = False,
        help="The new horn 1 geom (formerly known as 'alternate') is now default. " 
             "Use this option to use the old geometry. Default = %default.")

  horn_group.add_option('--do_horn1_fine_segmentation', action="store_true", 
        default = False,
        help="Works for old and new horn1. Default = %default.")

  horn_group.add_option('--horn1_position_X', default = HORN1_POSITION_X,
        help="horn 1 transverse offset (_X0). Default = %defaultcm.")
  horn_group.add_option('--horn1_position_Y', default = HORN1_POSITION_Y,
        help="horn 1 vertical offset (_Y0). Default = %defaultcm.")
  horn_group.add_option('--horn1_position_Z', default = HORN1_POSITION_Z,
        help="horn 1 longitudinal offset (_Z0). Default = %defaultcm.")

  horn_group.add_option('--horn2_position_X', default = HORN2_POSITION_X,
        help="horn 2 transverse offset (_X0). Default = %defaultcm.")
  horn_group.add_option('--horn2_position_Y', default = HORN2_POSITION_Y,
        help="horn 2 vertical offset (_Y0). Default = %defaultcm.")

  horn_group.add_option('--horn_water_mm', default = HORN_WATER_MM,
        help="Water layer on horn. Default = %defaultmm.")

  target_group = optparse.OptionGroup(parser, "Target Options")

  target_group.add_option('--target_position_X', default = TARGET_POSITION_X,
          help="target horizontal position. Default = %defaultcm.")
  target_group.add_option('--target_position_Y', default = TARGET_POSITION_Y,
          help="target vertical position. Default = %defaultcm.")
  target_group.add_option('--target_position_Z', default = TARGET_POSITION_Z,
          help="target longitudinal position. Default = %defaultcm.")
  target_group.add_option('--number_of_fins', default = NUMBER_OF_FINS,
          help="number of fins for ME tgt target. Default = 48.")
  target_group.add_option('--distance_between_fins', default = DISTANCE_BETWEEN_FINS,
          help="distance between fins for ME tgt target. Default = %defaultmm.")

  beam_group   = optparse.OptionGroup(parser, "Beam Options")

  beam_group.add_option('--beam_position_X', default = BEAM_POSITION_X,
        help="beam horizontal position. Default = %defaultmm.")
  beam_group.add_option('--beam_position_Y', default = BEAM_POSITION_Y,
        help="beam vertical position. Default = %defaultmm.")
  beam_group.add_option('--beam_spotsize_X', default = BEAM_SPOTSIZE_X,
        help="beam horizontal spot size. Default = %defaultmm.")
  beam_group.add_option('--beam_spotsize_Y', default = BEAM_SPOTSIZE_Y,
        help="beam vertical spot size. Default = %defaultmm.")

  old_group    = optparse.OptionGroup(parser, "Old Options")

  old_group.add_option('--template', default = TEMPLATE,
        help='Specify template macro. Default = %default.')
  old_group.add_option('--no_importance_weighting', action="store_true", 
        default = NO_IMPORTANCE_WEIGHTING,
        help="Turn off importance weighting. Default: importance weighting is on, "
             "i.e. this option is %default." )
  old_group.add_option('--target_water_cm', default = TARGET_WATER_CM,
        help="Simulate water in the target. In cm. Default = %defaultcm." )
  old_group.add_option('--do_target_water', default = False)
  old_group.add_option('--playlist', default = PLAYLIST,
        help="correct the Z target posision per playlist (LE). Default = %default.")
  old_group.add_option('--beamconfig', default = BEAMCONFIG, 
        help="Configure the neutrino beam. "
              "This sets the TargetZ0 and Baffle Z0 and HornCurrent. "
              "Must be in the form 'LE#z#i' or 'le#z#i', where # is any number. "
              "Examples are le010z185i, LE025.3z-200i, LE250z185.6i....etc. Default = %default.")

  parser.add_option_group(grid_group)
  parser.add_option_group(horn_group)
  parser.add_option_group(beam_group)
  parser.add_option_group(target_group)
  parser.add_option_group(old_group)

  options, remainder = parser.parse_args()

  options = finalize_options(options)

  return options

def finalize_options(options):
  options.beamconfig = options.beamconfig.lower()
  options.playlist   = options.playlist.lower()

  # Beam configuration ~ target Z position (depreciated in medium energy era)
  if options.beamconfig != BEAMCONFIG.lower():
    try:
      options.target_position_Z = LEbeamconfig_dict[options.beamconfig][options.playlist] if not options.target_position_Z else options.target_position_Z
    except:
      print sys.exit("Error! beamconfig-playlist pair not found!")

  # Target Water Layer (depreciated in medium energy era)
  if options.target_water_cm != TARGET_WATER_CM:
    options.do_target_water = True

  # Filetag
  if options.filetag != FILETAG:
    options.filetag = options.filetag if options.filetag[:1] == '_' else '_'+options.filetag

  return options

def make_macro(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)

  macro_string = template.safe_substitute(
      {'do_horn1_new_geometry':      str(not options.do_horn1_old_geometry).lower(),
       'do_horn1_fine_segmentation': str(options.do_horn1_fine_segmentation).lower(),
       'horn1_position_X':           options.horn1_position_X,
       'horn1_position_Y':           options.horn1_position_Y,
       'horn1_position_Z':           options.horn1_position_Z,
       'horn2_position_X':           options.horn2_position_X,
       'horn2_position_Y':           options.horn2_position_Y,
       'horn_water_mm':              options.horn_water_mm,
       'beam_position_X':            options.beam_position_X,
       'beam_position_Y':            options.beam_position_Y,
       'beam_spotsize_X':            options.beam_spotsize_X,
       'beam_spotsize_Y':            options.beam_spotsize_Y,
       'target_position_X':          options.target_position_X,
       'target_position_Y':          options.target_position_Y,
       'target_position_Z':          options.target_position_Z,
       'target_water_cm':            options.target_water_cm,
       
       'number_of_fins':            options.number_of_fins,
       'distance_between_fins':     options.distance_between_fins,

       'do_target_water':            str(options.do_target_water).lower(),
       'pot':                        options.pot,
       'run':                        options.run_number,
       'beamconfig':                 options.beamconfig.lower(),
       'playlist':                   options.playlist.lower(),
       'do_importance_weighting':    str(not options.no_importance_weighting).lower()
       #'outfile':                    outfile,     #set in g4numi_job.sh
       #'seed':                       options.seed #set in g4numi_job.sh
      }
    )

  macro_name = "g4numi.mac"
  macro = open(macro_name, "w") 
  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename, source_dir):
  tar = tarfile.open(output_filename, "w:gz")
  for i in os.listdir(source_dir):
    tar.add(i)
  tar.close()

if __name__ == "__main__":
  sys.exit(main())
