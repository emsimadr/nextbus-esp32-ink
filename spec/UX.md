
# UX spec: e-ink layout and states

This spec defines the display layout and state handling.

## Display content (MVP)
Show up to 2 lines (board items), each with:
- route label (short)
- destination or direction label (optional, short)
- minutes until next arrival
- status marker (stale / no service / error)

Also show:
- last updated time (optional, small)
- a simple progress/approach visualization and "leave now" threshold indicator

## Example layout (conceptual)
Top:
- "NextBus" title + WiFi indicator + last update clock

Row per line:
- "77 Alewife"      [ 7m ]   (bar)
- "1 Harvard"       [ 19m ]  (bar)

Bottom:
- Leave threshold legend: "Leave <= 6m" (or icon)

## Visualization
For each line, render a horizontal bar that maps minutes to proximity.
Rules:
- Define a `MAX_WINDOW_MINUTES` (default 30). Anything >= max shows an empty bar.
- Define a `LEAVE_THRESHOLD_MINUTES` (default 6).
- Bar fill increases as minutes decrease:
  fill = clamp( (MAX_WINDOW - minutes) / MAX_WINDOW, 0..1 )

Threshold:
- Draw a vertical tick at the point corresponding to LEAVE_THRESHOLD.
- If minutes <= threshold, show an attention marker (e.g., filled circle or "GO").

## States
- OK: show minutes
- STALE: show minutes with "*" marker (or small "S")
- NO_SERVICE: show "--" and "No svc"
- ERROR: show "Err"

## Refresh policy
- Prefer partial refresh if supported by the display driver.
- Do a full refresh periodically to prevent ghosting:
  - every N updates (default 20) or every 6 hours, whichever comes first.
