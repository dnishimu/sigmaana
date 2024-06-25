#!/bin/bash                                                                                                                    

# Basic setup                                                                                                      
                                                                                                                               
# Check if two arguments are provided                                                            
                                                                                                                               
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_37P.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "34P" "37P" "${LABEL}"
run_command "35P" "37P" "${LABEL}"
run_command "36P" "37P" "${LABEL}"
run_command "37P" "37P" "${LABEL}"
run_command "37S" "37P" "${LABEL}"
run_command "38S" "37P" "${LABEL}"
run_command "39S" "37P" "${LABEL}"
run_command "40S" "37P" "${LABEL}"
run_command "39Cl" "37P" "${LABEL}"
run_command "40Cl" "37P" "${LABEL}"
run_command "41Cl" "37P" "${LABEL}"
run_command "42Cl" "37P" "${LABEL}"
run_command "42Ar" "37P" "${LABEL}"
run_command "43Ar" "37P" "${LABEL}"
run_command "33Si" "37P" "${LABEL}"
run_command "34Si" "37P" "${LABEL}"
run_command "35Si" "37P" "${LABEL}"
run_command "31Al" "37P" "${LABEL}"
run_command "32Al" "37P" "${LABEL}"
run_command "33Al" "37P" "${LABEL}"
