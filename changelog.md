### <c-8BC81A>1.5.5 (5/1/2026)</c>

- Reenabled <c-E7FFB2>**multi-edit HSV**</c> on all platforms
- Fixed <c-EBFFB2>**incompatibility with Tinker**</c> causing menu buttons to shift after pasting state
- Fixed BetterEdit's <c-EFFFB2>**paste state keybind**</c> opening multiple popups
- Fixed the <c-F3FFB2>**reset button**</c> in Advanced Filter sometimes causing weird UI updates

### <c-B6C81A>1.5.4 (3/5/2026)</c>

- Fixed <c-FAFFB2>**crash when applying groups**</c> in Add Random Groups
- Fixed <c-FDFFB2>**crash when closing HSV live overlay**</c> with an input selected
- Fixed <c-FFFDB2>**inputs passing through**</c> HSV live overlay
- Fixed next free material button <c-FFFAB2>**always returning 0**</c> if any objects have a negative material ID
- Fixed <c-FFF6B2>**item IDs being limited to 0-999**</c> in multi-edit

### <c-C8AE1A>1.5.3 (3/2/2026)</c>

- Temporarily disabled <c-FFEEB2>**multi-edit HSV on all platforms except Windows**</c> due to TulipHook bug
- Fixed multi-edit HSV <c-FFE9B2>**not preserving absolute saturation/brightness**</c>
- Fixed <c-FFE5B2>**several crashes**</c> with the live HSV overlay

### <c-C8821A>1.5.2 (2/25/2026)</c>

- Migrated to <c-FFDCB2>**Geode v5 (2.2081)**</c>
- Added <c-FFD8B2>**number inputs**</c> to all HSV widgets
- Fixed <c-FFD4B2>**Named Editor Group buttons**</c> being removed in some triggers
- Fixed <c-FFD0B2>**Add Random Groups popup**</c> having an enter animation

### <c-C8561A>1.5.1 (1/14/2026)</c>

- Added <c-FFCAB2>**ranges and series**</c> to Advanced Filter
- Fixed <c-FFC8B2>**crash when using multi-edit**</c> with HSV
- Fixed <c-FFC5B2>**HSV sometimes showing as NaN**</c>
- Fixed multi-edit not working in <c-FFC3B2>**shake, animate, and follow player Y triggers**</c>
- Fixed next free button <c-FFC0B2>**overlapping buttons**</c> in edit area trigger
- Fixed next free button <c-FFBEB2>**not showing**</c> in some inputs
- Fixed labels being <c-FFBBB2>**too long**</c> in follow trigger

### <c-C82A1A>1.5.0 (1/8/2026)</c>

- Added <c-FFB8B2>**select all button**</c> to Advanced Filter
- Added <c-FFB6B2>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-FFB5B2>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-FFB4B2>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-FFB2B2>**Hide UI button**</c> not working on desktop
- Fixed <c-FFB2B3>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2B4>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2B6>**copy color ID inputs**</c>
- Fixed quick paste button <c-FFB2B7>**overriding the last used preset**</c>
- Fixed <c-FFB2B8>**objects being unselectable**</c> after pasting position state
- Fixed <c-FFB2BA>**visual errors**</c> after pasting editor layer state
- Fixed <c-FFB2BB>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-FFB2BD>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-C81A35>1.4.0 (6/29/2025)</c>

- Added <c-FFB2C1>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FFB2C3>**only support one trigger**</c>
- Added setting to <c-FFB2C6>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFB2C9>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFB2CB>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFB2CE>**Paste State keybind**</c>

### <c-C81A60>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFB2D7>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FFB2DD>**multiple multi-edit popups**</c> to appear

### <c-C81A8B>1.3.2 (5/3/2025)</c>

- Added button to <c-FFB2E7>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-FFB2EB>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-FFB2EF>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-FFB2F3>**guides not appearing**</c> when using a slider in particle triggers

### <c-C81AB6>1.3.1 (5/1/2025)</c>

- Added <c-FFB2FA>**iOS support**</c>
- Added Mac support for <c-FFB2FE>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-FBB2FF>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-F7B2FF>**no valid objects**</c>

### <c-AE1AC8>1.3.0 (4/24/2025)</c>

- Added <c-F0B2FF>**Advanced Paste State**</c>
- Added multi-edit mode to <c-EEB2FF>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-EBB2FF>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-E8B2FF>**follow mod X and Y**</c>
- Fixed <c-E5B2FF>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-E2B2FF>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-821AC8>1.2.1 (2/16/2025)</c>

- Added ability to <c-DBB2FF>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-D6B2FF>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-D1B2FF>**invalid string**</c> into a number input

### <c-561AC8>1.2.0 (1/31/2025)</c>

- Added <c-C9B2FF>**Improved Select Filter**</c>
- Fixed <c-C5B2FF>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-C1B2FF>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-BDB2FF>**sliders being low opacity**</c> after hiding the UI

### <c-2A1AC8>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-B2B4FF>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-1A35C8>1.1.0 (1/1/2025)</c>

- Migrated to <c-B2C1FF>**Geode v4 (2.2074)**</c>
- Added <c-B2C4FF>**Multi-Edit Mode**</c> to triggers
- Added <c-B2C7FF>**Improved Edit Triggers Popup**</c>
- Added <c-B2CAFF>**Hide UI**</c> button to triggers
- Removed <c-B2CEFF>**10 group limit**</c> in Add Random Groups popup

### <c-1A60C8>1.0.0 (3/30/2024)</c>

- Added <c-B2DAFF>**Add Random Groups**</c>
