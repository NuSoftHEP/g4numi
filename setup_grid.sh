#!/bin/bash

# Grid + Permissions
voms-proxy-destroy # destroy the existing certificates, to get a new with
kx509
voms-proxy-init --rfc --voms=fermilab:/fermilab/minerva/Role=Analysis --noregen

# jobsub_submit + client
source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups.sh
setup jobsub_client
