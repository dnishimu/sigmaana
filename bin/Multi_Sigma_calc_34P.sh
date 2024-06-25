#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}
run_command "33P" "34P" ${LABEL}
run_command "34P" "34P" ${LABEL}
run_command "35P" "34P" ${LABEL}
run_command "35S" "34P" ${LABEL}
run_command "36S" "34P" ${LABEL}
run_command "37S" "34P" ${LABEL}
run_command "38Cl" "34P" ${LABEL}
run_command "39Cl" "34P" ${LABEL}
run_command "40Ar" "34P" ${LABEL}
run_command "41Ar" "34P" ${LABEL}
run_command "31Si" "34P" ${LABEL}
run_command "32Si" "34P" ${LABEL}
run_command "30Al" "34P" ${LABEL}
