#!/bin/sh
  

# TRAIN FROM SINGLE MOCK
declare -i cosmo
declare -i hod
cosmo=16
hod=1626

savetag="_nonolap"
statistic="vpf"

result_dir="results_$statistic"
mkdir -p "${result_dir}/training_${statistic}${savetag}"

savefn=${result_dir}/training_${statistic}${savetag}/${statistic}_cosmo_${cosmo}_HOD_${hod}_test_0.dat
#meanngalsfn=numgals/mean_numgals_training.dat
meanngalsfn=numgals/zero.dat
if [ ! -f $savefn ]; then
  echo "Computing upf of cosmo $cosmo hod $hod savetag $savetag" 
  mockdir=/mount/sirocco2/zz681/emulator/CMASSLOWZ/galaxy_mocks/mocks
  #old: mockdir=/mount/sirocco1/zz681/emulator/CMASS_BIAS/COS_2000HOD/galaxy_mock/
  mockname=mock_cosmo_${cosmo}_HOD_${hod}_test_0.mock
  "/home/users/ksf293/clust/upf" $mockdir/$mockname 5 45 9 $savefn $meanngalsfn
fi
