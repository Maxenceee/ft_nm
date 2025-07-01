#!/bin/bash
echo "${YELLOW}Starting ft_nm vs nm comparison tests...${NC}"

BIN=("btc" "marvin_game" "rpn" "taskmasterd" "woody" "famine" "PmergeMe" "target" "webserv" "woody_woodpacker")
FLAGS=("" "-a" "-g" "-u")

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

for T in "${BIN[@]}"; do
	TARGET=./targets/$T
	for F in "${FLAGS[@]}"; do
		echo "${YELLOW}Testing $T with flag '$F'...${NC}"
		./ft_nm $F "$TARGET" 2>&1 | sort > out.f
		nm $F "$TARGET" 2>&1 | sort > out.m

		diff out.f out.m > out.diff
		if [ $? -ne 0 ]; then
			echo "${RED}Difference found for $T with flag '$F':${NC}"
			echo "./ft_nm $F "$TARGET" 2>&1 | sort > out.f"
			cat out.diff
			exit
		else
			echo "${GREEN}No difference for $T with flag '$F'${NC}"
		fi
	done
done

# Mix all combinations of two flags
for T in "${BIN[@]}"; do
	TARGET=./targets/$T
	for F1 in "${FLAGS[@]}"; do
		for F2 in "${FLAGS[@]}"; do
			if [ "$F1" != "$F2" ]; then
				COMBO="$F1 $F2"
				echo "${YELLOW}Testing $T with flags '$COMBO'...${NC}"
				./ft_nm $COMBO "$TARGET" 2>&1 | sort > out.f
				nm $COMBO "$TARGET" 2>&1 | sort > out.m

				diff out.f out.m > out.diff
				if [ $? -ne 0 ]; then
					echo "${RED}Difference found for $T with flags '$COMBO':${NC}"
					echo "./ft_nm $COMBO \"$TARGET\" 2>&1 | sort > out.f"
					cat out.diff
					exit
				else
					echo "${GREEN}No difference for $T with flags '$COMBO'${NC}"
				fi
			fi
		done
	done
done

echo "${YELLOW}All tests completed.${NC}"