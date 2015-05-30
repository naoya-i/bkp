# bkp
Baka Parallel Processor

## Prologue
We want to process a lot of similar tasks (e.g., parsing 100,000 documents) with several machines connected by a network.
Because the tasks are independent, we can exploit parallel processing to finish these tasks efficiently.
One option could be to use GNU Parallel (http://www.gnu.org/software/parallel/).
However, GNU Parallel works only on a single machine.
Remember, what we want to do is to distribute these tasks over several machines.
Then the option could be to use gxp (http://www.logos.t.u-tokyo.ac.jp/gxp/), 
but our environment does not allow us to ```ssh``` the machines. _Oh god!_


## Prerequisites
- MPICH (http://www.mpich.org/)

In the below, we assume that you are ready to use MPICH (e.g., ```mpdboot -n 20 -f mpd.hosts```).
Please look for other nice websites to figure out how to make it.


## Install
Clone and build our bkp.

    $ git clone https://github.com/naoya-i/bkp
    $ cd bkp
    $ make


## Usage

    $ mpirun -np NUM_OF_PARALLEL_PROCESSES bin/bkp NUM_OF_TASKS COMMAND

- _NUM_OF_PARALLEL_PROCESSES_: The number of processes that are executed in parallel.
- _NUM_OF_TASKS_: The number of tasks to be processed.
- _COMMAND_: The command to be execute to process each task.

For example, let us output 1,000 ```Boooom``` with bkp. First, we prepare a shell script as follows:

    $ cat myprocessor.sh
    echo "Boooom"

Then, we use the following command to use bkp:

    $ mpirun -np 16 bin/bkp 1000 bash myprocessor.sh

Of course, this is useless though.


## Advanced Usage

You can use the environmental variable ```BKP_INDEX``` to determine what task should be processed by this task.
The simplest example is the following:

    $ cat myprocessor.sh
    echo "Hi, my name is bkp. My task number is $BKP_INDEX."

Processing 1,000 files in parallel:

    $ cat myprocessor.sh
    sort /foo/bar/data.$BKP_INDEX.txt > /foo/bar/data.sorted.$BKP_INDEX.txt

