# PRISM DESKTOP — MANDATORY AUTONOMOUS DEVELOPMENT RULES

## Execution Mode: Autonomous Build–Test–Fix

Work autonomously on the currently authorized Prism Desktop milestone.

Do not stop after writing code. Complete implementation, compilation, automated verification, debugging, and reporting.

### Mandatory execution loop

For every task:

1. Inspect existing source code, interfaces, tests, and dependencies.
2. Implement the smallest complete and testable change.
3. Run relevant build, static checks, and automated tests.
4. If any check fails, analyze the actual failure and fix its root cause.
5. Repeat the build–test–fix cycle until the required checks pass.
6. Never proceed to the next dependent task while required checks fail.
7. Record verified results and update progress documentation.
8. Continue automatically with the next task within the authorized milestone.

### Testing integrity

- Never fabricate PASS results.
- Never remove, weaken, skip, or bypass failing tests to force success.
- Never hide compiler warnings or QML errors.
- Never classify unverified GUI interactions as tested.
- Add regression tests for bugs when practical.
- Report limitations of headless or unavailable graphical testing.

### Human interaction

- Do not ask for confirmation after routine code edits, builds, test runs, or bug fixes that are already authorized.
- Avoid unnecessary interactive commands.
- Use non-interactive commands and deterministic test procedures when possible.
- Continue automatically between tasks.
- Report milestone progress in the project files.

### Safety boundaries

- Work within the project repository.
- Never use sudo automatically.
- Never modify or replace KDE Plasma, SDDM, or system-wide desktop configuration.
- Never delete unrelated user files.
- Never expose or modify credentials.
- Never install system packages or change security settings without authorization.
- Never push to remote repositories without explicit permission.
- Respect tool and environment approval requirements; do not attempt to bypass them.

### Error handling

If a task repeatedly fails:

1. Inspect logs and identify the root cause.
2. Try alternative solutions within the approved scope.
3. Rebuild and rerun the relevant tests.
4. Document every unresolved blocker.
5. If safe progress is impossible, mark the task BLOCKED and stop.

Never enter an uncontrolled infinite retry loop.

### Progress reports

Maintain:

- `docs/progress/STATUS.md`
- A milestone-specific report
- Build and test results
- Known issues and blockers
- Relevant commit references

Allowed statuses:

- NOT STARTED
- IN PROGRESS
- TESTING
- BLOCKED
- DONE

Only mark DONE when all required, verifiable acceptance criteria pass.

### Milestone discipline

- Work only on milestones explicitly authorized by the user.
- Do not implement unrelated future milestones.
- Keep modules separated by responsibility.
- Preserve stable interfaces and working functionality.
- Commit completed logical changes when the Git repository is ready.
- Continue until the authorized milestone is complete or a genuine blocker prevents safe progress.

## Priority

Correctness > Safety > Testability > Maintainability > Visual polish > Speed of delivery.
