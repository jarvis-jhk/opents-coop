---
key: VariantName
scope: scenarios
label: Campaign version label
summary: Sets the mission version label shown before loading a campaign mission.
when_omitted:
  kind: computed
  note: The original is labeled Original and a variant uses its filename tag.
---

`[Basic] VariantName=` names this version in the campaign mission chooser. For example, `VariantName=Two bases` labels a two-force variant. Omitting it labels the original `Original` and a variant with its filename tag. See [Campaign mission variants](/formats/campaign-variants/) for filenames and force declarations.
