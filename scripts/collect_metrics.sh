#!/usr/bin/env bash
# Simple collector to build and run selected examples and emit CSV + Markdown tables (macOS/Linux)
# Requires: clang++/g++, objdump/size, awk, sed
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
OUT_DIR="$ROOT_DIR/metrics"
mkdir -p "$OUT_DIR"
CSV="$OUT_DIR/results.csv"
MD="$OUT_DIR/README.md"

# Header
printf "module,file,binary,metric,value,unit\n" > "$CSV"

# Helper: compute .rodata size for a binary (ELF/Mach-O)
rodata_size_for() {
  local bin="$1"
  local sum=0
  # Try ELF objdump first
  local hex_list
  hex_list=$(objdump -h "$bin" 2>/dev/null | awk '/\.rodata/{print $3}') || true
  if [[ -n "$hex_list" ]]; then
    for hx in $hex_list; do
      hx=${hx#0x}
      [[ -n "$hx" ]] && sum=$((sum + 16#$hx))
    done
    echo "$sum"
    return 0
  fi
  # Mach-O fallback via otool: sum __cstring and __const sizes
  if command -v otool >/dev/null 2>&1; then
    local mac_sum=0
    while IFS= read -r hx; do
      hx=${hx#0x}
      [[ -n "$hx" ]] && mac_sum=$((mac_sum + 16#$hx))
    done < <(otool -l "$bin" 2>/dev/null | awk '/sectname __cstring|sectname __const/{f=1} f&&$1=="size"{print $2; f=0}')
    echo "$mac_sum"
    return 0
  fi
  echo 0
}

# Helper: file size in bytes (portable)
file_size_bytes() {
  local bin="$1"
  wc -c < "$bin" | tr -d ' '
}

echo "Building and running performance_test.cpp..."
SRC="$ROOT_DIR/04_Uygulamalar/src/performance_test.cpp"
PERF_DIR="$OUT_DIR/performance_test"
mkdir -p "$PERF_DIR"
BIN="$PERF_DIR/perf_test"
clang++ -std=c++11 -O2 -o "$BIN" "$SRC"

# Run and capture
RUN_OUT="$PERF_DIR/perf_test.out"
"$BIN" > "$RUN_OUT" 2>&1 || true

# Parse timings and metrics (robust)
copy_time=$(awk 'f&&/microseconds/{gsub(/[^0-9]/,"");print;exit} /String Copy Approach/{f=1}' "$RUN_OUT")
pool_time=$(awk 'f&&/microseconds/{gsub(/[^0-9]/,"");print;exit} /Constant Pool Approach/{f=1}' "$RUN_OUT")

strcmp_time=$(grep -E "strcmp süresi" "$RUN_OUT" | tail -n1 | sed -E 's/[^0-9]*([0-9]+).*/\1/' || true)
pointer_time=$(grep -E "Pointer süresi" "$RUN_OUT" | tail -n1 | sed -E 's/[^0-9]*([0-9]+).*/\1/' || true)

# Size info
SIZE_TXT="$PERF_DIR/perf_test.size"
size "$BIN" > "$SIZE_TXT" || true
rodata_bytes=$(rodata_size_for "$BIN")
# macOS Mach-O fallback: sum __cstring and __const using otool without awk strtonum
if [[ -z "$rodata_bytes" || "$rodata_bytes" == "0" ]]; then
  if command -v otool >/dev/null 2>&1; then
    hex_sizes=$(otool -l "$BIN" 2>/dev/null | awk '/sectname __cstring|sectname __const/{f=1} f && $1=="size"{print $2; f=0}')
    sum=0
    while IFS= read -r hx; do
      hx=${hx#0x}
      if [[ -n "$hx" ]]; then
        val=$((16#$hx))
        sum=$((sum + val))
      fi
    done <<< "$hex_sizes"
    rodata_bytes="$sum"
  fi
fi

# CSV rows
[[ -n "$copy_time" ]] && printf "Uygulamalar,performance_test.cpp,perf_test,copy_duration,%s,microseconds\n" "$copy_time" >> "$CSV"
[[ -n "$pool_time" ]] && printf "Uygulamalar,performance_test.cpp,perf_test,pool_duration,%s,microseconds\n" "$pool_time" >> "$CSV"
[[ -n "$strcmp_time" ]] && printf "Uygulamalar,performance_test.cpp,perf_test,strcmp_duration,%s,microseconds\n" "$strcmp_time" >> "$CSV"
[[ -n "$pointer_time" ]] && printf "Uygulamalar,performance_test.cpp,perf_test,pointer_duration,%s,microseconds\n" "$pointer_time" >> "$CSV"
[[ -n "$rodata_bytes" ]] && printf "Uygulamalar,performance_test.cpp,perf_test,rodata_size,%s,bytes\n" "$rodata_bytes" >> "$CSV"

# optimization_analysis
echo "Building and running optimization_analysis.cpp..."
SRC2="$ROOT_DIR/03_Ileri_Kavramlar/examples/optimization_analysis.cpp"
OPT_DIR="$OUT_DIR/optimization_analysis"
mkdir -p "$OPT_DIR"
BIN2="$OPT_DIR/optimization_analysis"
clang++ -std=c++11 -O2 -o "$BIN2" "$SRC2"
RUN2_OUT="$OPT_DIR/optimization_analysis.out"
"$BIN2" > "$RUN2_OUT" 2>&1 || true

# Parse perf table lines (average per op)
avg_direct=$(grep -E "Direct literal access:" "$RUN2_OUT" | sed -E 's/.*: +([0-9.]+) ns/\1/')
avg_func=$(grep -E "Function call:" "$RUN2_OUT" | sed -E 's/.*: +([0-9.]+) ns/\1/')
avg_tmpl=$(grep -E "Template call:" "$RUN2_OUT" | sed -E 's/.*: +([0-9.]+) ns/\1/')
[[ -n "$avg_direct" ]] && printf "Ileri,optimization_analysis.cpp,optimization_analysis,avg_direct,%s,ns\n" "$avg_direct" >> "$CSV"
[[ -n "$avg_func" ]] && printf "Ileri,optimization_analysis.cpp,optimization_analysis,avg_function,%s,ns\n" "$avg_func" >> "$CSV"
[[ -n "$avg_tmpl" ]] && printf "Ileri,optimization_analysis.cpp,optimization_analysis,avg_template,%s,ns\n" "$avg_tmpl" >> "$CSV"

# Duplicate test: collect addresses equality
echo "Building duplicate_test.cpp..."
SRC3="$ROOT_DIR/02_Temel_Kavramlar/examples/duplicate_test.cpp"
DUP_DIR="$OUT_DIR/duplicate_test"
mkdir -p "$DUP_DIR"
BIN3="$DUP_DIR/duplicate_test"
clang++ -std=c++11 -O2 -o "$BIN3" "$SRC3"
RUN3_OUT="$DUP_DIR/duplicate_test.out"
"$BIN3" > "$RUN3_OUT" 2>&1 || true
same_addr=$(grep -E "SUCCESS: str1 and str2 point to the same" "$RUN3_OUT" | wc -l | tr -d ' ')
printf "Temel,duplicate_test.cpp,duplicate_test,literal_pooling_worked,%s,boolean\n" "$same_addr" >> "$CSV"

# performance_benchmark.cpp
echo "Building and running performance_benchmark.cpp..."
SRC4="$ROOT_DIR/03_Ileri_Kavramlar/examples/performance_benchmark.cpp"
PB_DIR="$OUT_DIR/performance_benchmark"
mkdir -p "$PB_DIR"
BIN4="$PB_DIR/perf_bench"
clang++ -std=c++11 -O2 -o "$BIN4" "$SRC4"
RUN4_OUT="$PB_DIR/perf_bench.out"
"$BIN4" > "$RUN4_OUT" 2>&1 || true
rodata_dur=$(grep -E "\.rodata approach:\s+[0-9]+\s+microseconds" "$RUN4_OUT" | sed -E 's/.*: ([0-9]+) microseconds/\1/' | tail -n1 || true)
inline_dur=$(grep -E "Inline simulation:\s+[0-9]+\s+microseconds" "$RUN4_OUT" | sed -E 's/.*: ([0-9]+) microseconds/\1/' | tail -n1 || true)
speedup_bench=$(grep -E "Speedup:\s+([0-9.]+)x" "$RUN4_OUT" | sed -E 's/.*Speedup: ([0-9.]+)x.*/\1/' | tail -n1 || true)
[[ -n "$rodata_dur" ]] && printf "Ileri,performance_benchmark.cpp,perf_bench,rodata_duration,%s,microseconds\n" "$rodata_dur" >> "$CSV"
[[ -n "$inline_dur" ]] && printf "Ileri,performance_benchmark.cpp,perf_bench,inline_duration,%s,microseconds\n" "$inline_dur" >> "$CSV"
[[ -n "$speedup_bench" ]] && printf "Ileri,performance_benchmark.cpp,perf_bench,speedup,%s,x\n" "$speedup_bench" >> "$CSV"

# memory_layout_analysis.cpp
echo "Building and running memory_layout_analysis.cpp..."
SRC5="$ROOT_DIR/03_Ileri_Kavramlar/examples/memory_layout_analysis.cpp"
ML_DIR="$OUT_DIR/memory_layout"
mkdir -p "$ML_DIR"
BIN5="$ML_DIR/memory_layout"
clang++ -std=c++11 -O1 -o "$BIN5" "$SRC5" || true
if [[ -x "$BIN5" ]]; then
  RUN5_OUT="$ML_DIR/memory_layout.out"
  "$BIN5" > "$RUN5_OUT" 2>&1 || true
  addr_span=$(grep -E "Address span:\s+[0-9]+ bytes" "$RUN5_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
  cache_lines=$(grep -E "Cache lines used .*:\s+[0-9]+" "$RUN5_OUT" | sed -E 's/.*: ([0-9]+)/\1/' | tail -n1)
  sso_size=$(grep -E "sizeof\(std::string\):\s+[0-9]+ bytes" "$RUN5_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
  pooling_ok_adv=$(grep -E "literal1 == literal2: YES" "$RUN5_OUT" | wc -l | tr -d ' ')
  [[ -n "$addr_span" ]] && printf "Ileri,memory_layout_analysis.cpp,memory_layout,address_span,%s,bytes\n" "$addr_span" >> "$CSV"
  [[ -n "$cache_lines" ]] && printf "Ileri,memory_layout_analysis.cpp,memory_layout,cache_lines,%s,count\n" "$cache_lines" >> "$CSV"
  [[ -n "$sso_size" ]] && printf "Ileri,memory_layout_analysis.cpp,memory_layout,sizeof_std_string,%s,bytes\n" "$sso_size" >> "$CSV"
  printf "Ileri,memory_layout_analysis.cpp,memory_layout,literal_pooling_worked,%s,boolean\n" "$pooling_ok_adv" >> "$CSV"
else
  echo "Skipping memory_layout_analysis (build failed)"
fi

# assembly_analysis.cpp (sizes only)
echo "Building assembly_analysis.cpp..."
SRC6="$ROOT_DIR/02_Temel_Kavramlar/examples/assembly_analysis.cpp"
ASM_DIR="$OUT_DIR/assembly_analysis"
mkdir -p "$ASM_DIR"
BIN6="$ASM_DIR/assembly_test"
clang++ -std=c++11 -O1 -o "$BIN6" "$SRC6"
asm_size=$(file_size_bytes "$BIN6")
asm_ro=$(rodata_size_for "$BIN6")
[[ -n "$asm_size" ]] && printf "Temel,assembly_analysis.cpp,assembly_test,binary_size,%s,bytes\n" "$asm_size" >> "$CSV"
[[ -n "$asm_ro" ]] && printf "Temel,assembly_analysis.cpp,assembly_test,rodata_size,%s,bytes\n" "$asm_ro" >> "$CSV"

# size_comparison.cpp across -O levels
echo "Building size_comparison.cpp at multiple -O levels..."
SRC7="$ROOT_DIR/02_Temel_Kavramlar/examples/size_comparison.cpp"
SIZE_DIR="$OUT_DIR/size_comparison"
mkdir -p "$SIZE_DIR"
for opt in O0 O2 Os; do
  SUB="$SIZE_DIR/$opt"
  mkdir -p "$SUB"
  BIN7="$SUB/size_test_$opt"
  clang++ -std=c++11 -$opt -o "$BIN7" "$SRC7"
  fsize=$(file_size_bytes "$BIN7")
  ros=$(rodata_size_for "$BIN7")
  printf "Temel,size_comparison.cpp,size_test_%s,binary_size,%s,bytes\n" "$opt" "$fsize" >> "$CSV"
  printf "Temel,size_comparison.cpp,size_test_%s,rodata_size,%s,bytes\n" "$opt" "$ros" >> "$CSV"
done
# Count 'Repeated message' occurrences in strings (may be deduped to 1)
rep_cnt=$(strings "$SIZE_DIR/O2/size_test_O2" 2>/dev/null | grep -c "Repeated message" || true)
printf "Temel,size_comparison.cpp,size_test_O2,repeated_message_strings,%s,count\n" "$rep_cnt" >> "$CSV"

# embedded_systems.cpp (will run ~4s)
echo "Building and running embedded_systems.cpp..."
SRC8="$ROOT_DIR/04_Uygulamalar/src/embedded_systems.cpp"
EMB_DIR="$OUT_DIR/embedded_systems"
mkdir -p "$EMB_DIR"
BIN8="$EMB_DIR/embedded_sim"
clang++ -std=c++11 -O2 -pthread -o "$BIN8" "$SRC8"
RUN8_OUT="$EMB_DIR/embedded_sim.out"
"$BIN8" > "$RUN8_OUT" 2>&1 || true
ram_bad=$(grep -E "RAM used by bad system:\s+[0-9]+ bytes" "$RUN8_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
ram_good=$(grep -E "RAM used by good system:\s+[0-9]+ bytes" "$RUN8_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
rom_use=$(grep -E "ROM usage:\s+[0-9]+ bytes" "$RUN8_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
net_benefit=$(grep -E "Net benefit:\s+[0-9]+ bytes saved" "$RUN8_OUT" | sed -E 's/.*: ([0-9]+) bytes.*/\1/' | tail -n1)
final_ram=$(grep -E "Final RAM usage:\s+[0-9]+ bytes" "$RUN8_OUT" | sed -E 's/.*: ([0-9]+) bytes/\1/' | tail -n1)
[[ -n "$ram_bad" ]] && printf "Uygulamalar,embedded_systems.cpp,embedded_sim,ram_bad,%s,bytes\n" "$ram_bad" >> "$CSV"
[[ -n "$ram_good" ]] && printf "Uygulamalar,embedded_systems.cpp,embedded_sim,ram_good,%s,bytes\n" "$ram_good" >> "$CSV"
[[ -n "$rom_use" ]] && printf "Uygulamalar,embedded_systems.cpp,embedded_sim,rom_usage,%s,bytes\n" "$rom_use" >> "$CSV"
[[ -n "$net_benefit" ]] && printf "Uygulamalar,embedded_systems.cpp,embedded_sim,net_benefit,%s,bytes\n" "$net_benefit" >> "$CSV"
[[ -n "$final_ram" ]] && printf "Uygulamalar,embedded_systems.cpp,embedded_sim,final_ram,%s,bytes\n" "$final_ram" >> "$CSV"

# game_performance.cpp
echo "Building and running game_performance.cpp..."
SRC9="$ROOT_DIR/04_Uygulamalar/src/game_performance.cpp"
GAME_DIR="$OUT_DIR/game_performance"
mkdir -p "$GAME_DIR"
BIN9="$GAME_DIR/game_perf"
clang++ -std=c++11 -O2 -pthread -o "$BIN9" "$SRC9"
RUN9_OUT="$GAME_DIR/game_perf.out"
"$BIN9" > "$RUN9_OUT" 2>&1 || true
# Inefficient
inf_frame=$(grep -E "Inefficient System Results:|Inefficient System Results" -n "$RUN9_OUT" | head -n1 | cut -d: -f1)
if [[ -n "$inf_frame" ]]; then
  l=$((inf_frame+1)); avg_inf=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9.]+) ms/\1/')
  l=$((inf_frame+2)); fps_inf=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((inf_frame+3)); alloc_inf=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((inf_frame+4)); cache_inf=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  [[ -n "$avg_inf" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,inefficient_avg_frame,%s,ms\n" "$avg_inf" >> "$CSV"
  [[ -n "$fps_inf" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,inefficient_fps,%s,count\n" "$fps_inf" >> "$CSV"
  [[ -n "$alloc_inf" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,inefficient_string_allocs,%s,count\n" "$alloc_inf" >> "$CSV"
  [[ -n "$cache_inf" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,inefficient_cache_lines,%s,count\n" "$cache_inf" >> "$CSV"
fi
# Optimized
opt_line=$(grep -n "Optimized System Results:" "$RUN9_OUT" | tail -n1 | cut -d: -f1)
if [[ -n "$opt_line" ]]; then
  l=$((opt_line+1)); avg_opt=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9.]+) ms/\1/')
  l=$((opt_line+2)); fps_opt=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((opt_line+3)); alloc_opt=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((opt_line+4)); cache_opt=$(sed -n "${l}p" "$RUN9_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  [[ -n "$avg_opt" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,optimized_avg_frame,%s,ms\n" "$avg_opt" >> "$CSV"
  [[ -n "$fps_opt" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,optimized_fps,%s,count\n" "$fps_opt" >> "$CSV"
  [[ -n "$alloc_opt" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,optimized_string_allocs,%s,count\n" "$alloc_opt" >> "$CSV"
  [[ -n "$cache_opt" ]] && printf "Uygulamalar,game_performance.cpp,game_perf,optimized_cache_lines,%s,count\n" "$cache_opt" >> "$CSV"
fi

# web_server.cpp
echo "Building and running web_server.cpp..."
SRC10="$ROOT_DIR/04_Uygulamalar/src/web_server.cpp"
WEB_DIR="$OUT_DIR/web_server"
mkdir -p "$WEB_DIR"
BIN10="$WEB_DIR/web_server"
clang++ -std=c++11 -O2 -pthread -o "$BIN10" "$SRC10"
RUN10_OUT="$WEB_DIR/web_server.out"
"$BIN10" > "$RUN10_OUT" 2>&1 || true
# Inefficient test
inf_time=$(grep -E "Total test time:\s+[0-9]+ms" "$RUN10_OUT" | head -n1 | sed -E 's/.*: ([0-9]+)ms/\1/')
inf_req=$(grep -n "Inefficient Web Server Performance Stats:" "$RUN10_OUT" | head -n1 | cut -d: -f1)
if [[ -n "$inf_req" ]]; then
  l=$((inf_req+1)); rqs=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((inf_req+2)); alloc=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((inf_req+3)); avgms=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)ms/\1/')
  l=$((inf_req+4)); hit=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9.]+)%/\1/')
  [[ -n "$inf_time" ]] && printf "Uygulamalar,web_server.cpp,web_server,inefficient_total_time,%s,ms\n" "$inf_time" >> "$CSV"
  [[ -n "$rqs" ]] && printf "Uygulamalar,web_server.cpp,web_server,inefficient_requests,%s,count\n" "$rqs" >> "$CSV"
  [[ -n "$alloc" ]] && printf "Uygulamalar,web_server.cpp,web_server,inefficient_allocations,%s,count\n" "$alloc" >> "$CSV"
  [[ -n "$avgms" ]] && printf "Uygulamalar,web_server.cpp,web_server,inefficient_avg_ms,%s,ms\n" "$avgms" >> "$CSV"
  [[ -n "$hit" ]] && printf "Uygulamalar,web_server.cpp,web_server,inefficient_cache_hit_rate,%s,percent\n" "$hit" >> "$CSV"
fi
# Optimized test
opt_time=$(grep -E "Total test time:\s+[0-9]+ms" "$RUN10_OUT" | sed -n '2p' | sed -E 's/.*: ([0-9]+)ms/\1/')
opt_req=$(grep -n "Optimized Web Server Performance Stats:" "$RUN10_OUT" | head -n1 | cut -d: -f1)
if [[ -n "$opt_req" ]]; then
  l=$((opt_req+1)); rqs=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((opt_req+2)); alloc=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((opt_req+3)); avgms=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)ms/\1/')
  l=$((opt_req+4)); hit=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9.]+)%/\1/')
  [[ -n "$opt_time" ]] && printf "Uygulamalar,web_server.cpp,web_server,optimized_total_time,%s,ms\n" "$opt_time" >> "$CSV"
  [[ -n "$rqs" ]] && printf "Uygulamalar,web_server.cpp,web_server,optimized_requests,%s,count\n" "$rqs" >> "$CSV"
  [[ -n "$alloc" ]] && printf "Uygulamalar,web_server.cpp,web_server,optimized_allocations,%s,count\n" "$alloc" >> "$CSV"
  [[ -n "$avgms" ]] && printf "Uygulamalar,web_server.cpp,web_server,optimized_avg_ms,%s,ms\n" "$avgms" >> "$CSV"
  [[ -n "$hit" ]] && printf "Uygulamalar,web_server.cpp,web_server,optimized_cache_hit_rate,%s,percent\n" "$hit" >> "$CSV"
fi
# Pooled concurrent test
pool_time=$(grep -E "Concurrent test completed in:\s+[0-9]+ms" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)ms/\1/' | tail -n1)
pool_req=$(grep -n "Connection Pooled Server Performance Stats:" "$RUN10_OUT" | head -n1 | cut -d: -f1)
if [[ -n "$pool_req" ]]; then
  l=$((pool_req+1)); rqs=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((pool_req+2)); alloc=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)/\1/')
  l=$((pool_req+3)); avgms=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9]+)ms/\1/')
  l=$((pool_req+4)); hit=$(sed -n "${l}p" "$RUN10_OUT" | sed -E 's/.*: ([0-9.]+)%/\1/')
  [[ -n "$pool_time" ]] && printf "Uygulamalar,web_server.cpp,web_server,pooled_total_time,%s,ms\n" "$pool_time" >> "$CSV"
  [[ -n "$rqs" ]] && printf "Uygulamalar,web_server.cpp,web_server,pooled_requests,%s,count\n" "$rqs" >> "$CSV"
  [[ -n "$alloc" ]] && printf "Uygulamalar,web_server.cpp,web_server,pooled_allocations,%s,count\n" "$alloc" >> "$CSV"
  [[ -n "$avgms" ]] && printf "Uygulamalar,web_server.cpp,web_server,pooled_avg_ms,%s,ms\n" "$avgms" >> "$CSV"
  [[ -n "$hit" ]] && printf "Uygulamalar,web_server.cpp,web_server,pooled_cache_hit_rate,%s,percent\n" "$hit" >> "$CSV"
fi

# Emit Markdown summary
{
  echo "# Metrics Summary"
  echo
  echo "Generated: $(date)"
  echo
  echo "## Results (CSV)"
  echo
  echo '```'
  column -s, -t < "$CSV"
  echo '```'
} > "$MD"

echo "Done. See $CSV and $MD"