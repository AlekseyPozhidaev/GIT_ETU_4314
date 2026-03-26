#!/bin/bash

APP="./linux"
TRIES=3

SIZES=(512 1024)

echo "=== AIO benchmark start ==="

for SZ in "${SIZES[@]}"; do

    IN="input_${SZ}M.bin"
    OUT="output_${SZ}M.bin"

    BLOCK_OUT="blocks_${SZ}.dat"
    OPS_OUT="ops_${SZ}.dat"

    echo ""
    echo ">>> Working with ${SZ}MB file"

    # create file if needed
    if [ ! -f "$IN" ]; then
        echo "Generating ${SZ}MB file..."
        dd if=/dev/urandom of="$IN" bs=1M count=$SZ status=none
    fi

    # ---------------- BLOCK TEST ----------------
    echo "block_kb avg_time" > "$BLOCK_OUT"

    OPS_FIXED=4
    BLOCK_SET=(32 128 512 1024 4096 8192)

    echo "-- block size test (ops=$OPS_FIXED)"

    for B in "${BLOCK_SET[@]}"; do
        echo -n "block ${B}KB: "

        TOTAL=0

        for ((run=1; run<=TRIES; run++)); do

            RESULT=$($APP "$IN" "$OUT" "$B" "$OPS_FIXED")

            TIME=$(echo "$RESULT" | awk '/Time:/ {print $3}')

            if [ "$run" -eq 1 ]; then
                cmp -s "$IN" "$OUT" || { echo "FAIL: data mismatch"; exit 1; }
            fi

            printf "%s " "$TIME"
            TOTAL=$(awk "BEGIN {print $TOTAL + $TIME}")
        done

        AVG=$(awk "BEGIN {print $TOTAL / $TRIES}")
        printf " -> avg %.4f\n" "$AVG"

        echo "$B $AVG" >> "$BLOCK_OUT"
    done

    # ---------------- OPS TEST ----------------
    echo ""
    echo "ops avg_time" > "$OPS_OUT"

    BLOCK_FIXED=1024
    OPS_SET=(1 2 4 6 8 12)

    echo "-- concurrency test (block=${BLOCK_FIXED}KB)"

    for O in "${OPS_SET[@]}"; do
        echo -n "ops ${O}: "

        TOTAL=0

        for ((run=1; run<=TRIES; run++)); do

            RESULT=$($APP "$IN" "$OUT" "$BLOCK_FIXED" "$O")
            TIME=$(echo "$RESULT" | awk '/Time:/ {print $3}')

            if [ "$run" -eq 1 ]; then
                cmp -s "$IN" "$OUT" || { echo "FAIL: data mismatch"; exit 1; }
            fi

            printf "%s " "$TIME"
            TOTAL=$(awk "BEGIN {print $TOTAL + $TIME}")
        done

        AVG=$(awk "BEGIN {print $TOTAL / $TRIES}")
        printf " -> avg %.4f\n" "$AVG"

        echo "$O $AVG" >> "$OPS_OUT"
    done

done

echo ""
echo "=== Done ==="
