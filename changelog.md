# Changelog

## [0.2.0] - 2025-09-28
### Added
- Concurrency support via a custom pthreads threadpool.
- Per-target threadpool model for clean open/closed counting.
- Mutex/condition variable handling for safe counters and banner printing.
- First fully working concurrent scan on localhost.

### Fixed
- Suppressed duplicate CLOSED prints.
- Cleaned test harness/debug output.

## [0.1.0] - 2025-09-14
- Initial working prototype of Rapido: sequential port scanning with banner grabbing.
