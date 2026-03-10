# Sztuka logowania w systemach embedded — kod

To repozytorium zawiera kod źródłowy do książki  
**„Sztuka logowania w systemach embedded” (Leanpub, 2026)**.

## 📦 Zawartość

### `CANSim/`
Minimalny symulator systemu embedded w C++:
- model CAN (ramki, dictionary, błędy),
- minimalny logger (JSONL + narracyjny),
- mini‑scheduler (bez wywłaszczania),
- zadania i scenariusze,
- przykłady użyte w książce.

### `tools/log_parser.py`
Lekki parser logów JSONL:
- filtracja po `actor`, `level`, `corr_id`, `trace_id`,
- ekstrakcja narracyjnych wpisów,
- eksport do CSV i dalszej analizy.

## ▶️ Budowanie

Projekt korzysta z CMake. Aby zbudować CANSim:

```bash
cd CANSim
mkdir build
cd build
cmake ..
cmake --build .
