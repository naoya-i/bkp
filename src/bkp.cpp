// BKP: BaKa Parallel processor.
//

#include "mpi.h"

#include <sstream>
#include <iostream>
#include <stack>
#include <algorithm>

#include <math.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

inline string _timestamp() {
  static string weekday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  time_t t; tm *p_ltm;

  time(&t);
  p_ltm = localtime(&t);

  char ret[1024];
  sprintf(ret, "%02d/%02d/%04d %02d:%02d:%02d", 1+p_ltm->tm_mon, p_ltm->tm_mday, 1900+p_ltm->tm_year, p_ltm->tm_hour, p_ltm->tm_min, p_ltm->tm_sec);

  return ret;
}

string _getTargetCommand(int argc, char **argv) {
  string ret;

  for(int i=2; i<argc; i++) {
    if(2 != i)
      ret += " ";

    ret += argv[i];
  }

  return ret;
}

void _queen(const char *processorName, int myMPIrank, int numMPIprocs, int argc, char **argv) {
  int msgExit  = -1;
  int numTasks = atoi(argv[1]);

  cout << _timestamp() << " QUEEN@" << processorName << ": Target command: " << _getTargetCommand(argc, argv) << endl;
  cout << _timestamp() << " QUEEN@" << processorName << ": # of tasks: " << numTasks << endl;

  // Wait free worker processes to assign a new task.
  for(int i=0; i<numTasks; i++) {
    MPI_Status statRecv;
    int        msgRecv;

    MPI_Recv(&msgRecv, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &statRecv);
    MPI_Send(&i,       1, MPI_INT, statRecv.MPI_SOURCE, 0, MPI_COMM_WORLD);
  }

  // Send "exit message" to all the worker processes.
  for(int i=1; i<numMPIprocs; i++)
    MPI_Send(&msgExit, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
}

void _worker(const char *processorName, int myMPIrank, int argc, char **argv) {
  int    msgSent = 0;
  string cmd     = _getTargetCommand(argc, argv);

  while(true) {
    // Inform the queen that I am free.
    int        msgRecv = 0;
    MPI_Status statRecv;

    MPI_Send(&msgSent, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Recv(&msgRecv, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &statRecv);

    cout << _timestamp() << " R" << myMPIrank << "@" << processorName << " is assigned the task " << msgRecv << "..." << endl;

    if(-1 == msgRecv) break;

    // Execute the process.
    ostringstream ss;
    ss << "BKP_INDEX=" << msgRecv << " " << cmd;

    cout << _timestamp() << " R" << myMPIrank << "@" << processorName << " is executing " << ss.str() << endl;

    int ret = system(ss.str().c_str());

    cout << _timestamp() << " R" << myMPIrank << "@" << processorName << " has done the task " << msgRecv << "! (status=" << ret << ")" << endl;
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  // Get the information of the current MPI execution.
  int numMPIprocs, myMPIrank;
  MPI_Comm_size(MPI_COMM_WORLD, &numMPIprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myMPIrank);

  int namelen;
  char processorName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processorName, &namelen);

  // Worker bees (rank N, where N != 0) works for a queen (rank = 0).
  if(0 == myMPIrank) _queen(processorName, myMPIrank, numMPIprocs, argc, argv);
  else               _worker(processorName, myMPIrank, argc, argv);

  MPI_Finalize();

  return 0;
}
