#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "44Sc" "48V" ${LABEL}
run_command "45Sc" "48V" ${LABEL}
run_command "45Ti" "48V" ${LABEL}
run_command "46Ti" "48V" ${LABEL}
run_command "46V" "48V" ${LABEL}
run_command "47V" "48V" ${LABEL}
run_command "48V" "48V" ${LABEL}
run_command "48Cr" "48V" ${LABEL}
run_command "49Cr" "48V" ${LABEL}
run_command "50Cr" "48V" ${LABEL}
run_command "50Mn" "48V" ${LABEL}
run_command "51Mn" "48V" ${LABEL}
run_command "52Mn" "48V" ${LABEL}
run_command "52Fe" "48V" ${LABEL}
run_command "53Fe" "48V" ${LABEL}
run_command "54Fe" "48V" ${LABEL}
run_command "54Co" "48V" ${LABEL}
run_command "55Co" "48V" ${LABEL}
