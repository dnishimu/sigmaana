#!/bin/bash
run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}
run_command "42Sc" "47Cr" ${LABEL}
run_command "43Sc" "47Cr" ${LABEL}
run_command "43Ti" "47Cr" ${LABEL}
run_command "44Ti" "47Cr" ${LABEL}
run_command "45Ti" "47Cr" ${LABEL}
run_command "44V" "47Cr" ${LABEL}
run_command "45V" "47Cr" ${LABEL}
run_command "46V" "47Cr" ${LABEL}
run_command "47V" "47Cr" ${LABEL}
run_command "46Cr" "47Cr" ${LABEL}
run_command "47Cr" "47Cr" ${LABEL}
run_command "48Cr" "47Cr" ${LABEL}
run_command "48Mn" "47Cr" ${LABEL}
run_command "49Mn" "47Cr" ${LABEL}
run_command "50Mn" "47Cr" ${LABEL}
run_command "50Fe" "47Cr" ${LABEL}
run_command "51Fe" "47Cr" ${LABEL}
