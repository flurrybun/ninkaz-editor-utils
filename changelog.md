### <c-8BC81A>1.5.4 (3/5/2026)</c>

- Fixed <c-E7FFB2>**crash when applying groups**</c> in Add Random Groups
- Fixed <c-EAFFB2>**crash when closing HSV live overlay**</c> with an input selected
- Fixed <c-EDFFB2>**inputs passing through**</c> HSV live overlay
- Fixed next free material button <c-F0FFB2>**always returning 0**</c> if any objects have a negative material ID
- Fixed <c-F3FFB2>**item IDs being limited to 0-999**</c> in multi-edit

### <c-B6C81A>1.5.3 (3/2/2026)</c>

- Temporarily disabled <c-FBFFB2>**multi-edit HSV on all platforms except Windows**</c> due to TulipHook bug
- Fixed multi-edit HSV <c-FFFDB2>**not preserving absolute saturation/brightness**</c>
- Fixed <c-FFF8B2>**several crashes**</c> with the live HSV overlay

### <c-C8AE1A>1.5.2 (2/25/2026)</c>

- Migrated to <c-FFEFB2>**Geode v5 (2.2081)**</c>
- Added <c-FFEBB2>**number inputs**</c> to all HSV widgets
- Fixed <c-FFE7B2>**Named Editor Group buttons**</c> being removed in some triggers
- Fixed <c-FFE4B2>**Add Random Groups popup**</c> having an enter animation

### <c-C8821A>1.5.1 (1/14/2026)</c>

- Added <c-FFDDB2>**ranges and series**</c> to Advanced Filter
- Fixed <c-FFDBB2>**crash when using multi-edit**</c> with HSV
- Fixed <c-FFD8B2>**HSV sometimes showing as NaN**</c>
- Fixed multi-edit not working in <c-FFD6B2>**shake, animate, and follow player Y triggers**</c>
- Fixed next free button <c-FFD4B2>**overlapping buttons**</c> in edit area trigger
- Fixed next free button <c-FFD1B2>**not showing**</c> in some inputs
- Fixed labels being <c-FFCFB2>**too long**</c> in follow trigger

### <c-C8561A>1.5.0 (1/8/2026)</c>

- Added <c-FFCBB2>**select all button**</c> to Advanced Filter
- Added <c-FFCAB2>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-FFC8B2>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-FFC7B2>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-FFC5B2>**Hide UI button**</c> not working on desktop
- Fixed <c-FFC4B2>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-FFC3B2>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-FFC1B2>**copy color ID inputs**</c>
- Fixed quick paste button <c-FFC0B2>**overriding the last used preset**</c>
- Fixed <c-FFBFB2>**objects being unselectable**</c> after pasting position state
- Fixed <c-FFBDB2>**visual errors**</c> after pasting editor layer state
- Fixed <c-FFBCB2>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-FFBAB2>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-C82A1A>1.4.0 (6/29/2025)</c>

- Added <c-FFB6B2>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FFB4B2>**only support one trigger**</c>
- Added setting to <c-FFB2B3>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFB2B6>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFB2B8>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFB2BB>**Paste State keybind**</c>

### <c-C81A35>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFB2C4>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FFB2CA>**multiple multi-edit popups**</c> to appear

### <c-C81A60>1.3.2 (5/3/2025)</c>

- Added button to <c-FFB2D5>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-FFB2D8>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-FFB2DC>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-FFB2E0>**guides not appearing**</c> when using a slider in particle triggers

### <c-C81A8B>1.3.1 (5/1/2025)</c>

- Added <c-FFB2E7>**iOS support**</c>
- Added Mac support for <c-FFB2EB>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-FFB2EF>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-FFB2F3>**no valid objects**</c>

### <c-C81AB6>1.3.0 (4/24/2025)</c>

- Added <c-FFB2F9>**Advanced Paste State**</c>
- Added multi-edit mode to <c-FFB2FC>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-FEB2FF>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-FBB2FF>**follow mod X and Y**</c>
- Fixed <c-F9B2FF>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-F6B2FF>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-AE1AC8>1.2.1 (2/16/2025)</c>

- Added ability to <c-EEB2FF>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-E9B2FF>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-E5B2FF>**invalid string**</c> into a number input

### <c-821AC8>1.2.0 (1/31/2025)</c>

- Added <c-DCB2FF>**Improved Select Filter**</c>
- Fixed <c-D8B2FF>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-D4B2FF>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-D0B2FF>**sliders being low opacity**</c> after hiding the UI

### <c-561AC8>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-C3B2FF>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-2A1AC8>1.1.0 (1/1/2025)</c>

- Migrated to <c-B6B2FF>**Geode v4 (2.2074)**</c>
- Added <c-B3B2FF>**Multi-Edit Mode**</c> to triggers
- Added <c-B2B4FF>**Improved Edit Triggers Popup**</c>
- Added <c-B2B8FF>**Hide UI**</c> button to triggers
- Removed <c-B2BBFF>**10 group limit**</c> in Add Random Groups popup

### <c-1A35C8>1.0.0 (3/30/2024)</c>

- Added <c-B2C7FF>**Add Random Groups**</c>
