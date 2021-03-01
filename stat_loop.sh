#!/bin/sh

hod=3
cosmos=(0 1 2 3 4 5 6)
boxes=(0 1 2 3 4)

statistics=("marks" "mcf")
#statistics=("marks" "xi")
factor_star=1.0
p=0.75
savetag="_investigatelin_fstar${factor_star}_p${p}"
#savetag="_investigate"

overwrite=false
test=0

workdir="/home/users/ksf293/clust"
echo $workdir
cosmofn=/mount/sirocco1/zz681/emulator/CMASS/Gaussian_Process/hod_file/cosmology_camb_test_box_full.dat

for statistic in ${statistics[@]}; do
    result_dir=$workdir/results_$statistic/testing_${statistic}${savetag}
    mkdir -p "${result_dir}"
    for cidx in ${!cosmos[@]}; do
        cosmo=${cosmos[$cidx]}
        for bidx in ${!boxes[@]}; do
            box=${boxes[$bidx]}
            savefn="${result_dir}/${statistic}_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat"
            if [ ! -f $savefn ] || [ $overwrite = true ]; then
                echo "Computing $statistic of cosmo $cosmo hod $hod box $box savetag $savetag"
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
                    echo "$workdir/marks_zspace $mockdir/$mockname $savefn $meanngalsfn $cosmofn $cosmo $facotr_star $p"
                    meanngalsfn=$workdir/numgals/mean_numgals_testing_hod100_test0.dat
                    $workdir/marks_zspace $mockdir/$mockname $savefn $meanngalsfn $cosmofn $cosmo $factor_star $p
                fi
                if [ $statistic = "mcf" ] || [ $statistic = "wxi" ] || [ $statistic = "xi" ]; then
                    echo "python markedcf.py $mockdir/$mockname 0.1 50 9 $savefn $cosmofn $cosmo"
                    #marktag=$savetag
                    marktag="_investigate_fstar${factor_star}_p${p}"
                    markdir=$workdir/results_marks/testing_marks${marktag}
                    markfn=${markdir}/marks_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat
                    #python markedcf.py $mockdir/$mockname 0.1 50 9 $savefn $markfn $cosmofn $cosmo $statistic
                    python markedcf.py $mockdir/$mockname 0 130 52 $savefn $markfn $cosmofn $cosmo $statistic
                fi
            fi
        done
    done
done
