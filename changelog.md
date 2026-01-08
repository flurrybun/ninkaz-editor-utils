### <c-8BC81A>1.5.0 (1/8/2026)</c>

- Added <c-E5FFB2>**select all button**</c> to Advanced Filter
- Added <c-E6FFB2>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-E8FFB2>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-E9FFB2>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-EAFFB2>**HSV sometimes showing as NaN**</c>
- Fixed <c-ECFFB2>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-EDFFB2>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-EEFFB2>**copy color ID inputs**</c>
- Fixed quick paste button <c-F0FFB2>**overriding the last used preset**</c>
- Fixed <c-F1FFB2>**objects being unselectable**</c> after pasting position state
- Fixed <c-F3FFB2>**visual errors**</c> after pasting editor layer state
- Fixed <c-F4FFB2>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-F5FFB2>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-B6C81A>1.4.0 (6/29/2025)</c>

- Added <c-F9FFB2>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FCFFB2>**only support one trigger**</c>
- Added setting to <c-FFFEB2>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFFBB2>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFF9B2>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFF6B2>**Paste State keybind**</c>

### <c-C8AE1A>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFEDB2>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FFE6B2>**multiple multi-edit popups**</c> to appear

### <c-C8821A>1.3.2 (5/3/2025)</c>

- Added button to <c-FFDCB2>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-FFD8B2>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-FFD4B2>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-FFD0B2>**guides not appearing**</c> when using a slider in particle triggers

### <c-C8561A>1.3.1 (5/1/2025)</c>

- Added <c-FFC9B2>**iOS support**</c>
- Added Mac support for <c-FFC5B2>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-FFC1B2>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-FFBDB2>**no valid objects**</c>

### <c-C82A1A>1.3.0 (4/24/2025)</c>

- Added <c-FFB6B2>**Advanced Paste State**</c>
- Added multi-edit mode to <c-FFB4B2>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-FFB2B3>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-FFB2B6>**follow mod X and Y**</c>
- Fixed <c-FFB2B8>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-FFB2BB>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-C81A35>1.2.1 (2/16/2025)</c>

- Added ability to <c-FFB2C3>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-FFB2C7>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-FFB2CC>**invalid string**</c> into a number input

### <c-C81A60>1.2.0 (1/31/2025)</c>

- Added <c-FFB2D5>**Improved Select Filter**</c>
- Fixed <c-FFB2D8>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-FFB2DC>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-FFB2E0>**sliders being low opacity**</c> after hiding the UI

### <c-C81A8B>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-FFB2ED>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-C81AB6>1.1.0 (1/1/2025)</c>

- Added <c-FFB2FA>**2.2074 support**</c>
- Added <c-FFB2FD>**Multi-Edit Mode**</c> to triggers
- Added <c-FDB2FF>**Improved Edit Triggers Popup**</c>
- Added <c-FAB2FF>**Hide UI**</c> button to triggers
- Removed <c-F6B2FF>**10 group limit**</c> in Add Random Groups popup

### <c-AE1AC8>1.0.0 (3/30/2024)</c>

- Added <c-E9B2FF>**Add Random Groups**</c>
