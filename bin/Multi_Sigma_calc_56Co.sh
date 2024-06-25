#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "53Fe" "56Co" ${LABEL}
run_command "54Fe" "56Co" ${LABEL}
run_command "55Fe" "56Co" ${LABEL}
run_command "54Co" "56Co" ${LABEL}
run_command "55Co" "56Co" ${LABEL}
run_command "56Co" "56Co" ${LABEL}
run_command "56Ni" "56Co" ${LABEL}
run_command "57Ni" "56Co" ${LABEL}
run_command "58Ni" "56Co" ${LABEL}
run_command "58Cu" "56Co" ${LABEL}
run_command "59Cu" "56Co" ${LABEL}
run_command "60Cu" "56Co" ${LABEL}
run_command "60Zn" "56Co" ${LABEL}
run_command "61Zn" "56Co" ${LABEL}
