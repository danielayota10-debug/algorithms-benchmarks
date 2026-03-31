package main

import (
	"bufio"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"strings"
	"time"
)

// ─────────────────────────────────────────────
//  ANSI colors
// ─────────────────────────────────────────────
const (
	reset   = "\033[0m"
	bold    = "\033[1m"
	dim     = "\033[2m"
	cyan    = "\033[36m"
	green   = "\033[32m"
	yellow  = "\033[33m"
	magenta = "\033[35m"
	red     = "\033[31m"
	white   = "\033[97m"
)

// ─────────────────────────────────────────────
//  Sorting algorithms
// ─────────────────────────────────────────────

func insertionSort(arr []int) {
	for i := 1; i < len(arr); i++ {
		key, j := arr[i], i-1
		for j >= 0 && arr[j] > key {
			arr[j+1] = arr[j]
			j--
		}
		arr[j+1] = key
	}
}

func bubbleSort(arr []int) {
	n := len(arr)
	for i := 0; i < n-1; i++ {
		for j := 0; j < n-i-1; j++ {
			if arr[j] > arr[j+1] {
				arr[j], arr[j+1] = arr[j+1], arr[j]
			}
		}
	}
}

func selectionSort(arr []int) {
	n := len(arr)
	for i := 0; i < n-1; i++ {
		mn := i
		for j := i + 1; j < n; j++ {
			if arr[j] < arr[mn] {
				mn = j
			}
		}
		arr[i], arr[mn] = arr[mn], arr[i]
	}
}

func mergeSort(arr []int) {
	if len(arr) <= 1 {
		return
	}
	mid := len(arr) / 2
	left := make([]int, mid)
	right := make([]int, len(arr)-mid)
	copy(left, arr[:mid])
	copy(right, arr[mid:])
	mergeSort(left)
	mergeSort(right)
	i, j, k := 0, 0, 0
	for i < len(left) && j < len(right) {
		if left[i] <= right[j] {
			arr[k] = left[i]
			i++
		} else {
			arr[k] = right[j]
			j++
		}
		k++
	}
	for i < len(left) {
		arr[k] = left[i]
		i++
		k++
	}
	for j < len(right) {
		arr[k] = right[j]
		j++
		k++
	}
}

func quickSort(arr []int) {
	if len(arr) <= 1 {
		return
	}
	pivot := arr[len(arr)-1]
	i := -1
	for j := 0; j < len(arr)-1; j++ {
		if arr[j] <= pivot {
			i++
			arr[i], arr[j] = arr[j], arr[i]
		}
	}
	arr[i+1], arr[len(arr)-1] = arr[len(arr)-1], arr[i+1]
	pi := i + 1
	quickSort(arr[:pi])
	quickSort(arr[pi+1:])
}

func heapify(arr []int, n, i int) {
	largest, l, r := i, 2*i+1, 2*i+2
	if l < n && arr[l] > arr[largest] {
		largest = l
	}
	if r < n && arr[r] > arr[largest] {
		largest = r
	}
	if largest != i {
		arr[i], arr[largest] = arr[largest], arr[i]
		heapify(arr, n, largest)
	}
}

func heapSort(arr []int) {
	n := len(arr)
	for i := n/2 - 1; i >= 0; i-- {
		heapify(arr, n, i)
	}
	for i := n - 1; i > 0; i-- {
		arr[0], arr[i] = arr[i], arr[0]
		heapify(arr, i, 0)
	}
}

// ─────────────────────────────────────────────
//  UI helpers
// ─────────────────────────────────────────────

func printBanner() {
	fmt.Println()
	fmt.Println(cyan + bold + "  ╔══════════════════════════════════════════════╗" + reset)
	fmt.Println(cyan + bold + "  ║   " + white + "SORTING ALGORITHM BENCHMARK" + cyan + "  ·  " + yellow + "Go" + cyan + "         ║" + reset)
	fmt.Println(cyan + bold + "  ╚══════════════════════════════════════════════╝" + reset)
	fmt.Println()
}

func separator() {
	fmt.Println(dim + "  ──────────────────────────────────────────────" + reset)
}

func printBar(ms, maxMs float64, width int) {
	filled := 0
	if maxMs > 0 {
		filled = int(ms / maxMs * float64(width))
	}
	if filled > width {
		filled = width
	}
	fmt.Print(green + "  [")
	for i := 0; i < width; i++ {
		if i < filled {
			fmt.Print("█")
		} else {
			fmt.Print(dim + "░" + reset + green)
		}
	}
	fmt.Print("]" + reset)
}

func colorForMs(ms float64) string {
	if ms < 1.0 {
		return green
	} else if ms < 100 {
		return yellow
	}
	return red
}

func printResult(name string, ms float64, maxMs float64, skipped bool) {
	if skipped {
		fmt.Printf(yellow+"  %-18s "+dim+"skipped (too slow for large N)\n"+reset, name)
		return
	}
	fmt.Printf(white+bold+"  %-18s "+reset, name)
	fmt.Printf(colorForMs(ms)+"  %8.3f ms  "+reset, ms)
	printBar(ms, maxMs, 30)
	fmt.Println()
}

// ─────────────────────────────────────────────
//  Data helpers
// ─────────────────────────────────────────────

func copySlice(src []int) []int {
	dst := make([]int, len(src))
	copy(dst, src)
	return dst
}

func generateRandom(n int) []int {
	arr := make([]int, n)
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	for i := range arr {
		arr[i] = r.Intn(n * 10)
	}
	return arr
}

func loadFromFile(path string) ([]int, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	var arr []int
	scanner := bufio.NewScanner(f)
	scanner.Split(bufio.ScanWords)
	for scanner.Scan() {
		v, err := strconv.Atoi(strings.TrimSpace(scanner.Text()))
		if err == nil {
			arr = append(arr, v)
		}
	}
	return arr, scanner.Err()
}

// ─────────────────────────────────────────────
//  Benchmark runner
// ─────────────────────────────────────────────

const largeThreshold = 500000

type Algorithm struct {
	name      string
	fn        func([]int)
	skipLarge bool
}

func runBenchmarks(original []int) {
	n := len(original)
	algos := []Algorithm{
		{"Insertion Sort", insertionSort, true},
		{"Bubble Sort", bubbleSort, true},
		{"Selection Sort", selectionSort, true},
		{"Merge Sort", mergeSort, false},
		{"Quick Sort", quickSort, false},
		{"Heap Sort", heapSort, false},
	}

	fmt.Printf(dim+"  Array size: "+reset+white+bold+"%d elements\n\n"+reset, n)

	times := make([]float64, len(algos))
	skipped := make([]bool, len(algos))
	maxMs := 0.0

	for i, algo := range algos {
		if algo.skipLarge && n > largeThreshold {
			skipped[i] = true
			continue
		}
		arr := copySlice(original)
		start := time.Now()
		algo.fn(arr)
		elapsed := float64(time.Since(start).Microseconds()) / 1000.0
		times[i] = elapsed
		if elapsed > maxMs {
			maxMs = elapsed
		}
	}

	separator()
	fmt.Printf(dim+"  %-18s   %10s   %s\n"+reset, "Algorithm", "Time", "Relative")
	separator()

	for i, algo := range algos {
		printResult(algo.name, times[i], maxMs, skipped[i])
	}
	separator()

	bestMs, bestIdx := 1e18, -1
	for i := range algos {
		if !skipped[i] && times[i] < bestMs {
			bestMs = times[i]
			bestIdx = i
		}
	}
	if bestIdx >= 0 {
		fmt.Printf(green+bold+"\n  🏆 Fastest: %s (%.3f ms)\n"+reset, algos[bestIdx].name, bestMs)
	}
	fmt.Println()
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────

func readLine(prompt string) string {
	fmt.Print(prompt)
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	return strings.TrimSpace(scanner.Text())
}

func main() {
	printBanner()

	var data []int

	if len(os.Args) >= 2 {
		var err error
		data, err = loadFromFile(os.Args[1])
		if err != nil {
			fmt.Println(red + "  Error: " + err.Error() + reset)
			os.Exit(1)
		}
		fmt.Printf(green+"  ✔ Loaded %d integers from '%s'\n\n"+reset, len(data), os.Args[1])
	} else {
		fmt.Println(bold + "  Input source:" + reset)
		fmt.Println("    " + cyan + "[1]" + reset + " Generate random array")
		fmt.Println("    " + cyan + "[2]" + reset + " Load from file")
		choice := readLine("\n  Choice: ")

		if choice == "2" {
			path := readLine("  File path: ")
			var err error
			data, err = loadFromFile(path)
			if err != nil {
				fmt.Println(red + "  Error: " + err.Error() + reset)
				os.Exit(1)
			}
			fmt.Printf(green+"  ✔ Loaded %d integers\n\n"+reset, len(data))
		} else {
			sizeStr := readLine("  Array size (e.g. 100000): ")
			n, err := strconv.Atoi(sizeStr)
			if err != nil || n <= 0 {
				fmt.Println(red + "  Invalid size." + reset)
				os.Exit(1)
			}
			data = generateRandom(n)
			fmt.Printf(green+"  ✔ Generated %d random integers\n\n"+reset, n)
		}
	}

	if len(data) > largeThreshold {
		fmt.Printf(yellow+"  ⚠  N > %d → O(n²) algorithms will be skipped.\n\n"+reset, largeThreshold)
	}

	fmt.Println(cyan + bold + "  Running benchmarks...\n" + reset)
	runBenchmarks(data)
}
