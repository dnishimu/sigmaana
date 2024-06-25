#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "51Fe" "54Co" ${LABEL}
run_command "52Fe" "54Co" ${LABEL}
run_command "53Fe" "54Co" ${LABEL}
run_command "53Co" "54Co" ${LABEL}
run_command "54Co" "54Co" ${LABEL}
run_command "53Ni" "54Co" ${LABEL}
run_command "54Ni" "54Co" ${LABEL}
run_command "55Ni" "54Co" ${LABEL}
run_command "56Ni" "54Co" ${LABEL}
run_command "56Cu" "54Co" ${LABEL}
run_command "57Cu" "54Co" ${LABEL}
run_command "58Cu" "54Co" ${LABEL}
