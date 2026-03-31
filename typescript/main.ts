import * as fs from "fs";
import * as readline from "readline";

// ─────────────────────────────────────────────
//  ANSI colors
// ─────────────────────────────────────────────
const C = {
  reset: "\x1b[0m",
  bold: "\x1b[1m",
  dim: "\x1b[2m",
  cyan: "\x1b[36m",
  green: "\x1b[32m",
  yellow: "\x1b[33m",
  red: "\x1b[31m",
  white: "\x1b[97m",
};

// ─────────────────────────────────────────────
//  Sorting algorithms
// ─────────────────────────────────────────────

function insertionSort(arr: number[]): void {
  for (let i = 1; i < arr.length; i++) {
    const key = arr[i];
    let j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

function bubbleSort(arr: number[]): void {
  const n = arr.length;
  for (let i = 0; i < n - 1; i++)
    for (let j = 0; j < n - i - 1; j++)
      if (arr[j] > arr[j + 1]) [arr[j], arr[j + 1]] = [arr[j + 1], arr[j]];
}

function selectionSort(arr: number[]): void {
  const n = arr.length;
  for (let i = 0; i < n - 1; i++) {
    let mn = i;
    for (let j = i + 1; j < n; j++) if (arr[j] < arr[mn]) mn = j;
    [arr[i], arr[mn]] = [arr[mn], arr[i]];
  }
}

function mergeSortHelper(arr: number[], l: number, r: number): void {
  if (l >= r) return;
  const m = Math.floor((l + r) / 2);
  mergeSortHelper(arr, l, m);
  mergeSortHelper(arr, m + 1, r);
  const left = arr.slice(l, m + 1),
    right = arr.slice(m + 1, r + 1);
  let i = 0,
    j = 0,
    k = l;
  while (i < left.length && j < right.length)
    arr[k++] = left[i] <= right[j] ? left[i++] : right[j++];
  while (i < left.length) arr[k++] = left[i++];
  while (j < right.length) arr[k++] = right[j++];
}
function mergeSort(arr: number[]): void {
  mergeSortHelper(arr, 0, arr.length - 1);
}

function quickSortHelper(arr: number[], low: number, high: number): void {
  if (low >= high) return;
  const pivot = arr[high];
  let i = low - 1;
  for (let j = low; j < high; j++)
    if (arr[j] <= pivot) {
      i++;
      [arr[i], arr[j]] = [arr[j], arr[i]];
    }
  [arr[i + 1], arr[high]] = [arr[high], arr[i + 1]];
  const pi = i + 1;
  quickSortHelper(arr, low, pi - 1);
  quickSortHelper(arr, pi + 1, high);
}
function quickSort(arr: number[]): void {
  quickSortHelper(arr, 0, arr.length - 1);
}

function heapify(arr: number[], n: number, i: number): void {
  let largest = i,
    l = 2 * i + 1,
    r = 2 * i + 2;
  if (l < n && arr[l] > arr[largest]) largest = l;
  if (r < n && arr[r] > arr[largest]) largest = r;
  if (largest !== i) {
    [arr[i], arr[largest]] = [arr[largest], arr[i]];
    heapify(arr, n, largest);
  }
}
function heapSort(arr: number[]): void {
  const n = arr.length;
  for (let i = Math.floor(n / 2) - 1; i >= 0; i--) heapify(arr, n, i);
  for (let i = n - 1; i > 0; i--) {
    [arr[0], arr[i]] = [arr[i], arr[0]];
    heapify(arr, i, 0);
  }
}

// ─────────────────────────────────────────────
//  UI helpers
// ─────────────────────────────────────────────

function printBanner(): void {
  console.log();
  console.log(
    C.cyan +
      C.bold +
      "  ╔═══════════════════════════════════════════════╗" +
      C.reset,
  );
  console.log(
    C.cyan +
      C.bold +
      "  ║   " +
      C.white +
      "SORTING ALGORITHM BENCHMARK" +
      C.cyan +
      "  ·  " +
      C.yellow +
      "TypeScript" +
      C.cyan +
      "  ║" +
      C.reset,
  );
  console.log(
    C.cyan +
      C.bold +
      "  ╚═══════════════════════════════════════════════╝" +
      C.reset,
  );
  console.log();
}

function separator(): void {
  console.log(
    C.dim + "  ──────────────────────────────────────────────" + C.reset,
  );
}

function renderBar(ms: number, maxMs: number, width = 30): string {
  const filled =
    maxMs > 0 ? Math.min(Math.round((ms / maxMs) * width), width) : 0;
  const bar =
    "█".repeat(filled) +
    (C.dim + "░" + C.reset + C.green).repeat(width - filled);
  return C.green + "  [" + bar + "]" + C.reset;
}

function colorForMs(ms: number): string {
  return ms < 1 ? C.green : ms < 100 ? C.yellow : C.red;
}

function printResult(
  name: string,
  ms: number,
  maxMs: number,
  skipped: boolean,
): void {
  if (skipped) {
    console.log(
      C.yellow +
        `  ${name.padEnd(18)} ` +
        C.dim +
        "skipped (too slow for large N)" +
        C.reset,
    );
    return;
  }
  const timeStr = ms.toFixed(3).padStart(8) + " ms";
  process.stdout.write(C.white + C.bold + `  ${name.padEnd(18)} ` + C.reset);
  process.stdout.write(colorForMs(ms) + `  ${timeStr}  ` + C.reset);
  console.log(renderBar(ms, maxMs));
}

// ─────────────────────────────────────────────
//  Data helpers
// ─────────────────────────────────────────────

function generateRandom(n: number): number[] {
  return Array.from({ length: n }, () => Math.floor(Math.random() * n * 10));
}

function loadFromFile(path: string): number[] {
  const content = fs.readFileSync(path, "utf-8");
  return content
    .split(/\s+/)
    .filter(Boolean)
    .map(Number)
    .filter((v) => !isNaN(v));
}

// ─────────────────────────────────────────────
//  Benchmark runner
// ─────────────────────────────────────────────

const LARGE_THRESHOLD = 100_000;

interface Algorithm {
  name: string;
  fn: (arr: number[]) => void;
  skipLarge: boolean;
}

const algorithms: Algorithm[] = [
  { name: "Insertion Sort", fn: insertionSort, skipLarge: true },
  { name: "Bubble Sort", fn: bubbleSort, skipLarge: true },
  { name: "Selection Sort", fn: selectionSort, skipLarge: true },
  { name: "Merge Sort", fn: mergeSort, skipLarge: false },
  { name: "Quick Sort", fn: quickSort, skipLarge: false },
  { name: "Heap Sort", fn: heapSort, skipLarge: false },
];

function runBenchmarks(original: number[]): void {
  const n = original.length;
  console.log(
    C.dim +
      "  Array size: " +
      C.reset +
      C.white +
      C.bold +
      `${n} elements\n` +
      C.reset,
  );

  const times: number[] = [];
  const skipped: boolean[] = [];
  let maxMs = 0;

  for (const algo of algorithms) {
    const skip = algo.skipLarge && n > LARGE_THRESHOLD;
    skipped.push(skip);
    if (skip) {
      times.push(-1);
      continue;
    }

    const arr = [...original];
    const start = performance.now();
    algo.fn(arr);
    const ms = performance.now() - start;
    times.push(ms);
    if (ms > maxMs) maxMs = ms;
  }

  separator();
  console.log(
    C.dim +
      `  ${"Algorithm".padEnd(18)}   ${"Time".padStart(10)}   Relative` +
      C.reset,
  );
  separator();

  algorithms.forEach((algo, i) =>
    printResult(algo.name, times[i], maxMs, skipped[i]),
  );
  separator();

  let bestMs = Infinity,
    bestIdx = -1;
  times.forEach((ms, i) => {
    if (!skipped[i] && ms < bestMs) {
      bestMs = ms;
      bestIdx = i;
    }
  });
  if (bestIdx >= 0)
    console.log(
      C.green +
        C.bold +
        `\n  🏆 Fastest: ${algorithms[bestIdx].name} (${bestMs.toFixed(3)} ms)` +
        C.reset,
    );
  console.log();
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────

async function prompt(question: string): Promise<string> {
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });
  return new Promise((resolve) =>
    rl.question(question, (ans) => {
      rl.close();
      resolve(ans.trim());
    }),
  );
}

async function main(): Promise<void> {
  printBanner();

  let data: number[];

  if (process.argv[2]) {
    data = loadFromFile(process.argv[2]);
    console.log(
      C.green +
        `  ✔ Loaded ${data.length} integers from '${process.argv[2]}'\n` +
        C.reset,
    );
  } else {
    console.log(C.bold + "  Input source:" + C.reset);
    console.log("    " + C.cyan + "[1]" + C.reset + " Generate random array");
    console.log("    " + C.cyan + "[2]" + C.reset + " Load from file");
    const choice = await prompt("\n  Choice: ");

    if (choice === "2") {
      const path = await prompt("  File path: ");
      data = loadFromFile(path);
      console.log(C.green + `  ✔ Loaded ${data.length} integers\n` + C.reset);
    } else {
      const sizeStr = await prompt("  Array size (e.g. 100000): ");
      const n = parseInt(sizeStr, 10);
      if (isNaN(n) || n <= 0) {
        console.error(C.red + "  Invalid size." + C.reset);
        process.exit(1);
      }
      data = generateRandom(n);
      console.log(C.green + `  ✔ Generated ${n} random integers\n` + C.reset);
    }
  }

  if (data.length > LARGE_THRESHOLD)
    console.log(
      C.yellow +
        `  ⚠  N > ${LARGE_THRESHOLD} → O(n²) algorithms will be skipped.\n` +
        C.reset,
    );

  console.log(C.cyan + C.bold + "  Running benchmarks...\n" + C.reset);
  runBenchmarks(data);
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
