declare -i cosmo_id
declare -i hod_id
cosmo_id=16
hod_id=1626

statistic="knn"
knn_order_max=2
L=1050.0
redshift=0.57
n_bins=9
r_min=-1
r_max=-1

result_dir="/home/users/ksf293/clust/results_aemulus_train/results_$statistic"
mkdir -p ${result_dir}
echo ${result_dir}

fn_save=${result_dir}/${statistic}_cosmo_${cosmo_id}_HOD_${hod_id}_test_0.dat
cosmofn=/mount/sirocco1/zz681/emulator/CMASS/Gaussian_Process/hod_file/cosmology_camb_full.dat

echo "Computing knn of cosmo $cosmo_id hod $hod_id" 
mockdir=/mount/sirocco2/zz681/emulator/CMASSLOWZ/galaxy_mocks/mocks
mockname=mock_cosmo_${cosmo_id}_HOD_${hod_id}_test_0.mock

echo "python run_statistics_aemulus.py $mockdir/$mockname $L $cosmofn $cosmo_id $redshift $statistic $fn_save $r_min $r_max $n_bins -knn_order_max=$knn_order_max"
python run_statistics_aemulus.py $mockdir/$mockname $L $cosmofn $cosmo_id $redshift $statistic $fn_save $r_min $r_max $n_bins -knn_order_max=$knn_order_max