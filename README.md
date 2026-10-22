# KernelSim — Operating Systems Lab

A zero-dependency, responsive browser companion to the KernelSim C++17 project. All web calculations run locally in the browser; no backend, account, package manager, or external assets are required.

## Run locally

1. Extract the project ZIP.
2. Open `KernelSim/web/index.html` in a modern browser (Chrome, Edge, Firefox, or Safari).
3. Choose a lab from the navigation. The app works offline once the files are available locally.

Alternatively, serve the `web` directory with any static file server. No build command is required.

## Deploy publicly

### GitHub Pages
1. Push the project to a GitHub repository.
2. In **Settings → Pages**, choose **Deploy from a branch**.
3. Either move/copy the contents of `web/` into the repository's published root, or use a workflow/static-host setup that publishes the `web/` directory.
4. Save and open the Pages URL. Ensure `index.html`, `app.js`, `styles.css`, and this README are included in the published directory.

### Netlify / Cloudflare Pages / Vercel
- Create a project from the repository.
- Set the publish/output directory to `KernelSim/web` (or `web` if that is the repository root).
- Leave the build command blank; this is a static site.
- Deploy. No environment variables are needed.

## Features

### CPU scheduling
- FCFS, SJF (non-preemptive), SRTF, Priority (preemptive and non-preemptive), Round Robin.
- Editable process list, validation, CSV/JSON import, CSV exports, Gantt timeline, per-process CT/TAT/WT/RT, averages, CPU utilization, throughput, and algorithm comparison.
- Priority convention: smaller number means higher priority. Assumptions: one CPU, integer time units, one CPU burst per process, no I/O blocking, zero context-switch overhead.

### Banker's algorithm
- Safety evaluation, Need and Available calculations, safe sequence, step trace, and transactional resource requests.
- To use a custom scenario, expand **Edit resource model (JSON)** in the resource-state panel. Edit `ids`, `names`, `total`, `alloc`, and `max`; then choose **Validate and apply model**. Dimensions, unique IDs/names, integer values, resource totals, and allocation-vs-maximum are validated.
- `Need = Maximum − Allocation`; `Available = Total − sum(Allocation)`.

### Deadlock lab
- Editable total resources, process IDs, allocation matrix, and outstanding request matrix.
- Detection trace shows Work before and after each process completion and lists processes that remain unfinished.
- Includes a concise overview of prevention, avoidance, detection, and recovery. The detector assumes the entered request matrix represents current outstanding requests; it is not a future-request prediction.

## C++ build and tests

The original C++ project is preserved alongside the browser app. From the `KernelSim/` directory:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The browser algorithms are a separate JavaScript implementation. The C++ test suite verifies the existing C++ core; it is not a formal proof of equivalence between every browser result and the C++ engine. Browser JavaScript syntax was checked with Node.js. A full graphical browser smoke test was not performed in this environment.

## Files
- `web/index.html` — application structure
- `web/styles.css` — responsive styling and dark theme
- `web/app.js` — client-side algorithms and interactions
- `src/` and `tests/` — original C++ implementation and automated tests
- `docs/` — project report, demo notes, and test records

KernelSim is an educational simulator, not an operating-system kernel.


### Opening from a ZIP on Windows

Extract the entire ZIP first using **Right-click → Extract All**. Do not open `index.html` from inside the compressed ZIP preview. The main stylesheet is also embedded in `index.html` so the layout remains styled even in restrictive local-file previews; keep `app.js` alongside `index.html` for the simulator functionality.
