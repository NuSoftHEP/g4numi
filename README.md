The `g4numi` code consists of code to build a representation of the NuMI
beamline geometry and interface it with the Geant4 physics tools. The
same geometry is also used for the [Flugg](https://cdcvs.fnal.gov/redmine/projects/numi-beam-sim/wiki/Flugg) simulation. A simple
model of incoming proton beam is used as a source of initial particles.
The primary output of the this code is an ntuple representing the decays
of secondaries that give rise to neutrinos.

## building procedure

You can find instructions on building g4numi on the Numi Beam Simulations wiki [here](https://github.com/NuSoftHEP/g4numi/wiki/How-to-build-and-run-the-main-modern-g4-branch)

## physics processes

There is a [FNAL Geant4
Effort](https://sharepoint.fnal.gov/project/geant4-pub/SitePages/Home.aspx)
to improve the *physics list* used to control (mostly hadronic)
interactions in order to better agree with data (e.g. NA49, NA61, etc)
in physics regimes that are relevant for the NuMI beam.

## Important note about entry values

The following are *additions* to the base `g4numi` ntuple (`nudata`).
Unlike the rest of the ntuple which uses *cm* (for lengths) and *GeV*
(for energies and momenta) this portion uses *mm* and *MeV*, so take
care.

These hold information about the ancestor between the original proton
and the neutrino. The arrays are a fixed length of 10 entries, though it
is possible for the actual history chain to be longer than that (in
which case it is truncated). For any one entry only the indices `0` to
`min(ntrajectory-1,9)` are meaningful. Elements `ntrajectory` and beyond
are filled with junk left over from previous entries; do not use them.

        Int_t           ntrajectory;
        Bool_t          overflow;
        Int_t           pdg[10];
        Int_t           trackId[10];
        Int_t           parentId[10];
        Double_t        startx[10];
        Double_t        starty[10];
        Double_t        startz[10];
        Double_t        stopx[10];
        Double_t        stopy[10];
        Double_t        stopz[10];
        Double_t        startpx[10];
        Double_t        startpy[10];
        Double_t        startpz[10];
        Double_t        stoppx[10];
        Double_t        stoppy[10];
        Double_t        stoppz[10];
        Double_t        pprodpx[10];
        Double_t        pprodpy[10];
        Double_t        pprodpz[10];
        TString         proc[10];
        TString         ivol[10];
        TString         fvol[10];

## How to run g4numi on NUMI-X nodes: \[\[A newcomer’s guide\]\]
