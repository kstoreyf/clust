#!/bin/sh
#cosmos=(2 0 4)
#hods=(3 37 42)
#boxes=(0 4 2)

fiducial
cosmos=(2)
hods=(3)
boxes=(0)

#statistics=("marks" "mcf")
#statistics=("marks" "xi" "wxi")
statistics=("xi" "xi2")
#factor_star=1.0
#p=0.75
#savetag="_investigate_fstar${factor_star}_p${p}"
#savetag="_investigate"
#savetag="_padwhite2009_fstar${factor_star}_p${p}"
#savetag="_investigatelin_fstar${factor_star}_p${p}"
savetag=""

overwrite=true
test=0

workdir="/home/users/ksf293/clust"
echo $workdir
cosmofn=/mount/sirocco1/zz681/emulator/CMASS/Gaussian_Process/hod_file/cosmology_camb_test_box_full.dat

for statistic in ${statistics[@]}; do
  result_dir=$workdir/results_$statistic/testing_${statistic}${savetag}
  mkdir -p "${result_dir}"
  for idx in ${!cosmos[@]}; do
    cosmo=${cosmos[$idx]}
    hod=${hods[$idx]}
    box=${boxes[$idx]}
    savefn="${result_dir}/${statistic}_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat"
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
        echo "$workdir/marks_zspace $mockdir/$mockname $savefn $meanngalsfn $cosmofn $cosmo $factor_star $p"
        meanngalsfn=$workdir/numgals/mean_numgals_testing_hod100_test0.dat
        $workdir/marks_zspace $mockdir/$mockname $savefn $meanngalsfn $cosmofn $cosmo $factor_star $p
      fi
      if [ $statistic = "mcf" ] || [ $statistic = "wxi" ]; then
        echo "python markedcf.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo $statistic"
        marktag=$savetag
        markdir=$workdir/results_marks/testing_marks${marktag}
        markfn=${markdir}/marks_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat
        #python markedcf.py $mockdir/$mockname 0.1 50 9 $savefn $markfn $cosmofn $cosmo $statistic
        python markedcf.py $mockdir/$mockname 0 132 44 $savefn $markfn $cosmofn $cosmo $statistic
      fi
      if [ $statistic = "xi" ] || [ $statistic = "xi2" ]; then
        echo "python xi.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo $statistic"
        python xi.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo $statistic
      fi
    fi
  done
done
