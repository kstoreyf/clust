# clust

This repo contains scripts for computing clustering statistics on periodic cosmological simulation boxes, and associated routines. These were used in the paper "The Aemulus Project VI: Emulation of beyond-standard galaxy clustering statistics to improve cosmological constraints" (https://arxiv.org/abs/2210.03203).

The clustering statistics included are: the monopole and quadrupole of the correlation function, the projected correlation function, the underdensity probability function (UPF), the marked correlation function (MCF), and k-Nearest Neighbors statstics.

The main scripts to note are:
- run_statistics_aemulus.c: Runs C-based clustering statistics (UPF, density marks for the MCF) on Aemulus simulation boxes (https://aemulusproject.github.io/)
- run_statistics_aemulus.py: Runs python-based clustering statistics (all others) on Aemulus simulation boxes.
- run_statistics_mocks.c: Runs C-based clustering statistics (UPF, density marks for the MCF) on more general simulation formats.
- run_statistics_mocks.py: Runs python-based clustering statistics (all others) on more general simulation formats.

The relevant routines for each clustering statistic are called from the above scripts. 

Also of note is our custom code for computing a kd-tree on a periodic simulation box:
- kdtree/kdtree_periodic.c (an extension of https://github.com/jtsiomb/kdtree)
