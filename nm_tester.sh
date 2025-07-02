#!/bin/bash
echo -e "${YELLOW}Starting ft_nm vs nm comparison tests...${NC}"

BIN=(
	"btc"
	"marvin_game"
	"rpn"
	"taskmasterd"
	"woody"
	"famine"
	"PmergeMe"
	"target"
	"webserv"
	"woody_woodpacker"
)
FLAGS=(
	""
	"-a"
	"-g"
	"-u"
	"-D"
	"-p"
	"-r"
	"-U"
	"-n"
	"-to"
	"-tx"
	"-td"
	"-W"
)

OUTDIR=test_res

rm -r $OUTDIR
mkdir -p $OUTDIR

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

total_tests=0
for T in "${BIN[@]}"; do
	for F1 in "${FLAGS[@]}"; do
		for F2 in "${FLAGS[@]}"; do
			if [ "$F1" != "$F2" ]; then
				((total_tests++))
			fi
		done
	done
done

current_test=1
for T in "${BIN[@]}"; do
	TARGET=./targets/$T
	for F1 in "${FLAGS[@]}"; do
		for F2 in "${FLAGS[@]}"; do
			if [ "$F1" != "$F2" ]; then
				COMBO="$F1 $F2"
				echo -e "${YELLOW}[$current_test/$total_tests] Testing $T with flags '$COMBO'...${NC}"
				./ft_nm $COMBO "$TARGET" 2>&1 > $OUTDIR/out.f
				nm $COMBO "$TARGET" 2>&1 > $OUTDIR/out.m

				diff $OUTDIR/out.f $OUTDIR/out.m > $OUTDIR/out.diff
				if [ $? -ne 0 ]; then
					echo -e "${RED}Difference found for $T with flags '$COMBO':${NC}"
					echo -e "./ft_nm $COMBO \"$TARGET\" 2>&1 > $OUTDIR/out.f"
					cat $OUTDIR/out.diff
					cp $OUTDIR/out.f "$OUTDIR/out_${T}_flag_${F// /_}.ftnm"
					cp $OUTDIR/out.m "$OUTDIR/out_${T}_flag_${F// /_}.nm"
				else
					echo -e "${GREEN}No difference for $T with flags '$COMBO'${NC}"
				fi
				((current_test++))
			fi
		done
	done
done

echo -e "${YELLOW}All tests completed.${NC}"