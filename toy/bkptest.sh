# HOW TO RUN ME ON TCCSC:
#   $ cd /path/to/bkp
#   $ qsub -q lx -b 2 toy/bkptest.sh
cd $PBS_O_WORKDIR
mpirun -ppn 24 -np 48 bin/bkp 48 python toy/hello.py


