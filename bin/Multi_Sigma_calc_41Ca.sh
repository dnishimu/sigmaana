#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "44Ti" "41Ca" ${LABEL}
run_command "45Ti" "41Ca" ${LABEL}
run_command "42Sc" "41Ca" ${LABEL}
run_command "43Sc" "41Ca" ${LABEL}
run_command "44Sc" "41Ca" ${LABEL}
run_command "40Ca" "41Ca" ${LABEL}
run_command "41Ca" "41Ca" ${LABEL}
run_command "42Ca" "41Ca" ${LABEL}
run_command "39K" "41Ca" ${LABEL}
run_command "40K" "41Ca" ${LABEL}
run_command "41K" "41Ca" ${LABEL}
run_command "37Ar" "41Ca" ${LABEL}
run_command "38Ar" "41Ca" ${LABEL}
run_command "35Cl" "41Ca" ${LABEL}
run_command "36Cl" "41Ca" ${LABEL}
