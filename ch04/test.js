const { execSync } = require('child_process');
const fs = require('fs');

// Default configuration
let numRuns = 5;
let forceFetch = false;
let debug = false;
const macros = ['PROTO', 'LIB', 'LIB_4_1', 'LIB_4_3']; // Example macros
const bold = "\033[1m";
const normal = "\033[0m";

// Parse command-line arguments
process.argv.slice(2).forEach(arg => {
  if (arg === '-f') forceFetch = true;
  if (arg === '-d') debug = true;
  else if (!isNaN(arg)) numRuns = parseInt(arg);
});

// Function to run shell commands synchronously and return stdout
function runCommand(cmd) {
  return execSync(cmd, { encoding: 'utf-8' }).trim();
}

// Fetch data if needed
if (forceFetch || !fs.existsSync('data.csv')) {
  console.log("Fetching stock data...");
  runCommand('python3 get_stock_data.py -t AAPL MSFT NVDA AMZN META GOOGL GOOG BRK-B LLY AVGO JPM TSLA UNH XOM V PG COST JNJ MA HD -p max -i 1d > data.csv');
}

// Compile C programs
macros.forEach(macro => {
  let command = "";
  if (macro.includes('CPP')) {
    command = "g++"
    if (debug)
      command += " -g";
    command += ` -D${macro} csv.cpp test.cpp -o test_${macro}`;
  } else {
    command = "gcc -Wall";
    if (debug)
      command += " -g";
    command += ` -D${macro} csv.c test.c -o test_${macro}`;
  }
   runCommand(command);
});

// Store run times for each macro
let times = {};
macros.forEach(macro => times[macro] = []);

// Function to calculate and format times
function calculateAverage(timesArray) {
  const total = timesArray.reduce((acc, curr) => acc + parseFloat(curr), 0);
  return (total / timesArray.length).toFixed(3);
}

// Print the table header
let tableHeader = "Run\t";
macros.forEach(macro => tableHeader += `${macro}\t`);
console.log(tableHeader);

// Run the tests, redirect output to dedicated files, and record timings
for (let i = 1; i <= numRuns; i++) {
  let row = `${i}\t`;
  let runTimes = [];

  macros.forEach(macro => {
    const start = Date.now();
    // Redirect program output to its dedicated result file
    runCommand(`./test_${macro} < data.csv > test_${macro}_result_${i}.txt`);
    const elapsedTime = ((Date.now() - start) / 1000).toFixed(3);
    times[macro].push(elapsedTime);
    runTimes.push(elapsedTime);
    row += `${elapsedTime}\t`;
  });

  // Determine the fastest time in this run and highlight it
  const fastestTime = Math.min(...runTimes);
  macros.forEach((macro, idx) => {
    if (runTimes[idx] == fastestTime) {
      row = row.replace(runTimes[idx], `${bold}${runTimes[idx]}${normal}`);
    }
  });

  console.log(row);  // Log the result of the current run
}

// Calculate and print averages
let avgRow = "Avg\t";
macros.forEach(macro => {
  const avg = calculateAverage(times[macro]);
  avgRow += `${bold}${avg}${normal}\t`;
});

console.log(avgRow);  // Log the final average row

