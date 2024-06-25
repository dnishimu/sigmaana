#!/bin/bash

run_command() {
  ./Batch_Sigma_calc.sh "$1" "$2" "$3"
}
run_command "41Ti" "39Ca" ${LABEL}
run_command "42Ti" "39Ca" ${LABEL}
run_command "40Sc" "39Ca" ${LABEL}
run_command "41Sc" "39Ca" ${LABEL}
run_command "38Ca" "39Ca" ${LABEL}
run_command "39Ca" "39Ca" ${LABEL}
run_command "40Ca" "39Ca" ${LABEL}
run_command "36K" "39Ca" ${LABEL}
run_command "37K" "39Ca" ${LABEL}
run_command "38K" "39Ca" ${LABEL}
run_command "35Ar" "39Ca" ${LABEL}
run_command "36Ar" "39Ca" ${LABEL}
run_command "34Cl" "39Ca" ${LABEL}
run_command "35Cl" "39Ca" ${LABEL}
run_command "36Cl" "39Ca" ${LABEL}
