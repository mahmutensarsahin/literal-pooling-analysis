# Metrics Summary

Generated: Fri Sep 19 18:56:56 +03 2025

## Results (CSV)

```
module       file                        binary                 metric                      value       unit
Uygulamalar  performance_test.cpp        perf_test              copy_duration               330495      microseconds
Uygulamalar  performance_test.cpp        perf_test              pool_duration               85201       microseconds
Uygulamalar  performance_test.cpp        perf_test              strcmp_duration             1202906     microseconds
Uygulamalar  performance_test.cpp        perf_test              pointer_duration            165421      microseconds
Uygulamalar  performance_test.cpp        perf_test              rodata_size                 1909        bytes
Ileri        optimization_analysis.cpp   optimization_analysis  avg_direct                  0.00        ns
Ileri        optimization_analysis.cpp   optimization_analysis  avg_function                0.00        ns
Ileri        optimization_analysis.cpp   optimization_analysis  avg_template                0.00        ns
Temel        duplicate_test.cpp          duplicate_test         literal_pooling_worked      1           boolean
Ileri        performance_benchmark.cpp   perf_bench             rodata_duration             0           microseconds
Ileri        performance_benchmark.cpp   perf_bench             inline_duration             0           microseconds
Ileri        memory_layout_analysis.cpp  memory_layout          address_span                150         bytes
Ileri        memory_layout_analysis.cpp  memory_layout          cache_lines                 3           count
Ileri        memory_layout_analysis.cpp  memory_layout          sizeof_std_string           24          bytes
Ileri        memory_layout_analysis.cpp  memory_layout          literal_pooling_worked      1           boolean
Temel        assembly_analysis.cpp       assembly_test          binary_size                 37488       bytes
Temel        assembly_analysis.cpp       assembly_test          rodata_size                 543         bytes
Temel        size_comparison.cpp         size_test_O0           binary_size                 57088       bytes
Temel        size_comparison.cpp         size_test_O0           rodata_size                 518         bytes
Temel        size_comparison.cpp         size_test_O2           binary_size                 53648       bytes
Temel        size_comparison.cpp         size_test_O2           rodata_size                 522         bytes
Temel        size_comparison.cpp         size_test_Os           binary_size                 53744       bytes
Temel        size_comparison.cpp         size_test_Os           rodata_size                 530         bytes
Temel        size_comparison.cpp         size_test_O2           repeated_message_strings    1           count
Uygulamalar  embedded_systems.cpp        embedded_sim           ram_bad                     768         bytes
Uygulamalar  embedded_systems.cpp        embedded_sim           ram_good                    0           bytes
Uygulamalar  embedded_systems.cpp        embedded_sim           rom_usage                   744         bytes
Uygulamalar  embedded_systems.cpp        embedded_sim           net_benefit                 396         bytes
Uygulamalar  embedded_systems.cpp        embedded_sim           final_ram                   0           bytes
Uygulamalar  game_performance.cpp        game_perf              inefficient_avg_frame       0.00937963  ms
Uygulamalar  game_performance.cpp        game_perf              inefficient_fps             106614      count
Uygulamalar  game_performance.cpp        game_perf              inefficient_string_allocs   100000      count
Uygulamalar  game_performance.cpp        game_perf              inefficient_cache_lines     11          count
Uygulamalar  game_performance.cpp        game_perf              optimized_avg_frame         0.00614783  ms
Uygulamalar  game_performance.cpp        game_perf              optimized_fps               162658      count
Uygulamalar  game_performance.cpp        game_perf              optimized_string_allocs     0           count
Uygulamalar  game_performance.cpp        game_perf              optimized_cache_lines       9           count
Uygulamalar  web_server.cpp              web_server             inefficient_total_time      1320        ms
Uygulamalar  web_server.cpp              web_server             inefficient_requests        10000       count
Uygulamalar  web_server.cpp              web_server             inefficient_allocations     10000       count
Uygulamalar  web_server.cpp              web_server             inefficient_avg_ms          0           ms
Uygulamalar  web_server.cpp              web_server             inefficient_cache_hit_rate  0           percent
Uygulamalar  web_server.cpp              web_server             optimized_total_time        668         ms
Uygulamalar  web_server.cpp              web_server             optimized_requests          10000       count
Uygulamalar  web_server.cpp              web_server             optimized_allocations       0           count
Uygulamalar  web_server.cpp              web_server             optimized_avg_ms            0           ms
Uygulamalar  web_server.cpp              web_server             optimized_cache_hit_rate    83.3333     percent
Uygulamalar  web_server.cpp              web_server             pooled_total_time           19          ms
Uygulamalar  web_server.cpp              web_server             pooled_requests             10000       count
Uygulamalar  web_server.cpp              web_server             pooled_allocations          0           count
Uygulamalar  web_server.cpp              web_server             pooled_avg_ms               0           ms
Uygulamalar  web_server.cpp              web_server             pooled_cache_hit_rate       0           percent
```
