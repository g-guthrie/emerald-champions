# Trade handoff pass — September 19, 2026

Partial source audit of party/mail transfer, NPC trade selection, cable/wireless
animation handoffs and evolution callback ownership. Not full link-protocol,
connection-loss, PC-received evolution, or Mail-capacity verification.

Consolidated NPC trade completion into FinishInGameTrade, using the active
wireless-style path's VAR_0x8004 selection and PC-aware pointer choice. The dormant
NPC cable branch previously passed VAR_0x8005 (trade definition ID) to TradeMons.
Both presentation styles now share the correct transaction/evolution handoff.
Current NPC scene initialization selects wireless-style, so the mismatch was
latent rather than a demonstrated current player-facing wrong-slot trade.
Removed the byte-loop Trade_Memcpy wrapper; the non-overlapping link transfer
buffers now use memcpy directly with identical sizes.

Native test invokes the shared completion transaction with selected slots0/2
and trade ID1, confirming received species/item/friendship and unchanged other
party slots, plus the sent Pokemon in the partner buffer. Does not launch a
trade evolution or simulate a network connection. Evidence: work/trade-audit-20260919/.

Outstanding seams: TradeMons copies a PC-received Pokemon to storage before a
possible trade evolution; no post-evolution PC writeback was found in the inspected
callbacks. Current Hoenn NPC reward species do not use trade evolution, so trace
actual reachable suppliers before changing persistence. The PC Mail branch also
uses incoming mail indices with TakeMailFromMon (local record clearing); current
Hoenn NPC rewards have no Mail, but FRLG/expanded trade suppliers need separate
capacity/identity handling. These are recorded limitations, not certified fixes.
Authored AI, encounter tables and save format remain unchanged.
