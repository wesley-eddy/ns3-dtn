All timestamps use the simulator time, not DTN epoch time, so SDNVs carrying
timestamps may be a bit smaller than if the DTN epoch was used.

Fragments are not reassembled before delivery, so sending a single large bundle
results in multiple smaller ones queued for Receive by the application.

Custody transfer for fragmented bundles is buggy.
- transmitted fragments are not stored as individual bundles, only the complete bundle
- fragment offset and extent are not checked in custody signals
- lots of ambiguity in RFC 5050 about how exactly fragmentation works with
  regard to storage, refragmentation, custody transfer, overlaps, etc.

All transmissions use a single static custody transfer retransmission timer.
This should be more granular, and possibly dynamic.

At the moment, ACS support is a binary on/off for the agent.  If enabled, ACS
will be used for all bundles sent with custody transfer requested.  This could
be improved to be more granular, e.g. tunable per-destination, per-flow, etc.

ACS-enabled receivers use a single static timer for aggregating ACS data before
sending the admin record with the ACS message.

