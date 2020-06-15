#!/bin/sh
  

#TEST
declare -i cosmo
declare -i box
declare -i hod

hod=0
cosmo=2
box=0
savetag="_check"
statistic="mcf"
overwrite=true
test=0

workdir="/home/users/ksf293/clust"
echo $workdir
result_dir=$workdir/results_$statistic/testing_${statistic}${savetag}
mkdir -p "${result_dir}"
savefn=${result_dir}/${statistic}_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat
cosmofn=/mount/sirocco1/zz681/emulator/CMASS/Gaussian_Process/hod_file/cosmology_camb_test_box_full.dat
if [ ! -f $savefn ] || [ $overwrite = true ]; then
  echo "Computing $statistic of cosmo $cosmo hod $hod savetag $savetag"
  mockdir=/mount/sirocco2/zz681/emulator/CMASSLOWZ/test_galaxy_mocks_wp_RSD/test_galaxy_mocks_new_f_env/mocks
  mockname=mock_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.mock
  
  if [ $statistic = "upf" ]; then
    meanngalsfn=$workdir/numgals/mean_numgals_testing_hod100_test0.dat
    $workdir/upf_zspace $mockdir/$mockname 5 45 9 $savefn $meanngalsfn $cosmofn $cosmo
  fi
  if [ $statistic = "vpf" ]; then
    meanngalsfn=$workdir/numgals/zero.dat
    $workdir/upf_zspace $mockdir/$mockname 5 45 9 $savefn $meanngalsfn $cosmofn $cosmo
  fi
  if [ $statistic = "wp" ]; then
    echo "python wprp.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo"
    python wprp.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo
  fi
  if [ $statistic = "marks" ]; then
    meanngalsfn=$workdir/numgals/mean_numgals_testing_hod100_test0.dat
    $workdir/marks_zspace $mockdir/$mockname $savefn $meanngalsfn $cosmofn $cosmo
  fi
  if [ $statistic = "mcf" ]; then
    echo "python markedcf.py $mockdir/$mockname 5 45 9 $savefn $cosmofn $cosmo"
    marktag=$savetag
    markdir=$workdir/results_marks/testing_marks${marktag}
    markfn=${markdir}/marks_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat
    python markedcf.py $mockdir/$mockname 2.5 47.5 9 $savefn $markfn $cosmofn $cosmo
  fi

fi
