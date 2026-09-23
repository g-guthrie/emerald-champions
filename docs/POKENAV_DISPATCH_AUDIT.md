# Pokenav dispatch pass — September 20, 2026

Partial review: callback table/menu transitions, retired Match Call hole, exit
ownership and resource helpers; Start-menu reload confirm/cancel callbacks.
Added menu-ID bound check before indexing/initializing callbacks. Unsigned
subtraction also rejects IDs below the menu base. The retired Match Call entry
remains rejected without changing stable enum values. No active invalid ID
producer identified; this is dispatch hardening, not proof of an ordinary crash.

Removed internal InitKeys_ and IsActiveMenuLoopTaskActive_ pass-through wrappers;
call their existing implementations directly. Valid initialization/order unchanged.
Native test rejects the retired ID and below/above-range IDs without allocated
Pokenav UI/resources. Evidence: work/pokenav-audit-20260920/. Full Map/Ribbons UI
replay, resource-failure paths and Start-menu save/reload flows remain to verify.
Prior rematch-removal native menu evidence is historical, not refreshed here.

Start-menu ReloadSave cancel clears its message, reinitializes the Start menu and
returns to HandleStartMenuInput; confirmation fades then delegates ReloadLastSave.
No source change was warranted in that path. Authored AI, save format and menu
contents unchanged.
