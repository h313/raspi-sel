#include <csignal>
#include <cstdio>
#include <tuple>
#include <unistd.h>

#include "classify.h"
#include "ina3221.h"
#include "record.h"

static volatile bool sentinel = true;

void int_handler(int signum) {
  sentinel = false;
}

int main(int argc, char **argv) {
  std::string model_file;
  std::tuple<double, double, double> predicted, actual;
  int target_pid;

  if (argc != 3) {
    printf("Usage: %s MODEL_FILE TARGET_PID\n", argv[0]);
    return -1;
  }

  signal(SIGINT, int_handler);

  model_file = argv[1];
  target_pid = atoi(argv[2]);

  Model classify_model(model_file);
  RecordSystem system_stats;
  INA3221 current_stats;

  while (sentinel) {
    predicted = classify_model.test_model(system_stats.get_system_info());
    actual = current_stats.read_currents();

    usleep(50);
  }

  return 0;
}