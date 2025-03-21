#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <program>"
    exit 1
fi

# Start the program in the background
"$@" &
PID=$!

echo "📢 Started: $1 (PID: $PID)"
echo "⏳ Monitoring memory usage..."

# Monitor memory usage while the process is running
while kill -0 "$PID" 2>/dev/null; do
    MEM_USAGE=$(ps -o rss= -p "$PID" | tr -d ' ') # RSS (Resident Set Size) memory usage in KB
    VSZ_USAGE=$(ps -o vsz= -p "$PID" | tr -d ' ') # VSZ (Virtual Memory Size) memory usage in KB

    # Convert KB to MB and format numbers with thousands separator
    MEM_USAGE_MB=$(echo "scale=2; $MEM_USAGE / 1024" | bc)
    VSZ_USAGE_MB=$(echo "scale=2; $VSZ_USAGE / 1024" | bc)
    CPU_USAGE=$(ps -o %cpu= -p "$PID" | tr -d ' ') # CPU usage percentage

    printf "%s | PID: %d | CPU: %5.1f%% | RAM: %'d KB (%.2f MB) | Virtual Memory: %'d KB (%.2f MB)\n" \
        "$(date +"%H:%M:%S")" "$PID" "$CPU_USAGE" "$MEM_USAGE" "$MEM_USAGE_MB" "$VSZ_USAGE" "$VSZ_USAGE_MB"

    sleep 1
done

echo "🚀 Process ($PID) has finished."
