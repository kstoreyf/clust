cd $PBS_O_WORKDIR
echo "Hello from $PBS_VNODENUM..."
# Reading config.$PBS_VNODENUM" >> myoutput.$PBS_NODENUM
# Actual command, with data file based on $PBS_VNODENUM:
# TRAIN
declare -i cosmo
declare -i hod
cosmo=0
hod=${PBS_VNODENUM}
savetag="_cos0"

#if [ ! -f /tmp/foo.txt ]; then
$PBS_O_WORKDIR/upf /mount/sirocco1/zz681/emulator/CMASS_BIAS/COS_2000HOD/galaxy_mock/cosmo_${cosmo}_HOD_${hod}_test_0.mock 0 30 12 $PBS_O_WORKDIR/results/training_upf${savetag}/upf_cosmo_${cosmo}_HOD_${hod}_test_0.dat

