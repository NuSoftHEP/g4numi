#"--Prachi Sharma December 21, 2023"

Readme file for plotting the flux distributions of particles on a virtual detector plane/H1 Tracking Plane placed downstream of NuMI Horn 1

** "Some general information about the Virtual Detector plane":

* The Virtual Detector Plane is 1mm thick and has an area of 1mx1m.
* By default the virtual detector plane is placed at a distance of z = -1600cm  (calculated as the distance 0 gives virtual detector at z=1950, so for getting the virtual detector at 350 cm I had to do like 350-1950=-1600)



** "Some technical information":

* The distance of the Virtual Detector Plane can be changed from the src/NumiDetectorConstruction.cc file. Search for the function NumiDetectorConstruction::ConstructNumiHorn1TrackingPlane and change the position of the Virtual Plane as desired in the double z = -{my virtual plane placement} * CLHEP::cm, but  ######NOTE:You have to compile after making this change (Just type "make" to compile the changes )
* Also, the thickness of the Virtual Detector can be changed in a similar manner as above with the variable 'trackingPlanesHalfThickness' which will set the half thickness of the Virtual Detector Plane.

#####'TO RUN A JOB ON THE GRID WITH DEFAULT PARAMETERS:'######

* To Submit a job on the grid, (assuming you have already setup g4numi by 'source setup_beamsim.sh' and compiled after making changes, you can also change the Protons on Target in this script), just type this in the working area:
  "python ProcessG4NuMIh1trackingplane.py"




##### SOME MORE INFORMATION#################

* By default, in this branch,  the standard dk2nu files are turned off, to turn them on just set the following variable in the 'macros/template_ME_h1trackingplane.mac':::::
 '/NuMI/output/outputNuNtuple' to true 
 
* By default, the importance weights are also turned off. To turn them back on, set the following variable in the 'macros/template_ME_h1trackingplane.mac'::::
 '/NuMI/run/useNImpWeight' to true

* By default, the Horn Current is turned off, to turn it back on just change the 
BEAMCONFIG = "me000z000i"(Horn current 000) to something like "me000z200i" to set the horn current as 200kA or whatever is required.

* By default this branch also outputs the electrons, positrons and gamma particles (which are otherwise not recorded in the main branch), to turn them off, just uncomment the section that starts with: '// Discard Gammas, Electrons, ... '

* By default, we only get one output file containing just the Virtual Detector Plane/Horn 1 Tracking Plane TTree. The output file would look something like :
'h1_trackingplanev6_minervame_me000z000i_*_0001.root'

* The Stepping Action is defined in src/NumiSteppingAction.cc in the function : NumiSteppingAction::CheckInTrackingDetectorH1Plane.

* The TTree is defined, filled and written in src/NumiAanalysis.cc file: The functions:  NumiAnalysis::FillH1Ntuple ( Fills a tracking plane Ntuple with information from the current step in the Geant4 simulation and the function  NumiAnalysis::Writeh1trackingplaneNtuple (Writes the filled information into the TTree named as  h1trackingtree).


* The Output TTree branches are:

	-Job: Jobid or event id number
	-tvx, tvy, tvz: Particle Vertices at the plane
	-tpx, tpy, tpz: Particle momenta at the plane
	-tptype: Particle Type
	-ppvx, ppvy, ppvz : Particle Production Vertices where recorded particles are originally created



###############################################



