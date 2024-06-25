#!/bin/bash

run_command() {
  ./Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}
run_command "51Fe" "52Fe" ${LABEL}
run_command "52Fe" "52Fe" ${LABEL}
run_command "53Fe" "52Fe" ${LABEL}
run_command "52Co" "52Fe" ${LABEL}
run_command "53Co" "52Fe" ${LABEL}
run_command "54Co" "52Fe" ${LABEL}
run_command "53Ni" "52Fe" ${LABEL}
run_command "54Ni" "52Fe" ${LABEL}
run_command "55Ni" "52Fe" ${LABEL}
run_command "56Ni" "52Fe" ${LABEL}
run_command "56Cu" "52Fe" ${LABEL}
run_command "57Cu" "52Fe" ${LABEL}
run_command "58Cu" "52Fe" ${LABEL}
