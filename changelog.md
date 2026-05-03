### <c-8BC81A>1.5.6 (5/2/2026)</c>

- Fixed <c-EDFFB2>**edit object menu**</c> crashing on iOS

### <c-B6C81A>1.5.5 (5/1/2026)</c>

- Reenabled <c-FAFFB2>**multi-edit HSV**</c> on all platforms
- Fixed <c-FEFFB2>**incompatibility with Tinker**</c> causing menu buttons to shift after pasting state
- Fixed BetterEdit's <c-FFFBB2>**paste state keybind**</c> opening multiple popups
- Fixed the <c-FFF7B2>**reset button**</c> in Advanced Filter sometimes causing weird UI updates

### <c-C8AE1A>1.5.4 (3/5/2026)</c>

- Fixed <c-FFF0B2>**crash when applying groups**</c> in Add Random Groups
- Fixed <c-FFEDB2>**crash when closing HSV live overlay**</c> with an input selected
- Fixed <c-FFE9B2>**inputs passing through**</c> HSV live overlay
- Fixed next free material button <c-FFE6B2>**always returning 0**</c> if any objects have a negative material ID
- Fixed <c-FFE3B2>**item IDs being limited to 0-999**</c> in multi-edit

### <c-C8821A>1.5.3 (3/2/2026)</c>

- Temporarily disabled <c-FFDBB2>**multi-edit HSV on all platforms except Windows**</c> due to TulipHook bug
- Fixed multi-edit HSV <c-FFD6B2>**not preserving absolute saturation/brightness**</c>
- Fixed <c-FFD1B2>**several crashes**</c> with the live HSV overlay

### <c-C8561A>1.5.2 (2/25/2026)</c>

- Migrated to <c-FFC9B2>**Geode v5 (2.2081)**</c>
- Added <c-FFC5B2>**number inputs**</c> to all HSV widgets
- Fixed <c-FFC1B2>**Named Editor Group buttons**</c> being removed in some triggers
- Fixed <c-FFBDB2>**Add Random Groups popup**</c> having an enter animation

### <c-C82A1A>1.5.1 (1/14/2026)</c>

- Added <c-FFB7B2>**ranges and series**</c> to Advanced Filter
- Fixed <c-FFB4B2>**crash when using multi-edit**</c> with HSV
- Fixed <c-FFB2B2>**HSV sometimes showing as NaN**</c>
- Fixed multi-edit not working in <c-FFB2B4>**shake, animate, and follow player Y triggers**</c>
- Fixed next free button <c-FFB2B7>**overlapping buttons**</c> in edit area trigger
- Fixed next free button <c-FFB2B9>**not showing**</c> in some inputs
- Fixed labels being <c-FFB2BC>**too long**</c> in follow trigger

### <c-C81A35>1.5.0 (1/8/2026)</c>

- Added <c-FFB2BF>**select all button**</c> to Advanced Filter
- Added <c-FFB2C1>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-FFB2C2>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-FFB2C3>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-FFB2C5>**Hide UI button**</c> not working on desktop
- Fixed <c-FFB2C6>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2C7>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2C9>**copy color ID inputs**</c>
- Fixed quick paste button <c-FFB2CA>**overriding the last used preset**</c>
- Fixed <c-FFB2CB>**objects being unselectable**</c> after pasting position state
- Fixed <c-FFB2CD>**visual errors**</c> after pasting editor layer state
- Fixed <c-FFB2CE>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-FFB2CF>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-C81A60>1.4.0 (6/29/2025)</c>

- Added <c-FFB2D3>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FFB2D6>**only support one trigger**</c>
- Added setting to <c-FFB2D9>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFB2DC>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFB2DE>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFB2E1>**Paste State keybind**</c>

### <c-C81A8B>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFB2EA>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FFB2F0>**multiple multi-edit popups**</c> to appear

### <c-C81AB6>1.3.2 (5/3/2025)</c>

- Added button to <c-FFB2FA>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-FFB2FE>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-FBB2FF>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-F7B2FF>**guides not appearing**</c> when using a slider in particle triggers

### <c-AE1AC8>1.3.1 (5/1/2025)</c>

- Added <c-EFB2FF>**iOS support**</c>
- Added Mac support for <c-EBB2FF>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-E7B2FF>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-E4B2FF>**no valid objects**</c>

### <c-821AC8>1.3.0 (4/24/2025)</c>

- Added <c-DDB2FF>**Advanced Paste State**</c>
- Added multi-edit mode to <c-DAB2FF>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-D7B2FF>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-D5B2FF>**follow mod X and Y**</c>
- Fixed <c-D2B2FF>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-CFB2FF>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-561AC8>1.2.1 (2/16/2025)</c>

- Added ability to <c-C8B2FF>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-C3B2FF>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-BEB2FF>**invalid string**</c> into a number input

### <c-2A1AC8>1.2.0 (1/31/2025)</c>

- Added <c-B5B2FF>**Improved Select Filter**</c>
- Fixed <c-B2B3FF>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-B2B6FF>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-B2BAFF>**sliders being low opacity**</c> after hiding the UI

### <c-1A35C8>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-B2C7FF>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-1A60C8>1.1.0 (1/1/2025)</c>

- Migrated to <c-B2D4FF>**Geode v4 (2.2074)**</c>
- Added <c-B2D7FF>**Multi-Edit Mode**</c> to triggers
- Added <c-B2DAFF>**Improved Edit Triggers Popup**</c>
- Added <c-B2DDFF>**Hide UI**</c> button to triggers
- Removed <c-B2E1FF>**10 group limit**</c> in Add Random Groups popup

### <c-1A8BC8>1.0.0 (3/30/2024)</c>

- Added <c-B2EDFF>**Add Random Groups**</c>
