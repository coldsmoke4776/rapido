# Rapido — Lightweight Recon Scanner

⚡ **Rapido** is a tiny, fast TCP port scanner and triage engine written in C — part of what I hope will become the Matt-a-Sploit family of tools.  

Built for speed, clarity, and to point you at the *next lever* to pull, not to be a full Nmap replacement.

---

## TL;DR / Tagline

> Rapido: fast, focused port scanning with built-in banner triage and next-step suggestions.

---

## Features (v0.1)

- Single-IP, CIDR ranges, or text-file input (`.txt`) of IPs.
- Port range scanning (start/end ports).
- Basic banner grabbing and per-protocol triage (HTTP, SSH, FTP — more to come).
- Quiet mode (`-q`) for compact output and quick results.
- Summary mode (`-s`) for high-level scan stats.
- Small, dependency-free C codebase; easy to read and extend.

## Features (v0.2) — Concurrent Scanning (2025-09-28)

**Highlights**
- ✅ Per-target concurrent scanning using a custom pthreads threadpool
- ✅ Banner grabbing and service triage
- ✅ Thread-safe printing of banners (no interleaved multi-line output)
- ✅ Per-target open/closed counters and summary

---

## Quick Install

```bash
# clone & build
git clone https://github.com/coldsmoke4776/rapido.git
cd rapido
make

# run
./rapido 127.0.0.1 1 100


Requirements:
- POSIX-like environment (Linux/macOS).
- gcc or clang and standard C dev headers.

# **Part 2 — Usage, Flags & Examples**

./rapido [options] <IP|CIDR|file.txt> <start_port> <end_port>


### Options
- `-q` Quiet mode — print only open ports (concise machine-friendly output).
- `-s` Summary mode — print a compact summary at the end of each target.
- `-h`, `--help` Show help text.

### Input types
- Single IP: `./rapido 10.0.4.30 1 1024`
- CIDR: `./rapido 10.0.4.0/27 1 25`
- File of IPs: `./rapido targets.txt 1 100` (one IP per line)

---

## Examples

1. Basic scan:
```bash
./rapido 127.0.0.1 1 100

2. Quiet Mode:
```bash
./rapido -q 127.0.0.1 1 1024

3. CIDR Scan:
```bash
./rapido 10.0.4.0/27 1 25

4. Input File Scan:
```bash
./rapido input_targets.txt 1 25


### Example Output

➜  rapido_scanner ./rapido 127.0.0.1 70 90
==============================================

            MATTasploit Framework (TM)            
==============================================

            Rapido Reconnaissance Tool              
==============================================

===== CONCURRENCY CHECK =====
Job 0 executed (thread 6097825792)
Job 1 executed (thread 6097825792)
Job 2 executed (thread 6097825792)
Job 3 executed (thread 6097825792)
Job 4 executed (thread 6097825792)
Job 5 executed (thread 6097825792)
Job 6 executed (thread 6097825792)
Job 7 executed (thread 6097825792)
Job 8 executed (thread 6097825792)
Job 9 executed (thread 6097825792)
========== Scanning 127.0.0.1 from port 70 to port 90 ==========
[-] 71/tcp CLOSED on 127.0.0.1
[-] 75/tcp CLOSED on 127.0.0.1
[-] 76/tcp CLOSED on 127.0.0.1
[-] 70/tcp CLOSED on 127.0.0.1
[-] 77/tcp CLOSED on 127.0.0.1
[-] 73/tcp CLOSED on 127.0.0.1
[-] 74/tcp CLOSED on 127.0.0.1
[-] 72/tcp CLOSED on 127.0.0.1
[-] 78/tcp CLOSED on 127.0.0.1
[-] 79/tcp CLOSED on 127.0.0.1
Port 80 is open on 127.0.0.1
[-] 81/tcp CLOSED on 127.0.0.1
[-] 85/tcp CLOSED on 127.0.0.1
[-] 83/tcp CLOSED on 127.0.0.1
[-] 82/tcp CLOSED on 127.0.0.1
[-] 86/tcp CLOSED on 127.0.0.1
[-] 84/tcp CLOSED on 127.0.0.1
[-] 87/tcp CLOSED on 127.0.0.1
[-] 88/tcp CLOSED on 127.0.0.1
[-] 90/tcp CLOSED on 127.0.0.1
[-] 89/tcp CLOSED on 127.0.0.1
Port 80 is open on 127.0.0.1 Banner: HTTP/1.0 200 OK
Server: SimpleHTTP/0.6 Python/3.9.6
Date: Sun, 28 Sep 2025 15:56:26 GMT
Content-type: text/html; charset=utf-8
Content-Length: 613


[!] 80/tcp: Python SimpleHTTP detected — check for directory listing.
[*] TCP - Port 80 is OPEN on 127.0.0.1
--------------Done scanning 127.0.0.1------------------
[Summary] 127.0.0.1: 1 open, 20 closed


### Dev Notes
---

## How it works (high level)
1. `main` calls `parse_args` to convert CLI input (single IP / CIDR / file) into a `targets[]` list.
2. For each target, loop port range and call `probe_port_ipv4`.
3. `probe_port_ipv4` creates a socket, connects (with a configurable timeout), optionally sends a small probe (e.g., `HEAD /`) for HTTP, reads banner via `recv()`, and returns open/closed.
4. If banner text is available, `triage` runs simple heuristics (e.g., check for "HTTP", "SSH", "FTP") and prints suggested next steps.

---

## Development notes
- The codebase is intentionally simple and well-commented for learning and fast iteration.
- Files are split into logical components:
  - `args.c/.h` — CLI parsing & input expansion (CIDR, file).
  - `cidr.c/.h` — CIDR -> host list expansion.
  - `probe.c/.h` — Socket connect / banner grabbing / timeouts.
  - `triage.c/.h` — Heuristics and next-step suggestions.
  - `banner.c/.h` — Small UI helpers (ASCII banner, colors).

---

## Roadmap (short to medium term)
- Improve triage rules (CMS detection, common service heuristics).
- JSON output mode for easy automation.
- Parallel scanning (thread pool / non-blocking sockets) for speed across CIDR ranges.

## Roadmap (v0.3+)
- Ordered port output (optional printing mode)
- CLI flags: -q/--quiet, -v/--verbose, --workers N, --timeout ms
- JSON output / logging to file
- UDP scanning mode and service-specific probes
- Fleshed out triage engine
- Performance tuning & tests (ASAN/Valgrind CI)


# Disclaimer

**Only use Rapido on networks you own or for which you have explicit authorization to scan.** 
Unauthorized scanning may be illegal and/or violate acceptable use policies. The author is not responsible for misuse.
