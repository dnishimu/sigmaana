#!/bin/bash                                                                                                   

# Basic setup                                                                    
                                                                                 
# Check if two arguments are provided                                             
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_56Co.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
}

run_command "53Fe" "56Co" "${LABEL}"
run_command "54Fe" "56Co" "${LABEL}"
run_command "55Fe" "56Co" "${LABEL}"
run_command "54Co" "56Co" "${LABEL}"
run_command "55Co" "56Co" "${LABEL}"
run_command "56Co" "56Co" "${LABEL}"
run_command "56Ni" "56Co" "${LABEL}"
run_command "57Ni" "56Co" "${LABEL}"
run_command "58Ni" "56Co" "${LABEL}"
run_command "58Cu" "56Co" "${LABEL}"
run_command "59Cu" "56Co" "${LABEL}"
run_command "60Cu" "56Co" "${LABEL}"
run_command "60Zn" "56Co" "${LABEL}"
run_command "61Zn" "56Co" "${LABEL}"
