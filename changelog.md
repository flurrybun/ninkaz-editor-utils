### <c-8BC81A>1.5.2 (2/25/2026)</c>

- Migrated to <c-E7FFB2>**Geode v5 (2.2081)**</c>
- Added <c-EBFFB2>**number inputs**</c> to all HSV widgets
- Fixed <c-EFFFB2>**Named Editor Group buttons**</c> being removed in some triggers
- Fixed <c-F3FFB2>**Add Random Groups popup**</c> having an enter animation

### <c-B6C81A>1.5.1 (1/14/2026)</c>

- Added <c-F9FFB2>**ranges and series**</c> to Advanced Filter
- Fixed <c-FBFFB2>**crash when using multi-edit**</c> with HSV
- Fixed <c-FEFFB2>**HSV sometimes showing as NaN**</c>
- Fixed multi-edit not working in <c-FFFDB2>**shake, animate, and follow player Y triggers**</c>
- Fixed next free button <c-FFFAB2>**overlapping buttons**</c> in edit area trigger
- Fixed next free button <c-FFF8B2>**not showing**</c> in some inputs
- Fixed labels being <c-FFF5B2>**too long**</c> in follow trigger

### <c-C8AE1A>1.5.0 (1/8/2026)</c>

- Added <c-FFF2B2>**select all button**</c> to Advanced Filter
- Added <c-FFF0B2>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-FFEFB2>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-FFEEB2>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-FFECB2>**Hide UI button**</c> not working on desktop
- Fixed <c-FFEBB2>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-FFE9B2>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-FFE8B2>**copy color ID inputs**</c>
- Fixed quick paste button <c-FFE7B2>**overriding the last used preset**</c>
- Fixed <c-FFE5B2>**objects being unselectable**</c> after pasting position state
- Fixed <c-FFE4B2>**visual errors**</c> after pasting editor layer state
- Fixed <c-FFE2B2>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-FFE1B2>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-C8821A>1.4.0 (6/29/2025)</c>

- Added <c-FFDDB2>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FFDAB2>**only support one trigger**</c>
- Added setting to <c-FFD7B2>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFD5B2>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFD2B2>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFCFB2>**Paste State keybind**</c>

### <c-C8561A>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFC6B2>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FFBFB2>**multiple multi-edit popups**</c> to appear

### <c-C82A1A>1.3.2 (5/3/2025)</c>

- Added button to <c-FFB5B2>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-FFB2B3>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-FFB2B6>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-FFB2BA>**guides not appearing**</c> when using a slider in particle triggers

### <c-C81A35>1.3.1 (5/1/2025)</c>

- Added <c-FFB2C2>**iOS support**</c>
- Added Mac support for <c-FFB2C5>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-FFB2C9>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-FFB2CD>**no valid objects**</c>

### <c-C81A60>1.3.0 (4/24/2025)</c>

- Added <c-FFB2D3>**Advanced Paste State**</c>
- Added multi-edit mode to <c-FFB2D6>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-FFB2D9>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-FFB2DC>**follow mod X and Y**</c>
- Fixed <c-FFB2DE>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-FFB2E1>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-C81A8B>1.2.1 (2/16/2025)</c>

- Added ability to <c-FFB2E8>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-FFB2ED>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-FFB2F2>**invalid string**</c> into a number input

### <c-C81AB6>1.2.0 (1/31/2025)</c>

- Added <c-FFB2FA>**Improved Select Filter**</c>
- Fixed <c-FFB2FE>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-FBB2FF>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-F7B2FF>**sliders being low opacity**</c> after hiding the UI

### <c-AE1AC8>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-E9B2FF>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-821AC8>1.1.0 (1/1/2025)</c>

- Migrated to <c-DCB2FF>**Geode v4 (2.2074)**</c>
- Added <c-D9B2FF>**Multi-Edit Mode**</c> to triggers
- Added <c-D6B2FF>**Improved Edit Triggers Popup**</c>
- Added <c-D3B2FF>**Hide UI**</c> button to triggers
- Removed <c-D0B2FF>**10 group limit**</c> in Add Random Groups popup

### <c-561AC8>1.0.0 (3/30/2024)</c>

- Added <c-C3B2FF>**Add Random Groups**</c>
