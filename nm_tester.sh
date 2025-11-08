#!/bin/bash
echo -e "${YELLOW}Starting ft_nm vs nm comparison tests...${NC}"

BIN=(
	"marvin_game"
	"famine"
	"ls"
	"target"
	"cat"
	"ssh"
)
FLAGS=(
	""
	"-a"
	"-g"
	"-u"
	"-D"
	"-r"
	"-p"
	# "-U" # Macos only
	"-n"
	"-to"
	"-tx"
	"-td"
	# "-W" # Macos only
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

success_count=0
fail_count=0
current_test=1

# collect failed tests: entries formatted as "TARGET|FLAGS|FT_FILE|NM_FILE"
failed_list=()

for T in "${BIN[@]}"; do
	TARGET=./targets/$T
	for F1 in "${FLAGS[@]}"; do
		for F2 in "${FLAGS[@]}"; do
			if [ "$F1" != "$F2" ]; then
				COMBO="$F1 $F2"
				echo -e "${YELLOW}[$current_test/$total_tests] Testing $T with flags '$COMBO'...${NC}"
				./ft_nm $COMBO "$TARGET" > $OUTDIR/out.f 2>&1
				nm $COMBO "$TARGET" > $OUTDIR/out.m 2>&1

				diff $OUTDIR/out.f $OUTDIR/out.m > $OUTDIR/out.diff
				if [ $? -ne 0 ]; then
					echo -e "${RED}Difference found for $T with flags '$COMBO':${NC}"
					echo -e "./ft_nm $COMBO \"$TARGET\" 2>&1 > $OUTDIR/out.f"
					cat $OUTDIR/out.diff
					SANITIZED=${COMBO// /_}
					FT_FILE="$OUTDIR/out_${T}_flag_${SANITIZED}.ftnm"
					NM_FILE="$OUTDIR/out_${T}_flag_${SANITIZED}.nm"
					cp $OUTDIR/out.f "$FT_FILE"
					cp $OUTDIR/out.m "$NM_FILE"
					((fail_count++))
					failed_list+=("${T}|${COMBO}|${FT_FILE}|${NM_FILE}")
				else
					echo -e "${GREEN}No difference for $T with flags '$COMBO'${NC}"
					((success_count++))
				fi
				((current_test++))
			fi
		done
	done
done

echo -e "${YELLOW}All tests completed.${NC}"
echo -e "${GREEN}Succeeded: ${success_count}${NC}"
echo -e "${RED}Failed: ${fail_count}${NC}"
echo -e "${YELLOW}Total: ${total_tests}${NC}"

if [ "${fail_count}" -ne 0 ]; then
	echo -e "${RED}Failed tests list:${NC}"
	for entry in "${failed_list[@]}"; do
		IFS='|' read -r t combo ftfile nmfile <<< "$entry"
		echo -e " - ${t} with flags '${combo}' -> saved: ${ftfile} , ${nmfile}"
	done
fi