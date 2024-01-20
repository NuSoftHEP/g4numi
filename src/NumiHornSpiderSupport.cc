
#include "NumiHornSpiderSupport.hh"

NumiHornSpiderSupport::NumiHornSpiderSupport()
{
  stripW=0;
  stripH=0;
  stripL=0;
  topW=0;
  topH=0;
  topL=0;
  bottomW=0;
  bottomH=0;
  bottomL=0;
  bottomThickMid=0;  //Thickness of bottom part in the middle
  bottomR=0;
  ceramicRodR=0;
}
NumiHornSpiderSupport::NumiHornSpiderSupport(const NumiHornSpiderSupport *HSS)
{
  stripW=HSS->stripW;
  stripH=HSS->stripH;
  stripL=HSS->stripL;
  topW=HSS->topW;
  topH=HSS->topH;
  topL=HSS->topL;
  bottomW=HSS->bottomW;
  bottomH=HSS->bottomH;
  bottomL=HSS->bottomL;
  bottomThickMid=HSS->bottomThickMid;  //Thickness of bottom part in the middle
  bottomR=HSS->bottomR;
  ceramicRodR=HSS->ceramicRodR;
}
NumiHornSpiderSupport::~NumiHornSpiderSupport()
{}
