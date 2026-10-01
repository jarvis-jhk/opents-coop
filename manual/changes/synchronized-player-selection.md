---
title: Synchronize networked selection triggers and keep control groups local
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_SELECTED
  effect: changed
- type: key
  id: Group
  scope: teamtype
  effect: changed
credit:
- jarvis-jhk
---

Networked selections raise [Selected by player](/mapping/events/tevent-selected/) through synchronized orders instead of changing triggers locally. Networked control groups remain local without changing the object’s [recruitment group](/keys/group/#scope-teamtype). Campaigns and skirmishes played alone keep their group behavior.

Use matching fork builds for network play; the synchronized selection order adds a network event. Existing object and team group values keep their meanings.
