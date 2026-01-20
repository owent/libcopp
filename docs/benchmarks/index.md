# Benchmarks

This section summarizes the stress test reports captured in `docs/reports` and provides the raw logs for reference.

## How to run

### Windows

```bat
{% include "reports/windows_start_benchmark.bat" %}
```

### Linux

```sh
{% include "reports/linux_start_benchmark.sh" %}
```

## Summary

- Each report measures allocate/create/switch/remove cycles for coroutine contexts and tasks with different stack allocators.
- Use the raw logs below for precise numbers and environment details.
