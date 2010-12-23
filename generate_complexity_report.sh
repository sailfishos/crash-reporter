#!/bin/sh

PMCCABE_BIN=$(which pmccabe)
 
if [ $? -ne 0 ]; then
	echo "Cannot calculate complexity. pmccabe is not installed. Run apt-get update && apt-get install pmccabe."
	exit 1
fi	

echo "Generating report to complexity_report.log..."

rm -f complexity_report.log && touch complexity_report.log

echo "Complexity for src/libs/*/*.cpp.\n" >> complexity_report.log

echo "Per-file totals.\n" >> complexity_report.log

$PMCCABE_BIN -Fv src/libs/*/*.cpp >> complexity_report.log

echo "\nPer-function totals.\n" >> complexity_report.log
$PMCCABE_BIN src/libs/*/*.cpp | sort -rn >> complexity_report.log

echo "\n---------------------------------------------------\n" >> complexity_report.log

echo "Complexity for src/daemon/*.cpp.\n" >> complexity_report.log

echo "Per-file totals.\n" >> complexity_report.log

$PMCCABE_BIN -Fv src/daemon/*.cpp >> complexity_report.log

echo "\nPer-function totals.\n" >> complexity_report.log
$PMCCABE_BIN src/daemon/*.cpp | sort -rn >> complexity_report.log

echo "\n---------------------------------------------------\n" >> complexity_report.log

echo "Complexity for src/dialogserver/*.cpp.\n" >> complexity_report.log

echo "Per-file totals.\n" >> complexity_report.log

$PMCCABE_BIN -Fv src/dialogserver/*.cpp >> complexity_report.log

echo "\nPer-function totals.\n" >> complexity_report.log
$PMCCABE_BIN src/dialogserver/*.cpp | sort -rn >> complexity_report.log

echo "\n---------------------------------------------------\n" >> complexity_report.log

echo "Complexity for src/dialogplugins/*/*.cpp.\n" >> complexity_report.log

echo "Per-file totals.\n" >> complexity_report.log

$PMCCABE_BIN -Fv src/dialogplugins/*/*.cpp >> complexity_report.log

echo "\nPer-function totals.\n" >> complexity_report.log
$PMCCABE_BIN src/dialogplugins/*/*.cpp | sort -rn >> complexity_report.log

echo "\n---------------------------------------------------\n" >> complexity_report.log

echo "Complexity for src/dcpapplet/*.cpp.\n" >> complexity_report.log

echo "Per-file totals.\n" >> complexity_report.log

$PMCCABE_BIN -Fv src/dcpapplet/*.cpp >> complexity_report.log

echo "\nPer-function totals.\n" >> complexity_report.log
$PMCCABE_BIN src/dcpapplet/*.cpp | sort -rn >> complexity_report.log

echo "\n---------------------------------------------------\n" >> complexity_report.log

SLOCCOUNT_BIN=$(which sloccount)

if [ $? -ne 0 ]; then
        echo "Cannot calculate lines of code. sloccount is not installed. Run apt-get update && apt-get install sloccount."
	exit 1
fi

$SLOCCOUNT_BIN src/* >> complexity_report.log

echo "Done."

