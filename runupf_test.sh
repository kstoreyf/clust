cd $PBS_O_WORKDIR
echo "Hello from $PBS_VNODENUM..."
# Reading config.$PBS_VNODENUM" >> myoutput.$PBS_NODENUM
# Actual command, with data file based on $PBS_VNODENUM:
#$PBS_O_WORKDIR/upf /mount/sirocco1/zz681/emulator/CMASS_BIAS/COS_2000HOD/galaxy_mock/cosmo_${PBS_VNODENUM}_HOD_0_test_0.mock 0 1 3 $PBS_O_WORKDIR/results_test/vpf_cosmo_${PBS_VNODENUM}_HOD_0_test_0.txt
declare -i cosmo
declare -i box
declare -i hod
savetag="_cos0"
#cosmo=${PBS_VNODENUM}/5
cosmo=0
hod=${PBS_VNODENUM}/5
box=${PBS_VNODENUM}%5
echo $cosmo
echo $box

test=0
$PBS_O_WORKDIR/upf /mount/sirocco1/zz681/emulator/CMASS_BIAS/GP_Test_BOX/test_galaxy_mock/cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.mock 0 30 12 $PBS_O_WORKDIR/results/testing_upf${savetag}/upf_cosmo_${cosmo}_Box_${box}_HOD_${hod}_test_${test}.dat

