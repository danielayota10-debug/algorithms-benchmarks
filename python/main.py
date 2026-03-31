#!/usr/bin/env python3
"""
Sorting Algorithm Benchmark — Python
Supports: Insertion, Bubble, Selection, Merge, Quick, Heap sort
"""

import copy
import random
import sys
import time

#  ANSI colors

R = "\033[0m"
BOLD = "\033[1m"
DIM = "\033[2m"
CYAN = "\033[36m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
RED = "\033[31m"
WHITE = "\033[97m"

#  Sorting algorithms


def insertion_sort(arr: list[int]) -> None:
    for i in range(1, len(arr)):
        key, j = arr[i], i - 1
        while j >= 0 and arr[j] > key:
            arr[j + 1] = arr[j]
            j -= 1
        arr[j + 1] = key


def bubble_sort(arr: list[int]) -> None:
    n = len(arr)
    for i in range(n - 1):
        for j in range(n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]


def selection_sort(arr: list[int]) -> None:
    n = len(arr)
    for i in range(n - 1):
        mn = min(range(i, n), key=arr.__getitem__)
        arr[i], arr[mn] = arr[mn], arr[i]


def merge_sort(arr: list[int]) -> None:
    if len(arr) <= 1:
        return
    mid = len(arr) // 2
    left, right = arr[:mid], arr[mid:]
    merge_sort(left)
    merge_sort(right)
    i = j = k = 0
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            arr[k] = left[i]
            i += 1
        else:
            arr[k] = right[j]
            j += 1
        k += 1
    while i < len(left):
        arr[k] = left[i]
        i += 1
        k += 1
    while j < len(right):
        arr[k] = right[j]
        j += 1
        k += 1


def _quick_sort(arr: list[int], low: int, high: int) -> None:
    if low >= high:
        return
    pivot, i = arr[high], low - 1
    for j in range(low, high):
        if arr[j] <= pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    pi = i + 1
    _quick_sort(arr, low, pi - 1)
    _quick_sort(arr, pi + 1, high)


def quick_sort(arr: list[int]) -> None:
    sys.setrecursionlimit(max(10_000, len(arr) * 2))
    _quick_sort(arr, 0, len(arr) - 1)


def _heapify(arr: list[int], n: int, i: int) -> None:
    largest, l, r = i, 2 * i + 1, 2 * i + 2
    if l < n and arr[l] > arr[largest]:
        largest = l
    if r < n and arr[r] > arr[largest]:
        largest = r
    if largest != i:
        arr[i], arr[largest] = arr[largest], arr[i]
        _heapify(arr, n, largest)


def heap_sort(arr: list[int]) -> None:
    n = len(arr)
    for i in range(n // 2 - 1, -1, -1):
        _heapify(arr, n, i)
    for i in range(n - 1, 0, -1):
        arr[0], arr[i] = arr[i], arr[0]
        _heapify(arr, i, 0)


# ─────────────────────────────────────────────
#  UI helpers
# ─────────────────────────────────────────────


def print_banner():
    print()
    print(f"{CYAN}{BOLD}  ╔═══════════════════════════════════════════════╗{R}")
    print(
        f"{CYAN}{BOLD}  ║   {WHITE}SORTING ALGORITHM BENCHMARK{CYAN}  ·  {YELLOW}Python{CYAN}      ║{R}"
    )
    print(f"{CYAN}{BOLD}  ╚═══════════════════════════════════════════════╝{R}")
    print()


def separator():
    print(f"{DIM}  ──────────────────────────────────────────────{R}")


def render_bar(ms: float, max_ms: float, width: int = 30) -> str:
    filled = int(ms / max_ms * width) if max_ms > 0 else 0
    filled = min(filled, width)
    bar = "█" * filled + f"{DIM}░{R}{GREEN}" * (width - filled)
    return f"{GREEN}  [{bar}]{R}"


def color_for_ms(ms: float) -> str:
    return GREEN if ms < 1 else (YELLOW if ms < 100 else RED)


def print_result(name: str, ms: float, max_ms: float, skipped: bool):
    if skipped:
        print(f"{YELLOW}  {name:<18} {DIM}skipped (too slow for large N){R}")
        return
    time_str = f"{ms:8.3f} ms"
    col = color_for_ms(ms)
    print(f"{WHITE}{BOLD}  {name:<18} {R}{col}  {time_str}  {R}", end="")
    print(render_bar(ms, max_ms))


# ─────────────────────────────────────────────
#  Data helpers
# ─────────────────────────────────────────────

LARGE_THRESHOLD = 100_000


def generate_random(n: int) -> list[int]:
    return [random.randint(0, n * 10) for _ in range(n)]


def load_from_file(path: str) -> list[int]:
    with open(path) as f:
        tokens = f.read().split()
    result = []
    for t in tokens:
        try:
            result.append(int(t))
        except ValueError:
            pass
    return result


# ─────────────────────────────────────────────
#  Benchmark runner
# ─────────────────────────────────────────────

ALGORITHMS = [
    ("Insertion Sort", insertion_sort, True),
    ("Bubble Sort", bubble_sort, True),
    ("Selection Sort", selection_sort, True),
    ("Merge Sort", merge_sort, False),
    ("Quick Sort", quick_sort, False),
    ("Heap Sort", heap_sort, False),
]


def run_benchmarks(original: list[int]) -> None:
    n = len(original)
    print(f"{DIM}  Array size: {R}{WHITE}{BOLD}{n} elements\n{R}")

    times: list[float] = []
    skipped: list[bool] = []
    max_ms = 0.0

    for name, fn, skip_large in ALGORITHMS:
        skip = skip_large and n > LARGE_THRESHOLD
        skipped.append(skip)
        if skip:
            times.append(-1.0)
            continue
        arr = copy.copy(original)
        t0 = time.perf_counter()
        fn(arr)
        ms = (time.perf_counter() - t0) * 1000
        times.append(ms)
        if ms > max_ms:
            max_ms = ms

    separator()
    print(f"{DIM}  {'Algorithm':<18}   {'Time':>10}   Relative{R}")
    separator()

    for i, (name, _, _) in enumerate(ALGORITHMS):
        print_result(name, times[i], max_ms, skipped[i])

    separator()

    valid = [(ms, i) for i, (ms, s) in enumerate(zip(times, skipped)) if not s]
    if valid:
        best_ms, best_i = min(valid)
        name = ALGORITHMS[best_i][0]
        print(f"{GREEN}{BOLD}\n  🏆 Fastest: {name} ({best_ms:.3f} ms){R}")
    print()


# ─────────────────────────────────────────────
#  Main
# ─────────────────────────────────────────────


def main():
    print_banner()

    if len(sys.argv) >= 2:
        path = sys.argv[1]
        data = load_from_file(path)
        print(f"{GREEN}  ✔ Loaded {len(data)} integers from '{path}'\n{R}")
    else:
        print(f"{BOLD}  Input source:{R}")
        print(f"    {CYAN}[1]{R} Generate random array")
        print(f"    {CYAN}[2]{R} Load from file")
        choice = input("\n  Choice: ").strip()

        if choice == "2":
            path = input("  File path: ").strip()
            data = load_from_file(path)
            print(f"{GREEN}  ✔ Loaded {len(data)} integers\n{R}")
        else:
            try:
                n = int(input("  Array size (e.g. 100000): ").strip())
                if n <= 0:
                    raise ValueError
            except ValueError:
                print(f"{RED}  Invalid size.{R}")
                sys.exit(1)
            data = generate_random(n)
            print(f"{GREEN}  ✔ Generated {n} random integers\n{R}")

    if len(data) > LARGE_THRESHOLD:
        print(
            f"{YELLOW}  ⚠  N > {LARGE_THRESHOLD} → O(n²) algorithms will be skipped.\n{R}"
        )

    print(f"{CYAN}{BOLD}  Running benchmarks...\n{R}")
    run_benchmarks(data)


if __name__ == "__main__":
    main()
