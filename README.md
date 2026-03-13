# Sztuka logowania w systemach embedded — kod

To repozytorium zawiera **kod źródłowy oraz materiały pomocnicze**
do książki **„Sztuka logowania w systemach embedded”**.

---

## 📦 Zawartość repozytorium

### 📘 `docs/embedded_logging_art.pdf`

Dostępny jest **roboczy fragment książki (~55 stron)**  
poświęconej logowaniu w systemach embedded.

Dokument jest **w trakcie pisania** i będzie stopniowo rozbudowywany

---

### 🧠 `CANSim/`

Minimalny symulator systemu embedded w **C++**, wykorzystywany w książce
do demonstracji zagadnień projektowych:

- model magistrali CAN (ramki, słowniki, błędy),
- minimalny system logowania (JSONL + log narracyjny),
- prosty scheduler (bez wywłaszczania),
- zadania i scenariusze testowe,
- przykłady omawiane w treści książki.

---

### 🛠 `tools/log_parser.py`

Lekki parser logów w formacie **JSONL**, napisany w Pythonie:

- filtracja po `actor`, `level`, `corr_id`, `trace_id`,
- ekstrakcja wpisów narracyjnych,
- eksport do CSV w celu dalszej analizy.

---

## ▶️ Budowanie

Projekt korzysta z **CMake**. Aby zbudować symulator `CANSim`:

```bash
cd CANSim
mkdir build
cd build
cmake ..
cmake --build .
