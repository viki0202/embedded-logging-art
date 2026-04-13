@REM type log.jsonl | python log_parser.py --color auto --trace safety-jitter --fields ts,level,actor,action,narrative,details

@REM type log.jsonl | python log_parser.py ^
@REM   --actor SCHED --actor CAN.Tx --actor HousekeepingTask ^
@REM   --fields ts,actor,action,narrative ^
@REM   --separator " | " ^
@REM   --color auto

@REM type log.jsonl | python log_parser.py --actor SCHED --actor CAN.Tx --actor CAN.Rx --actor Safety --fields ts,actor,action,narrative,details --separator " | " --color auto | less -R

type log.jsonl | python tools\log_parser.py --preset sched --color always | c:\msys64\usr\bin\less.exe -R