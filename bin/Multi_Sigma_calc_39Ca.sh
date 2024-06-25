#!/bin/bash                                                                                                                                                                   

# Basic setup                                                                                                                
                                                                                                                                                                  
# Check if two arguments are provided                                                                                                
                                                                                                                                                 
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 LABEL"
    echo "Example ./bin/Multi_Sigma_calc_39Ca.sh \"1\" "
    echo "You can use any LABEL as you like"
    exit 1
fi
LABEL="$1"
run_command() {
  ./bin/Batch_Sigma_calc_spe.sh "$1" "$2" "$3"
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
