### <c-8BC81A>1.5.7 (7/21/2026)</c>

- Fixed <c-E8FFB2>**stale multi-edit button**</c> crashing in HSV live overlay
- Fixed BetterEdit's <c-EDFFB2>**paste state keybind**</c> not working (again)
- Fixed the paste state button <c-F2FFB2>**staying invisible**</c> when backing out of the Improved Paste State menu

### <c-B6C81A>1.5.6 (5/2/2026)</c>

- Fixed <c-FFFDB2>**edit object menu**</c> crashing on iOS

### <c-C8AE1A>1.5.5 (5/1/2026)</c>

- Reenabled <c-FFEFB2>**multi-edit HSV**</c> on all platforms
- Fixed <c-FFEBB2>**incompatibility with Tinker**</c> causing menu buttons to shift after pasting state
- Fixed BetterEdit's <c-FFE7B2>**paste state keybind**</c> opening multiple popups
- Fixed the <c-FFE4B2>**reset button**</c> in Advanced Filter sometimes causing weird UI updates

### <c-C8821A>1.5.4 (3/5/2026)</c>

- Fixed <c-FFDDB2>**crash when applying groups**</c> in Add Random Groups
- Fixed <c-FFD9B2>**crash when closing HSV live overlay**</c> with an input selected
- Fixed <c-FFD6B2>**inputs passing through**</c> HSV live overlay
- Fixed next free material button <c-FFD3B2>**always returning 0**</c> if any objects have a negative material ID
- Fixed <c-FFD0B2>**item IDs being limited to 0-999**</c> in multi-edit

### <c-C8561A>1.5.3 (3/2/2026)</c>

- Temporarily disabled <c-FFC8B2>**multi-edit HSV on all platforms except Windows**</c> due to TulipHook bug
- Fixed multi-edit HSV <c-FFC3B2>**not preserving absolute saturation/brightness**</c>
- Fixed <c-FFBEB2>**several crashes**</c> with the live HSV overlay

### <c-C82A1A>1.5.2 (2/25/2026)</c>

- Migrated to <c-FFB5B2>**Geode v5 (2.2081)**</c>
- Added <c-FFB2B3>**number inputs**</c> to all HSV widgets
- Fixed <c-FFB2B6>**Named Editor Group buttons**</c> being removed in some triggers
- Fixed <c-FFB2BA>**Add Random Groups popup**</c> having an enter animation

### <c-C81A35>1.5.1 (1/14/2026)</c>

- Added <c-FFB2C0>**ranges and series**</c> to Advanced Filter
- Fixed <c-FFB2C3>**crash when using multi-edit**</c> with HSV
- Fixed <c-FFB2C5>**HSV sometimes showing as NaN**</c>
- Fixed multi-edit not working in <c-FFB2C7>**shake, animate, and follow player Y triggers**</c>
- Fixed next free button <c-FFB2CA>**overlapping buttons**</c> in edit area trigger
- Fixed next free button <c-FFB2CC>**not showing**</c> in some inputs
- Fixed labels being <c-FFB2CE>**too long**</c> in follow trigger

### <c-C81A60>1.5.0 (1/8/2026)</c>

- Added <c-FFB2D2>**select all button**</c> to Advanced Filter
- Added <c-FFB2D3>**next free collision block, item, timer, control, and material ID**</c> buttons
- Added <c-FFB2D5>**next free buttons**</c> to trigger menus which previously lacked them, such as alpha and collision
- Fixed <c-FFB2D6>**Saw Rotation Preview**</c> displaying incorrectly on non-16:9 aspect ratios
- Fixed <c-FFB2D8>**Hide UI button**</c> not working on desktop
- Fixed <c-FFB2D9>**softlock with multi-edit**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2DA>**several inputs**</c> in advanced follow
- Fixed multi-edit not setting <c-FFB2DC>**copy color ID inputs**</c>
- Fixed quick paste button <c-FFB2DD>**overriding the last used preset**</c>
- Fixed <c-FFB2DE>**objects being unselectable**</c> after pasting position state
- Fixed <c-FFB2E0>**visual errors**</c> after pasting editor layer state
- Fixed <c-FFB2E1>**object editor layer updating**</c> after pasting object ID state
- Fixed <c-FFB2E2>**incorrect label shown**</c> when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### <c-C81A8B>1.4.0 (6/29/2025)</c>

- Added <c-FFB2E6>**Saw Rotation Preview**</c>
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus <c-FFB2E9>**only support one trigger**</c>
- Added setting to <c-FFB2EC>**change the hide trigger UI key**</c>, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support <c-FFB2EE>**JIT-less iOS**</c>
- Fixed multi-edit HSV <c-FFB2F1>**incorrectly showing mixed state**</c> when there is none
- Fixed crash when using BetterEdit's <c-FFB2F4>**Paste State keybind**</c>

### <c-C81AB6>1.3.3 (5/4/2025)</c>

- Fixed Android crash when <c-FFB2FD>**using a slider in particle triggers**</c> <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing <c-FAB2FF>**multiple multi-edit popups**</c> to appear

### <c-AE1AC8>1.3.2 (5/3/2025)</c>

- Added button to <c-EFB2FF>**quickly paste default state**</c> in the Advanced Paste State menu
- Added setting to <c-EBB2FF>**disable Advanced Paste State**</c>
- Fixed multi-edit mode being <c-E7B2FF>**completely broken**</c> <c-9CA0AF>(my bad yall)</c>
- Fixed <c-E4B2FF>**guides not appearing**</c> when using a slider in particle triggers

### <c-821AC8>1.3.1 (5/1/2025)</c>

- Added <c-DCB2FF>**iOS support**</c>
- Added Mac support for <c-D8B2FF>**right clicking on inputs**</c> to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when <c-D4B2FF>**opening Advanced Paste State menu**</c>
- Fixed crash with multi-edit mode when there are <c-D0B2FF>**no valid objects**</c>

### <c-561AC8>1.3.0 (4/24/2025)</c>

- Added <c-CAB2FF>**Advanced Paste State**</c>
- Added multi-edit mode to <c-C7B2FF>**HSV sliders**</c>
- Added hide UI and multi-edit support to <c-C4B2FF>**particle triggers, collision triggers, and collision blocks**</c>
- Fixed multi-edit not setting <c-C1B2FF>**follow mod X and Y**</c>
- Fixed <c-BFB2FF>**minor visual bugs**</c> with next free group buttons
- Fixed mixed buttons <c-BCB2FF>**not hiding properly**</c> in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### <c-2A1AC8>1.2.1 (2/16/2025)</c>

- Added ability to <c-B4B2FF>**right click on an input**</c> to enter multi-edit mode (Windows only)
- Added <c-B2B4FF>**next free button**</c> to target & center group controls
- Fixed crashes caused by entering an <c-B2B9FF>**invalid string**</c> into a number input

### <c-1A35C8>1.2.0 (1/31/2025)</c>

- Added <c-B2C2FF>**Improved Select Filter**</c>
- Fixed <c-B2C5FF>**unclickable button**</c> in Add Random Groups popup
- Fixed crash when <c-B2C9FF>**enabling multi-edit mode**</c> after hiding the UI
- Fixed <c-B2CDFF>**sliders being low opacity**</c> after hiding the UI

### <c-1A60C8>v1.1.1 (1/2/2025)</c>

- Fixed duration controls being <c-B2DAFF>**limited to whole numbers**</c> in Improved Edit Triggers Popup

### <c-1A8BC8>1.1.0 (1/1/2025)</c>

- Migrated to <c-B2E7FF>**Geode v4 (2.2074)**</c>
- Added <c-B2EAFF>**Multi-Edit Mode**</c> to triggers
- Added <c-B2EDFF>**Improved Edit Triggers Popup**</c>
- Added <c-B2F0FF>**Hide UI**</c> button to triggers
- Removed <c-B2F3FF>**10 group limit**</c> in Add Random Groups popup

### <c-1AB6C8>1.0.0 (3/30/2024)</c>

- Added <c-B2FFFD>**Add Random Groups**</c>
