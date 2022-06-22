import numpy as np
import subprocess


mockdir="/mount/sirocco2/zz681/emulator/CMASSLOWZ_SHAM_Uchuu"
mockname="vpeak_scat0.08_format.dat"
mocktag="_uchuu"

fn = f'{mockdir}/{mockname}'
p = subprocess.Popen(['wc', '-l', fn], stdout=subprocess.PIPE, 
                                                stderr=subprocess.PIPE)
result, err = p.communicate()
if p.returncode != 0:
  raise IOError(err)
ngals = int(result.strip().split()[0])

print(ngals)

np.savetxt(f"numgals/numgals{mocktag}.dat", [ngals])
